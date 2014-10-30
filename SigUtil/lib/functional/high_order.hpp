/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_HIGH_ORDER_HPP
#define SIG_UTIL_HIGH_ORDER_HPP

#include "../helper/helper.hpp"
#include "../helper/container_helper.hpp"

/// \file high_order.hpp 高階関数

namespace sig
{
/// n引数高階関数
/**
	(b -> c -> ... -> a) -> [b] -> [c] -> ... -> [a] \n
*/
template <class F, class C1, class... Cs>
auto variadicZipWith(F&& func, C1&& list1, Cs&&... lists)
{
	using CR1 = typename impl::remove_const_reference<C1>::type;
	using R = typename impl::container_traits<CR1>::template rebind<decltype(impl::eval(
		std::forward<F>(func),
		std::declval<typename impl::forward_element<C1>::type>(),
		std::declval<typename impl::forward_element<Cs>::type>()...
	))>;

	R result;
	const uint length = min(list1.size(), lists.size()...);
	iterative_make(length, result, std::forward<F>(func), impl::begin(std::forward<C1>(list1)), impl::begin(std::forward<Cs>(lists))...);

	return result;
}

/// 1引数高階関数
/**
	(a -> b) -> [a] -> [b] \n
	コンテナの各要素に関数を適用し、結果を新たなコンテナに格納する

	\param func 関数 (a -> b)
	\param list データが格納されたコンテナ [a]（\ref sig_container ）

	\return 処理結果のコンテナ [b]（コンテナはlistと同じ種類）
	
	\code
	const array<int, 3> data1{ 1, -3, 5 };	// sig::array
	const std::unordered_set<double> data2{ 0, 1.1, -2.2, 3.3 };

	auto r1 = map(Increment(), data1);
	auto r2 = map([](double v){ return v * 2; }, data2);

	r1;		// array<int, 3>{ 2, -2, 6 }
	r2;		// std::unordered_set<double>{ 0, 2.2, -4.4, 6.6 }
	\endcode
*/
template <class F, class C>
auto map(F&& func, C&& list)
{
	return variadicZipWith(std::forward<F>(func), std::forward<C>(list));
}

/// 2引数高階関数
/**
	(a -> b -> c) -> [a] -> [b] -> [c] \n
	2つのコンテナのindexが対応する各要素に関数を適用し、結果を新たなコンテナに格納する

	\param func 関数 (a -> b -> c)
	\param list1 データが格納されたコンテナ [a]（\ref sig_container ）
	\param list2 データが格納されたコンテナ [b]（\ref sig_container ）

	\return 処理結果のコンテナ [c]（コンテナはlist1と同じ種類）
	
	\code
	const array<int, 3> data1{ 1, -3, 5 };	// sig::array
	const std::set<double> data2{ -2.2, 0 , 1.1, 3.3 };

	auto r = zipWith([](int a, double b){ return (a + b) / 2; }, data1, data2);

	r;		// array<double, 3>{ -6, -1.5, 3.05 }
	\endcode
*/
template <class F, class C1, class C2>
auto zipWith(F&& func, C1&& list1, C2&& list2)
{
	return variadicZipWith(std::forward<F>(func), std::forward<C1>(list1), std::forward<C2>(list2));
}

}
#endif
