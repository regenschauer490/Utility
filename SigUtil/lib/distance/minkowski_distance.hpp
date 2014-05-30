/*
Copyright(c) 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_MINKOWSKI_DISTANCE_HPP
#define SIG_UTIL_MINKOWSKI_DISTANCE_HPP

#include "../functional.hpp"

namespace sig
{
//ミンコフスキー距離
template <size_t P>
struct MinkowskiDistance
{
	template <class C1, class C2>
	double operator()(C1 const& vec1, C2 const& vec2) const
	{
		using T = typename std::common_type<typename sig::container_traits<C1>::value_type, typename sig::container_traits<C2>::value_type>::type;
		
		return std::pow(
			std::inner_product(std::begin(vec1), std::end(vec1), std::begin(vec2), T(), std::plus<T>(), [&](T v1, T v2){ return pow(sig::abs_delta(v1, v2), P); }),
			1.0 / P
		);
	}
};


//マンハッタン距離
using ManhattanDistance = MinkowskiDistance<1>;

const ManhattanDistance manhattan_distance;

//ユークリッド距離
using EuclideanDistance = MinkowskiDistance<2>;

const EuclideanDistance euclidean_distance;

}
#endif