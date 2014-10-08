/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_FUNCTIONAL_HPP
#define SIG_UTIL_FUNCTIONAL_HPP

#include "helper/helper.hpp"
#include "helper/container_helper.hpp"
#include "helper/maybe.hpp"

/* 関数型プログラミング サポート */

namespace sig
{
// n引数高階関数
template <class F, class C1, class... Cs>
auto variadicZipWith(F const& func, C1 const& container1, Cs const&... containers)
{
	using R = typename impl::container_traits<C1>::template rebind<decltype(impl::eval(
		func,
		std::declval<typename impl::container_traits<C1>::value_type>(),
		std::declval<typename impl::container_traits<Cs>::value_type>()...
	))>;

	R result;
	const uint length = min(container1.size(), containers.size()...);
	iterative_make(length, result, func, std::begin(container1), std::begin(containers)...);

	return result;
}

// (a -> b) -> [a] -> [b]
// 1引数高階関数
template <class F, class C>
auto map(F const& func, C const& container)
{
	return variadicZipWith<F>(func, container);
}

// (a -> b -> c) -> [a] -> [b] -> [c]
// 2引数高階関数
template <class F, class C1, class C2>
auto zipWith(F const& func, C1 const& container1, C2 const& container2)
{
	return variadicZipWith<F>(func, container1, container2);
}


// (a -> b -> a) -> a0 -> [b] -> a // a0はaに暗黙的に型変換可能
// コンテナの先頭からたたみ込み
template <class F, class T, class C>
auto foldl(F const& func, T init, C const& container)
{
	using R = decltype(func(init, std::declval<typename impl::container_traits<C>::value_type>()));
	return std::accumulate(std::begin(container), std::end(container), static_cast<R>(init), func);
}

#if SIG_GCC_GT_4_9 || SIG_CLANG_GT_3_5 || SIG_MSVC_ENV
// (a -> b -> b) -> b0 -> [a] -> b // b0はbに暗黙的に型変換可能
// コンテナの末尾からたたみ込み
template <class F, class T, class C>
auto foldr(F const& func, T init, C const& container)
{
	using R = typename std::result_of<F(T, typename impl::container_traits<C>::value_type)>::type;//decltype(impl::eval(func, init, std::declval<typename impl::container_traits<C>::value_type>()));
	return std::accumulate(std::rbegin(container), std::rend(container), static_cast<R>(init), std::bind(func, _2, _1));
}
#endif

// (a -> b -> ... -> a) -> a0 -> [b] -> [c] -> ... -> a  // a0はaに暗黙的に型変換可能
// 可変長個のコンテナに対して関数適用して結果を集約する関数
template <class F1, class F2, class T, class... Cs>
auto dotProduct(F1 const& fold_func, F2 const& oper_func, T init, Cs const&... cotainers)
{
	using R = decltype(impl::eval(
		fold_func,
		init,
		impl::eval(oper_func, std::declval<typename impl::container_traits<Cs>::value_type>()...)
	));
	R result = init;
	const uint length = min(containers.size()...);
	
	for (uint i = 0; i < loop; ++i, impl::increment_iterator(iterators...)){
		result = fold_func(result, impl::eval(func, impl::dereference_iterator(iterators)...));
	}
	return result;
}

// (a -> Bool) -> [a] -> [a]
// コンテナから指定条件を満たす要素を抽出する
template <class F, class C>
auto filter(F const& pred, C const& container)
{
	C result;
	for (auto const& e : container){
		if (pred(e)) impl::container_traits<C>::add_element(result, e);
	}
	return result;
}

// (a -> Bool) -> [a] -> ([a], [a])
template <class F, class C>
auto partition(F const& pred, C const& container)
{
	C result1, result2;

	for (auto const& e : container){
		if (pred(e)) impl::container_traits<C>::add_element(result1, e);
		else impl::container_traits<C>::add_element(result2, e);
	}
	return std::make_tuple(std::move(result1), std::move(result2));
}


// [a] -> [b] -> ... -> [(a, b, ...)]
// 複数のコンテナから、タプルのコンテナを作る (第1引数のコンテナが戻り値のコンテナとなる)
// for variadic parameter, const lvalue reference
template <class... Cs
#if !SIG_MSVC_ENV || !(MSC_VER < 1900)
	, typename std::enable_if< And(impl::container_traits<Cs>::exist...) >::type*& = enabler
#endif
	>
auto zip(Cs const&... containers)
{
	return variadicZipWith([](typename impl::container_traits<Cs>::value_type const&... vs){
		return std::make_tuple(vs...);
	}, containers...);
}

#if !SIG_MSVC_ENV || !(_MSC_VER < 1900)
// for variadic parameter, rvalue reference
template <class C1, class... Cs,
	typename std::enable_if<!std::is_lvalue_reference<C1>::value && !And(std::is_lvalue_reference<Cs>::value...)>::type*& = enabler,
	typename std::enable_if< And(impl::container_traits<Cs>::exist...) >::type*& = enabler
>
auto zip(C1&& container1, Cs&&... containers)
{
	using RT = typename impl::container_traits<C1>::template rebind<
		std::tuple<typename impl::container_traits<C1>::value_type, typename impl::container_traits<Cs>::value_type...>
	>;

	const uint length = min(container1.size(), containers.size()...);
	RT result;
	iterative_make(length, result, [](typename impl::container_traits<C1>::value_type&& v1, typename impl::container_traits<Cs>::value_type&&... vs){
		return std::make_tuple(std::move(v1), std::move(vs)...);
	}, std::make_move_iterator(std::begin(container1)), std::make_move_iterator(std::begin(containers))...);

	return result;
}
#endif

#if SIG_GCC_GT4_9_0 || SIG_CLANG_GT_3_5 || !(_MSC_VER < 1900)
template <class TC, size_t... I>
auto ZipImpl_(TC const& t_containers, std::index_sequence<I...>)
{
	return zip(std::get<I>(t_containers)...);
}
template <class TC, size_t... I>
auto ZipImpl_(TC&& t_containers, std::index_sequence<I...>)
{
	return zip(std::get<I>(std::forward<TC>(t_containers))...);
}

// ([a], [b], ...) -> [(a, b, ...)]
// コンテナのタプルから、タプルのコンテナを作る
// for tuple, const lvalue reference
template <class... Cs, typename Indices = std::make_index_sequence<sizeof...(Cs)>
auto zip(std::tuple<Cs...> const& t_containers)
{
	return ZipImpl_(t_containers, Indices());
}

// for tuple, rvalue reference
template <class... Cs, typename Indices = std::make_index_sequence<sizeof...(Cs)>
auto zip(std::tuple<Cs...>&& t_containers)
{
	return ZipImpl_(std::move(t_containers), Indices());
}
#endif

// [(a, b, ...)] -> [a0]
// タプルのコンテナから、指定したindexのコンテナ(templateパラメータで指定)を取り出す
template <size_t Index, class CT>
auto unzip(CT const& c_tuple)
{
	using T = typename std::tuple_element<Index, typename impl::container_traits<CT>::value_type>::type;
	using C = std::vector<T>;
	C result;

	for (auto const& e : c_tuple){
		impl::container_traits<C>::add_element(result, std::get<Index>(e));
	}
	return result;
}

template <uint Index, class CT, typename std::enable_if<!std::is_lvalue_reference<CT>::value>::type*& = enabler>
auto unzip(CT&& c_tuple)
{
	using T = typename std::tuple_element<Index, typename impl::container_traits<CT>::value_type>::type;
	using C = std::vector<T>;
	C result;

	for (auto& t : c_tuple){
		impl::container_traits<C>::add_element(result, std::get<Index>(std::move(t)));
	}
	return result;
}

template<class CT, size_t I = 0, typename std::enable_if<I + 1 == std::tuple_size<typename impl::container_traits<CT>::value_type>::value, void>::type*& = enabler>
auto UnzipImpl_(CT const& c_tuple)
{
	return std::make_tuple(unzip<I>(c_tuple));
}
template<class CT, size_t I = 0, typename std::enable_if<I + 1 < std::tuple_size<typename impl::container_traits<CT>::value_type>::value, void>::type*& = enabler>
auto UnzipImpl_(CT const& c_tuple)
{
	return std::tuple_cat(std::make_tuple(unzip<I>(c_tuple)), UnzipImpl_<CT, I + 1>(c_tuple));
}

template<class CT, uint I = 0, typename std::enable_if<I + 1 == std::tuple_size<typename impl::container_traits<CT>::value_type>::value, void>::type*& = enabler>
auto UnzipImpl_(CT&& c_tuple)
{
	return std::make_tuple(unzip<I>(std::forward<CT>(c_tuple)));
}
template<class CT, uint I = 0, typename std::enable_if<I + 1 < std::tuple_size<typename impl::container_traits<CT>::value_type>::value, void>::type*& = enabler>
auto UnzipImpl_(CT&& c_tuple)
{
	return std::tuple_cat(std::make_tuple(unzip<I>(std::forward<CT>(c_tuple))), UnzipImpl_<CT, I + 1>(std::forward<CT>(c_tuple)));
}

// [(a, b, ...)] -> ([a], [b], ...)
// タプルのコンテナから、コンテナのタプルを作る
template <class CT>
auto unzip(CT const& c_tuple)
{
	return UnzipImpl_(c_tuple);
}

template <class CT, typename std::enable_if<!std::is_lvalue_reference<CT>::value>::type*& = enabler>
auto unzip(CT&& c_tuple)
{
	return UnzipImpl_(std::forward<CT>(c_tuple));
}

// uint -> a -> [a]
// 値を複製したコンテナを返す
template <class T, class C = std::vector<T>>
C replicate(uint n, T const& value)
{
	C result;
	for (uint i = 0; i<n; ++i) impl::container_traits<C>::add_element(result, value);
	return result;
}

// 等差数列
// st:初項, d:公差, n:項数
template <class T1, class T2, class C = std::vector<typename std::common_type<T1, T2>::type>>
C seq(T1 st, T2 d, uint n)
{
	using R = typename std::common_type<T1, T2>::type;
	C result;
	for (uint i = 0; i<n; ++i) impl::container_traits<C>::add_element(result, static_cast<R>(st) + i * static_cast<R>(d));
	return result;
}

// [a] -> [a]
// コンテナの要素を逆転させたコンテナを返す
template <class C>
C reverse(C const& container)
{
	C result = container;
	std::reverse(std::begin(result), std::end(result));
	return result;
}

// [a] -> [a] -> [a]
// コンテナの結合
template <class C>
C merge(C const& container1, C const& container2)
{
	auto result = container1;
	impl::container_traits<C>::concat(result, container2);
	return result;
}

// [a] -> [a] -> [a]
// コンテナの結合 (tamplateパラメータCで結合後のコンテナの種類を指定)
template <class C, class C1, class C2>
C merge(C1 const& container1, C2 const& container2)
{
	C result;
	for (auto v : container1) impl::container_traits<C>::add_element(result, v);
	for (auto v : container2) impl::container_traits<C>::add_element(result, v);
	return result;
}

// uint -> [a] -> [a]
// コンテナの先頭からn個を取り出したコンテナを返す (順序の無いコンテナでは実装依存)
template <class C>
C take(uint n, C const& container)
{
	C result;
	uint i = 0;
	for (auto it = std::begin(container); i < n; ++i, ++it) impl::container_traits<C>::add_element(result, *it);
	return result;
}

// uint -> [a] -> [a]
// コンテナの先頭からn個を削除したコンテナを返す (順序の無いコンテナでは実装依存)
template <class C>
C drop(uint n, C const& container)
{
	C result;
	uint i = 0;
	auto it = std::begin(container), end = std::end(container);
	for (; i < n && it != end; ++i, ++it) ;
	for (; it != end; ++i, ++it) impl::container_traits<C>::add_element(result, *it);
	return result;
}

#if SIG_GCC_GT4_8_0 || SIG_CLANG_GT_3_4 || !(_MSC_VER < 1900)
//(a -> a -> bool) -> [a] -> [a]
//比較関数を指定してソート
template <class F, class C, typename std::enable_if<has_random_access_op<C>::value, void>::type*& = enabler>
auto sort(F const& binary_op, C const& data){
	C result = data;
	std::sort(std::begin(result), std::end(result), binary_op);
	return result;
}
template <class F, class C, typename std::enable_if<!has_random_access_op<C>::value, void>::type*& = enabler>
auto sort(F const& binary_op, C const& data){
	C result = data;
	result.sort(binary_op);
	return result;
}

#endif
}

#endif
