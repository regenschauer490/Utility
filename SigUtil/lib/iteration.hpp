/*
Copyright(c) 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_FOREACH_HPP
#define SIG_UTIL_FOREACH_HPP

#include "helper.hpp"
#include "container_helper.hpp"

/* ‚©‚ä‚¢‚Æ‚±‚ë‚Éè‚ª“Í‚­”½•œˆ—ŠÖ” */

namespace sig
{
#undef max
#undef min

	template <class F, class... Cs>
	auto for_each(F const& func, Cs&... containers)
	{
		const uint length = Min(containers.size()...);
		Iterate2(length, func, std::begin(containers)...);
	}
	
	template <class F, class... Cs>
	auto for_each(F const& func, int init, Cs&... containers)
	{
		const uint length = Min(containers.size()...);
		Iterate2(length, init, func, std::begin(containers)...);
	}

}

#endif