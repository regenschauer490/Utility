/*
Copyright(c) 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_BINARY_DISTANCE_HPP
#define SIG_UTIL_BINARY_DISTANCE_HPP

#include "../functional.hpp"

namespace sig
{

//バイナリ距離
struct BinaryDistance
{
	template <class C1, class C2,
		typename = typename std::enable_if<std::is_integral<typename container_traits<C1>::value_type>::value>::type,
		typename = typename std::enable_if<std::is_integral<typename container_traits<C2>::value_type>::value>::type
	>
	double operator()(C1 const& vec1, C2 const& vec2) const
	{
		int ether = 0, both = 0;
		auto it1 = std::begin(vec1), end1 = std::end(vec1);
		auto it2 = std::begin(vec2), end2 = std::end(vec2);

		for (; it1 != end1 && it2 != end2; ++it1, ++it2){
			if (*it1 == 1 && *it2 == 1) ++both;
			else if (*it1 == 1 || *it2 == 1) ++ether;
		}
		return static_cast<double>(ether) / (ether + both);
	}
};

const BinaryDistance binary_distance;

}
#endif