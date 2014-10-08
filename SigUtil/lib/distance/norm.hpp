/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_NORM_HPP
#define SIG_UTIL_NORM_HPP

#include "../calculation.hpp"
#include "comparable_check.hpp"
#include <numeric>
#undef max
#undef min

namespace sig
{
/// Pノルム
/**
	\tparam P Pノルムを指定
*/
template <size_t P>
struct Norm
{
	/**
	\param vec データ点の座標ベクトル

	\return データ点のノルム
	*/
	template <class C>
	double operator()(C const& vec) const
	{
		using T = typename impl::container_traits<C>::value_type;

		return std::pow(
			std::accumulate(std::begin(vec), std::end(vec), static_cast<T>(0), [&](T sum, T val){ return sum + std::pow(std::abs(val), P); }),
			1.0 / P
		);
	}
	
	/**
	\param vec1 データ点1の座標ベクトル
	\param vec2 データ点2の座標ベクトル

	\return データ点間のノルム
	*/
	template <class C1, class C2>
	double operator()(C1 const& vec1, C2 const& vec2) const
	{
		assert(is_comparable(vec1, vec2, impl::NumericVectorTag()));
		return this->operator()(minus(vec1, vec2));
	}
};

/// L1ノルムを求める関数（関数オブジェクト）
/**
	\param vec1 データ点1の座標ベクトル
	\param vec2 [option] データ点2の座標ベクトル

	\return データ点のノルム（vec2を指定時はデータ点間のノルム）
*/
const Norm<1> norm_L1;

/// L2ノルムを求める関数（関数オブジェクト）
/**
	\param vec1 データ点1の座標ベクトル
	\param vec2 [option] データ点2の座標ベクトル

	\return データ点のノルム（vec2を指定時はデータ点間のノルム）
*/
const Norm<2> norm_L2;


/// 最大ノルム
struct MaxNorm
{
	/**
	\param vec データ点の座標ベクトル

	\return データ点のノルム
	*/
	template <class C>
	double operator()(C const& vec) const
	{
		using T = typename impl::container_traits<C>::value_type;

		T max = *std::begin(vec);
		for(auto e : vec){
			if(std::abs(e) > max) max = std::abs(e);
		}
		return max;
	}

	/**
	\param vec1 データ点1の座標ベクトル
	\param vec2 データ点2の座標ベクトル

	\return データ点間のノルム
	*/
	template <class C1, class C2>
	double operator()(C1 const& vec1, C2 const& vec2) const
	{
		assert(is_comparable(vec1, vec2, impl::NumericVectorTag()));
		return this->operator()(minus(vec1, vec2));
	}
};

/// 最大ノルムを求める関数（関数オブジェクト）
/**
	\param vec1 データ点1の座標ベクトル
	\param vec2 [option] データ点2の座標ベクトル

	\return データ点のノルム（vec2を指定時はデータ点間のノルム）
*/
const MaxNorm norm_max;
}
#endif