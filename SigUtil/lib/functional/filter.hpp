/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_FILTER_HPP
#define SIG_UTIL_FILTER_HPP

#include "../helper/helper.hpp"
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
	const array<int, 3> data1{ 1, -3, 2 };	// sig::array
	const std::unordered_set<double> data2{ 0, 1.1, -2.2, 3.3 };

	auto fl1 = filter([](int v){ return v % 2; }, data1);
	auto fl2 = filter([](double v){ return v < 0; }, data2);

	fl1;	// array<int, 3>{ 1, -3 }. array[2]は未使用
	fl2;	// std::unordered_set<double>{ -2.2 }
	\endcode
*/
template <class F, class C>
auto filter(F const& pred, C const& list)
{
	C result;
	for (auto const& e : list){
		if (pred(e)) impl::container_traits<C>::add_element(result, e);
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
template <class F, class C>
auto partition(F const& pred, C const& list)
{
	C result1, result2;

	for (auto const& e : list){
		if (pred(e)) impl::container_traits<C>::add_element(result1, e);
		else impl::container_traits<C>::add_element(result2, e);
	}
	return std::make_tuple(std::move(result1), std::move(result2));
}

}
#endif
