/*
Copyright(c) 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_JS_DIVERGENCE_HPP
#define SIG_JS_DIVERGENCE_HPP

#include "../sigutil.hpp"

namespace sig
{
struct JS_Divergence
{
	//JS情報量
	//条件：distribution[i] は正の値 かつ 総和が 1
	//値域：[0, ∞)
	//失敗時：boost::none (if not use boost, return 0)
	template<class C1, class C2>
	auto operator()(C1 const& dist1, C2 const& dist2) const ->typename sig::Just<double>::type
	{
		if(dist1.size() != dist2.size()) return sig::Nothing(0);

		const auto Log2 = [](double n)->double{ return log(n) / log(2); };

		const auto JS = [&](C1 const& d1, C2 const& d2)->double{
			double sum1 = 0, sum2 = 0;
			for(sig::uint i=0; i<d1.size(); ++i){
				auto r = (d1[i] + d2[i]) * 0.5;
				sum1 += d1[i] * ( Log2(d1[i]) - Log2(r) );
				sum2 += d2[i] * ( Log2(d2[i]) - Log2(r) );
			}
			return (sum1 + sum2) * 0.5;
		};

		return typename sig::Just<double>::type(JS(dist1, dist2));
	}
};

const JS_Divergence js_divergence;

}
#endif