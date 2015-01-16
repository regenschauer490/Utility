/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIGUTIL_LIST_DEAL_HPP
#define SIGUTIL_LIST_DEAL_HPP

#include "../helper/helper_modules.hpp"
#include "../helper/container_helper.hpp"

/// \file list_deal.hpp リスト(コンテナ)操作関数

namespace sig
{
/// 値を複製したコンテナを返す
/**
	uint -> a -> [a]

	\param n 複製する数 uint
	\param value 複製する値 a

	\return 結果のコンテナ [a]（defaultは std::vector）

	\code
	auto rep1 = replicate(3, 3.14);					// std::vector<double>{ 3.14, 3.14, 3.14 }
	auto rep2 = replicate(2, std::string("rep"));	// std::vector<std::string>{ "rep", "rep" }
	\endcode
*/
template <class T, class C = std::vector<T>>
auto replicate(uint n, T const& value) ->C
{
	C result = impl::container_traits<C>::make(n);

	for (uint i = 0; i<n; ++i) impl::container_traits<C>::add_element(result, value);

	return result;
}


/// コンテナの要素の順番を逆転させたコンテナを返す
/**
	[a] -> [a]

	\param list ソースとなるコンテナ（\ref sig_container ）

	\return 結果のコンテナ（コンテナはlistと同じ種類）

	\code
	const array<int, 4> data1{ 1, 2, 3, 4 };		// sig::array
	std::vector<int> data2{ 1, -3, 5, 2, 10 };

	auto r1 = reverse(data1);				// array<int, 4>{ 4, 3, 2, 1 }
	auto r2 = reverse(std::move(data2));	// std::vector<int>{ 10, 2, 5, -3, 1 }
	
	assert(data2.size() == 0);
	\endcode
*/
template <class C>
auto reverse(C&& list) ->typename impl::remove_const_reference<C>::type
{
	auto result = std::forward<C>(list);

	std::reverse(std::begin(result), std::end(result));

	return result;
}


/// コンテナの結合（同じコンテナ型）
/**
	[a] -> [a] -> [a] \n
	非シーケンスコンテナの場合、結合後の要素の順番は保たれない

	\param list1 結合後に前に位置するコンテナ（\ref sig_container ）
	\param list2 結合後に後ろに位置するコンテナ（\ref sig_container ）

	\return 結果のコンテナ（コンテナはlist1と同じ種類）

	\sa merge(C1&& list1, C2&& list2)

	\code
	const array<int, 3> data1{ 1, 2, 3 };		// sig::array
	const array<int, 3> data2{ 4, 5, 6 };		// sig::array

	auto m = merge(data1, data2);	// array<int, 6>{ 1, 2, 3, 4 , 5, 6 }
	\endcode
*/
template <class C, class R = typename impl::remove_const_reference<C>::type,
	typename std::enable_if<!impl::static_container_traits<R>::exist>::type*& = enabler
>
auto merge(C&& list1, C&& list2) ->R
{
	const uint length = list1.size() + list2.size();
	R result = impl::container_traits<R>::make(length);

	impl::container_traits<R>::concat(result, std::forward<C>(list1));
	impl::container_traits<R>::concat(result, std::forward<C>(list2));

	return result;
}

namespace impl
{
template <class R, class C1, class C2>
auto merge_impl(C1&& list1, C2&& list2) ->R
{
	const uint length = list1.size() + list2.size();
	R result = impl::container_traits<R>::make(length);

	for (auto it = impl::begin(std::forward<C1>(list1)), end = impl::end(std::forward<C1>(list1)); it != end; ++it){
		impl::container_traits<R>::add_element(result, *it);
	}
	for (auto it = impl::begin(std::forward<C2>(list2)), end = impl::end(std::forward<C2>(list2)); it != end; ++it){
		impl::container_traits<R>::add_element(result, *it);
	}

	return result;
}

}	// impl

// コンテナの結合（異なるコンテナ型）
/**
	[a] -> [b] -> [c] \n
	ただし、cはstd::common_type<a, b>::type \n
	非シーケンスコンテナの場合、結合後の要素の順番は保たれない

	\tparam R [option] 返り値のコンテナ型を指定（\ref sig_container ）

	\param list1 結合後に前に位置するコンテナ（\ref sig_container ）
	\param list2 結合後に後ろに位置するコンテナ（\ref sig_container ）

	\return 結果のコンテナ（コンテナは R もしくは list1と同じ種類）

	\code
	const array<int, 4> data1{ 1, 2, 3, 4 };		// sig::array
	const array<double, 3> data2{ 5.5, 6.6, 7.7 };	// sig::array
	std::vector<int> data3{ 1, -3 };

	auto m1 = merge(data1, data2);		// array<double, 7>{ 1, 2, 3, 4, 5.5, 6.6, 7.7 }
	auto m2 = merge(data2, data3);		// std::vector<double>{ 5.5, 6.6, 7.7, 1, -3 }
	auto m3 = merge<std::set<double, std::greater<double>>>(data1, data2);	// std::set<double>{ 7.7, 6.6, 5.5, 4, 3, 2, 1 }
	\endcode
*/
template <class R = void, class C1, class C2,
	class CR1 = typename impl::remove_const_reference<C1>::type,
	class CR2 = typename impl::remove_const_reference<C2>::type,
	class RT = typename std::common_type<
		typename impl::container_traits<CR1>::value_type,
		typename impl::container_traits<CR2>::value_type
	>::type,
	class RR = typename impl::SameIf<R, void,
		typename impl::remove_const_reference< typename impl::container_traits<CR1>::template rebind<RT>>::type,
		R
	>::type,
	class AT1 = typename impl::forward_element<C1>::type,
	class AT2 = typename impl::forward_element<C2>::type,
	typename std::enable_if<!(impl::static_container_traits<CR1>::exist)>::type*& = enabler
>
auto merge(C1&& list1, C2&& list2) ->RR
{
	return impl::merge_impl<RR>(std::forward<C1>(list1), std::forward<C2>(list2));
}

template <class R = void, class SC, class C,
	class SCR = typename impl::remove_const_reference<SC>::type,
	class CR = typename impl::remove_const_reference<C>::type,
	class RT = typename std::common_type<
		typename impl::static_container_traits<SCR>::value_type,
		typename impl::container_traits<CR>::value_type
	>::type,
	class RR = typename impl::SameIf<R, void,
		typename impl::remove_const_reference< typename impl::container_traits<CR>::template rebind<RT>>::type,
		R
	>::type,
	typename std::enable_if<impl::static_container_traits<SCR>::exist && (!impl::static_container_traits<CR>::exist)>::type*& = enabler
>
auto merge(SC&& list1, C&& list2) ->RR
{
	return impl::merge_impl<RR>(std::forward<SC>(list1), std::forward<C>(list2));
}

template <class R = void, class SC1, class SC2,
	class SCR1 = typename impl::remove_const_reference<SC1>::type,
	class SCR2 = typename impl::remove_const_reference<SC2>::type,
	class RT = typename std::common_type<
		typename impl::static_container_traits<SCR1>::value_type,
		typename impl::static_container_traits<SCR2>::value_type
	>::type,
	class RR = typename impl::SameIf<R, void,
		typename impl::static_container_traits<SCR1>::template rebind_t<RT, impl::plus_t<impl::static_container_traits<SCR1>::size, impl::static_container_traits<SCR2>::size>::value>,
		R
	>::type,
	typename std::enable_if<impl::static_container_traits<SCR1>::exist && impl::static_container_traits<SCR2>::exist>::type*& = enabler
>
auto merge(SC1&& list1, SC2&& list2) ->RR
{
	return impl::merge_impl<RR>(std::forward<SC1>(list1), std::forward<SC2>(list2));
}


/// コンテナの先頭からn個の要素を取り出す
/**
	uint -> [a] -> [a]

	\param n 先頭から順に取り出す要素の個数
	\param list ソースとなるコンテナ（\ref sig_container ）

	\return 結果のコンテナ（listと同じ種類）

	\code
	const array<int, 4> data1{ 1, 2, 3, 4 };		// sig::array
	std::vector<int> data2{ 1, -3, 5, 2, 10 };

	auto t1 = take(2, data1);		// array<int, 4>{ 1, 2 }
	auto t2 = take(3, data2);		// std::vector<int> data2{ 1, -3, 5 }
	\endcode
*/
template <class C,
	class CR = typename impl::remove_const_reference<C>::type
>
auto take(uint n, C&& list) ->CR
{
	CR result = impl::container_traits<CR>::make(n);
	uint i = 0;

	for (auto it = impl::begin(std::forward<C>(list)), end = impl::end(std::forward<C>(list)); i < n && it != end; ++i, ++it) {
		impl::container_traits<CR>::add_element(result, *it);
	}
	return result;
}


/// コンテナの先頭からn個を削除したコンテナを返す
/**
	uint -> [a] -> [a]

	\param n 先頭から順に削除する要素の個数
	\param list ソースとなるコンテナ（\ref sig_container ）

	\return 結果のコンテナ（listと同じ種類）

	\code
	const array<int, 4> data1{ 1, 2, 3, 4 };		// sig::array
	std::vector<int> data2{ 1, -3, 5, 2, 10 };

	auto t1 = drop(2, data1);		// array<int, 4>{ 3, 4 }
	auto t2 = drop(3, data2);		// std::vector<int> data2{ 2, 10 }
	\endcode
*/
template <class C,
	class CR = typename impl::remove_const_reference<C>::type
>
auto drop(uint n, C&& list) ->CR
{
	CR result = impl::container_traits<CR>::make(list.size() - n);
	uint i = 0;
	auto it = impl::begin(list), end = impl::end(list);

	for (; i < n && it != end; ++i, ++it) ;
	for (; it != end; ++i, ++it) impl::container_traits<CR>::add_element(result, *it);

	return result;
}

}
#endif
