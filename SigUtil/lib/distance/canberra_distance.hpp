/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIGUTIL_CANBERRA_DISTANCE_HPP
#define SIGUTIL_CANBERRA_DISTANCE_HPP

#include "../functional/high_order.hpp"
#include "comparable_check.hpp"

namespace sig
{

///キャンベラ距離
struct CanberraDistance
{
	template <class C1, class C2>
	double operator()(C1 const& vec1, C2 const& vec2) const
	{
		using T = typename std::common_type<typename impl::container_traits<C1>::value_type, typename impl::container_traits<C2>::value_type>::type;
		
		assert(is_comparable(vec1, vec2, impl::NumericVectorTag()));
		auto tmp = sig::zipWith([](T val1, T val2){ return static_cast<T>(std::abs(val1 - val2)) / (std::abs(val1) + std::abs(val2)); }, vec1, vec2);

		return std::accumulate(std::begin(tmp), std::end(tmp), T(), std::plus<T>());
	}
};

/// キャンベラ距離を求める関数（関数オブジェクト）
/**
	\param vec1 データ点1の座標ベクトル（\ref sig_container ）
	\param vec2 データ点2の座標ベクトル（\ref sig_container ）

	\return データ点間の距離

	\code
	const array<uint, 5> dot1{  -1, 0, 1, 2, 3 };		// sig::array
	const std::list<bool> dot2{ 1, 1.5, 2, 2.5, 3 };

	double d = canberra_distance(dot1, dot2);
	d;		// 22 / 9.0
	\endcode
*/
const CanberraDistance canberra_distance;

}
#endif