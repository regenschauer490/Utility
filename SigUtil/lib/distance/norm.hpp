/*
Copyright(c) 2014 Akihiro Nishimura

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
// Pノルムの一般式
template <size_t P, class C>
double norm(C const& vec)
{
	using T = typename container_traits<C>::value_type;

	return std::pow(
		std::accumulate(std::begin(vec), std::end(vec), static_cast<T>(0), [&](T sum, T val){ return sum + std::pow(std::abs(val), P); }),
		1.0 / P
	);
}

template <class C>
double norm_L1(C const& vec)
{
	return norm<1>(vec);
}

template <class C1, class C2>
double norm_L1(C1 const& vec1, C2 const& vec2)
{
	assert(is_comparable(vec1, vec2, NumericVectorTag()));
	return norm<1>(minus(vec1, vec2));
}

template <class C>
double norm_L2(C const& vec)
{
	return norm<2>(vec);
}

template <class C1, class C2>
double norm_L2(C1 const& vec1, C2 const& vec2)
{
	assert(is_comparable(vec1, vec2, NumericVectorTag()));
	return norm<2>(minus(vec1, vec2));
}

template <class C>
double norm_max(C const& vec)
{
	using T = typename container_traits<C>::value_type;

	T max = *std::begin(vec);
	for(auto e : vec){
		if(std::abs(e) > max) max = std::abs(e);
	}
	return max;
}

template <class C1, class C2>
double norm_max(C1 const& vec1, C2 const& vec2)
{
	assert(is_comparable(vec1, vec2, NumericVectorTag()));
	return norm_max(minus(vec1, vec2));
}

}
#endif