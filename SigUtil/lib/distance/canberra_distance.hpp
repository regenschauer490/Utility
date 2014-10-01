/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_CANBERRA_DISTANCE_HPP
#define SIG_UTIL_CANBERRA_DISTANCE_HPP

#include "../functional.hpp"
#include "comparable_check.hpp"

namespace sig
{

///キャンベラ距離
/**
	boost.optional有効時には値がラップされて返される
*/
struct CanberraDistance
{
#if SIG_ENABLE_BOOST && SIG_USE_OPTIONAL
	/**
	boost.optional有効時

	\param vec1 データ点1の座標ベクトル
	\param vec2 データ点2の座標ベクトル

	\return データ点間の距離（ラップされている）．失敗時にはboost::none
	*/
	template <class C1, class C2>
	auto operator()(C1 const& vec1, C2 const& vec2) const->Just<double>
	{
		using T = typename std::common_type<typename container_traits<C1>::value_type, typename container_traits<C2>::value_type>::type;

		if(!is_comparable(vec1, vec2, impl::NumericVectorTag())) return boost::none;
		auto tmp = sig::zipWith([](T val1, T val2){ return static_cast<T>(std::abs(val1 - val2)) / (std::abs(val1) + std::abs(val2)); }, vec1, vec2);

		return Just<double>(std::accumulate(std::begin(tmp), std::end(tmp), T(), std::plus<T>()));
	}
#else
	/**
	boost.optional無効時

	\param vec1 データ点1の座標ベクトル
	\param vec2 データ点2の座標ベクトル

	\return データ点間の距離
	*/
	template <class C1, class C2>
	double operator()(C1 const& vec1, C2 const& vec2) const
	{
		using T = typename std::common_type<typename container_traits<C1>::value_type, typename container_traits<C2>::value_type>::type;
		
		assert(is_comparable(vec1, vec2, impl::NumericVectorTag()));
		auto tmp = sig::zipWith([](T val1, T val2){ return static_cast<T>(std::abs(val1 - val2)) / (std::abs(val1) + std::abs(val2)); }, vec1, vec2);

		return std::accumulate(std::begin(tmp), std::end(tmp), T(), std::plus<T>());
	}
#endif
};

/// キャンベラ距離を求める関数（関数オブジェクト）
/**
	\param vec1 データ点1の座標ベクトル
	\param vec2 データ点2の座標ベクトル

	\return データ点間の距離（boost.optional有効時には値がラップされている）
*/
const CanberraDistance canberra_distance;

}
#endif