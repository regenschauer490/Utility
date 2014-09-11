/*
Copyright(c) 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_DEBUG_HPP
#define SIG_UTIL_DEBUG_HPP

#include <assert.h>
#include "../lib/helper/helper.hpp"
#include "../lib/helper/container_helper.hpp"

/* デバッグ用ツール */

namespace sig
{
#undef min

	struct DebugEqual{
		template <class T1, class T2, typename std::enable_if<!(std::is_floating_point<T1>::value) && !(std::is_floating_point<T2>::value)>::type *& = enabler>
		void operator()(T1 v1, T2 v2) const{
			using T = typename std::common_type<T1, T2>::type;
			assert(static_cast<T>(v1) == static_cast<T>(v2));
		}

		template <class T1, class T2, typename std::enable_if<std::is_floating_point<T1>::value || std::is_floating_point<T2>::value>::type *& = enabler>
		void operator()(T1 v1, T2 v2) const{
			//std::cout << "v1:" << v1 << ", v2:" << v2 << std::endl;
			assert(equal(v1, v2));
		}
	};

	const DebugEqual debug_equal;

	template <class F, class C, class... Cs>
	auto assert_foreach(F const& func, C const& test, Cs const&... origs)
	{
		C calc;
		const uint length = min(origs.size()...);
		iterative_make(length, calc, func, std::begin(origs)...);
		assert(test.size() == calc.size()); 

		auto it1 = std::begin(test), end1 = std::end(test);
		auto it2 = std::begin(calc), end2 = std::end(calc);

		for (; it1!=end1 && it2!=end2; ++it1, ++it2){
			debug_equal(*it1, *it2);
		}
	}
};

#endif
