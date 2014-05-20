/*
Copyright(c) 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_DEBUG_HPP
#define SIG_UTIL_DEBUG_HPP

#include <assert.h>
#include "../lib/helper.hpp"
#include "../lib/functional.hpp"
#include "../lib/iteration.hpp"

/* デバッグ用ツール */

namespace sig
{
	//debug用
	template <class F, class... Cs>
	auto for_each(F const& func, Cs const&... containers)
	{
		const uint length = Min(containers.size()...);
		iterative_assign(length, func, std::begin(containers)...);
	}

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
};

#endif
