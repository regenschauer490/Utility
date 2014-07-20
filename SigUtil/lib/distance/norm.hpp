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
// Pノルム
template <size_t P>
struct Norm{
	template <class C>
	double operator()(C const& vec) const
	{
		using T = typename container_traits<C>::value_type;

		return std::pow(
			std::accumulate(std::begin(vec), std::end(vec), static_cast<T>(0), [&](T sum, T val){ return sum + std::pow(std::abs(val), P); }),
			1.0 / P
		);
	}
	
	template <class C1, class C2>
	double operator()(C1 const& vec1, C2 const& vec2) const
	{
		assert(is_comparable(vec1, vec2, NumericVectorTag()));
		return this->operator()(minus(vec1, vec2));
	}
};

const Norm<1> norm_L1;
const Norm<2> norm_L2;


// 最大ノルム
struct MaxNorm{
	template <class C>
	double operator()(C const& vec) const
	{
		using T = typename container_traits<C>::value_type;

		T max = *std::begin(vec);
		for(auto e : vec){
			if(std::abs(e) > max) max = std::abs(e);
		}
		return max;
	}

	template <class C1, class C2>
	double operator()(C1 const& vec1, C2 const& vec2) const
	{
		assert(is_comparable(vec1, vec2, NumericVectorTag()));
		return this->operator()(minus(vec1, vec2));
	}
};

const MaxNorm norm_max;
}
#endif