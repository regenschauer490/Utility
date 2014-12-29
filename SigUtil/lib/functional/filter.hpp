/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIGUTIL_FILTER_HPP
#define SIGUTIL_FILTER_HPP

#include "../helper/helper_modules.hpp"
#include "../helper/container_helper.hpp"

/// \file filter.hpp 各種フィルタ関数

namespace sig
{
/// コンテナから指定条件を満たす要素を抽出する
/**
	(a -> Bool) -> [a] -> [a] 

	\param pred 条件判定を行う述語関数 (a -> Bool)
	\param list データが格納されたコンテナ [a]（\ref sig_container ）

	\return 処理結果のコンテナ [a]（コンテナはlistと同じ種類）

	\code
	const std::unordered_set<double> data1{ 0, 1.1, -2.2, 3.3 };
	array<std::string, 3> data2{ "a", "bbb", "cc" };	// sig::array

	auto fl1 = filter([](int v){ return v < 0; }, data1);
	auto fl2 = filter([](std::string const& v){ return v.length() < 3; }, std::move(data2));

	fl1;	// std::unordered_set<double>{ -2.2 }
	fl2;	// array<std::string, 3>{ "a", "cc" } // array[2]は未使用

	assert(data2[0].empty() && data2[2].empty() && (!data2[1].empty()) );
	\endcode
*/
template <class F, class C,
	class CR = typename impl::remove_const_reference<C>::type,
	class ET = typename impl::forward_element<C>::type
>
auto filter(F&& pred, C&& list)
{
	CR result = impl::container_traits<CR>::make(list.size());

	for (auto&& e : std::forward<C>(list)){
		if (std::forward<F>(pred)(e)) impl::container_traits<CR>::add_element(result, std::forward<ET>(e));
	}
	return result;
}

/// コンテナから指定条件を満たす要素を抽出する(反復回数を表すindexが利用できる)
/**
	(int -> a -> Bool) -> int -> [a] -> [a] 

	\param pred: 条件判定を行う述語関数 (int -> a -> Bool)
	\param init indexの初期値 int
	\param list データが格納されたコンテナ [a]（\ref sig_container ）

	\return 処理結果のコンテナ [a]（コンテナはlistと同じ種類）

	\code
	const array<double, 3> data{ 1.1, 1.5, 4 };

	auto filt3 = filter([](int i, double v){ return v / i < 1; }, 1, data);
	data;		// { 1.5 }
	\endcode
*/
template <class F, class C,
	class CR = typename impl::remove_const_reference<C>::type,
	class ET = typename impl::forward_element<C>::type
>
auto filter(F&& pred, int init, C&& list)
{
	CR result = impl::container_traits<CR>::make(list.size());

	for (auto&& e : std::forward<C>(list)){
		if (std::forward<F>(pred)(init, e)) impl::container_traits<CR>::add_element(result, std::forward<ET>(e));
		++init;
	}
	return result;
}


/// コンテナから指定条件を満たす要素とそれ以外の要素とを分離する
/**
	(a -> Bool) -> [a] -> ([a], [a]) 

	\param pred 条件判定を行う述語関数 (a -> Bool)
	\param list データが格納されたコンテナ [a]（\ref sig_container ）

	\return 処理結果のタプル ([a], [a])（コンテナ2つはlistと同じ種類）

	\code
	const array<int, 3> data1{ 1, -3, 2 };	// sig::array
	const std::unordered_set<double> data2{ 0, 1.1, -2.2, 3.3 };

	auto pt1 = partition([](int v){ return v % 2; }, data1);
	auto pt2 = partition([](double v){ return v < 0; }, data2);

	auto pt1_t = std::get<0>(pt1);	// array<int, 3>{ 1, -3 }
	auto pt1_f = std::get<1>(pt1);	// array<int, 3>{ 2 }

	auto pt2_t = std::get<0>(pt2);	// std::unordered_set<double>{ -2.2 }
	auto pt2_f = std::get<1>(pt2);	// std::unordered_set<double>{ 0, 1.1, 3.3 }
	\endcode
*/
template <class F, class C,
	class CR = typename impl::remove_const_reference<C>::type,
	class ET = typename impl::forward_element<C>::type
>
auto partition(F&& pred, C&& list)
{
	CR result1 = impl::container_traits<CR>::make(list.size());
	CR result2 = impl::container_traits<CR>::make(list.size());

	for (auto&& e : std::forward<C>(list)){
		if (std::forward<F>(pred)(e)) impl::container_traits<CR>::add_element(result1, std::forward<ET>(e));
		else impl::container_traits<CR>::add_element(result2, std::forward<ET>(e));
	}
	return std::make_tuple(std::move(result1), std::move(result2));
}

}
#endif
