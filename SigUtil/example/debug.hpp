/*
Copyright(c) 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_DEBUG_HPP
#define SIG_UTIL_DEBUG_HPP

#include "../lib/functional.hpp"

/* デバッグ用ツール */

namespace sig
{

struct DebugEqual{
	template <class T1, class T2>
	bool operator()(T1 v1, T2 v2) const{ assert(v1 == v2); return v1 == v2; }
};

// 
template <class C, class T>
void DebugElements(C const& container1, std::initializer_list<T> container2)
{
	using CT = container_traits<C>::value_type;

	sig::zipWith([](CT v1, T v2){ assert(v1 == v2); return 0; }, container1, container2);
}

};

#endif