/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_FOLD_HPP
#define SIG_UTIL_FOLD_HPP

#include "../helper/helper.hpp"
#include "../helper/container_helper.hpp"

/// \file fold.hpp たたみ込み関数

namespace sig
{
/// コンテナの先頭からたたみ込み
/**
	(a -> b -> a) -> a0 -> [b] -> a \n
	a0はaに暗黙的に型変換可能

	\param func 関数 (a -> b -> a)
	\param init 初期値 a0
	\param list データが格納されたコンテナ [b]（\ref sig_container ）

	\return 処理結果 a

	\sa foldr(F const& func, T init, C const& list)

	\code
	const std::set<double> data1{ -2.2, 0 , 1.1, 3.3 };
	const array<int, 4> data2{ 1, -3, 5, 4 };	// sig::array

	double r1 = foldl(std::plus<double>(), 0, data1);
	double r2 = foldl([](double sum, int v){ return sum + v % 2; }, 0, data2);

	r1;		// 2.2
	r2;		// 1
	\endcode
*/
template <class F, class T, class C>
auto foldl(F const& func, T init, C const& list)
{
	using R = decltype(func(init, std::declval<typename impl::container_traits<C>::value_type>()));
	return std::accumulate(std::begin(list), std::end(list), static_cast<R>(init), func);
}

#if SIG_GCC_GT_4_9 || SIG_CLANG_GT_3_5 || SIG_MSVC_ENV

/// コンテナの末尾からたたみ込み
/**
	(a -> b -> b) -> b0 -> [a] -> b \n
	b0はbに暗黙的に型変換可能

	\param func 関数 (a -> b -> a)
	\param init 初期値 b0
	\param list データが格納されたコンテナ [a]（\ref sig_container ）

	\return 処理結果 b

	\sa foldl(F const& func, T init, C const& list)

	\code
	int fl = foldl(std::minus<int>(), 1, array<int, 3>{ 2, 3, 4 });	// ((1-2)-3)-4
	int fr = foldr(std::minus<int>(), 4, array<int, 3>{ 1, 2, 3 });	 // 1-(2-(3-4))

	assert(fl0 == ((1-2)-3)-4);
	assert(fr0 == 1-(2-(3-4)));
	\endcode
*/
template <class F, class T, class C>
auto foldr(F const& func, T init, C const& list)
{
	using R = decltype(impl::eval(func, init, std::declval<typename impl::container_traits<C>::value_type>()));
	return std::accumulate(std::rbegin(list), std::rend(list), static_cast<R>(init), std::bind(func, _2, _1));
}
#endif


/// 可変長個のコンテナに対して関数適用して結果を集約する関数
/**
	(a -> a -> a) -> (b -> c -> ... -> a) -> a0 -> [b] -> [c] -> ... -> a \n
	a0はaに暗黙的に型変換可能 \n
	listsのいずれかの末尾に到達するまで集約の計算が行われる

	\param fold_func oper_funcで計算した結果を集約する関数 (a -> a -> a)
	\param oper_func 各コンテナの要素を引数に取る関数 (b -> c -> ... -> a)
	\param init 初期値 a0
	\param lists... データが格納されたコンテナ [b] [c] ...（\ref sig_container ）

	\return 処理結果 a

	\code
	const array<int, 5> data1{ 1, 2, 3, 4, 5 };		// sig::array
	const std::vector<int> data2{ 1, -3, 5, 2, 10 };
	const std::list<int> data3{ 1, -3, 5, 2 };

	double dp = dotProduct(
		[](double sum, double a){ return sum + a; },
		[](int b, int c, int d, double e){ return std::pow(b*b + c*c + d*d + e*e, 0.25); },
		0,
		data1, data2, data3, array<double, 4>{ 1.1, 2.2, 3.3, 4.4 }
	);
	\endcode
*/
template <class F1, class F2, class T, class... Cs>
auto dotProduct(F1 const& fold_func, F2 const& oper_func, T init, Cs const&... lists)
{
	using R = decltype(impl::eval(
		fold_func,
		init,
		impl::eval(oper_func, std::declval<typename impl::container_traits<Cs>::value_type>()...)
		));
	R result = init;
	const uint length = min(lists.size()...);

	iterative_fold(length, result, oper_func, fold_func, std::begin(lists)...);

	return result;
}

}
#endif