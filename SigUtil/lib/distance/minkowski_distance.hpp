/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_MINKOWSKI_DISTANCE_HPP
#define SIG_UTIL_MINKOWSKI_DISTANCE_HPP

#include "../functional.hpp"
#include "comparable_check.hpp"

namespace sig
{
/// ミンコフスキー距離
/**
	\tparam P P=1はマンハッタン距離、P=2はユークリッド距離に等しい
*/
template <size_t P>
struct MinkowskiDistance
{
	/**
	\param vec1 データ点1の座標ベクトル
	\param vec2 データ点2の座標ベクトル

	\return データ点間の距離
	*/
	template <class C1, class C2>
	double operator()(C1 const& vec1, C2 const& vec2) const
	{
		using T = typename std::common_type<typename impl::container_traits<C1>::value_type, typename impl::container_traits<C2>::value_type>::type;
		
		assert(is_comparable(vec1, vec2, impl::NumericVectorTag()));

		return std::pow(
			std::inner_product(std::begin(vec1), std::end(vec1), std::begin(vec2), 0.0, std::plus<double>(), [&](T v1, T v2){ return std::pow(std::abs(v1 - v2), P); }),
			1.0 / P
		);
	}
};

using ManhattanDistance = MinkowskiDistance<1>;

/// マンハッタン距離を求める関数（関数オブジェクト）
/**
	\param vec1 データ点1の座標ベクトル
	\param vec2 データ点2の座標ベクトル

	\return データ点間の距離
*/
const ManhattanDistance manhattan_distance;


using EuclideanDistance = MinkowskiDistance<2>;
/// ユークリッド距離を求める関数（関数オブジェクト）
/**
	\param vec1 データ点1の座標ベクトル
	\param vec2 データ点2の座標ベクトル

	\return データ点間の距離
*/
const EuclideanDistance euclidean_distance;

}
#endif