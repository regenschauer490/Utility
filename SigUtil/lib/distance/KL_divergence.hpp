/*
Copyright(c) 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_KL_DIVERGENCE_HPP
#define SIG_KL_DIVERGENCE_HPP

#include "comparable_check.hpp"

namespace sig
{
struct KL_Divergence
{
	//KL情報量
	//条件：distribution[i] は正の値 かつ 総和が 1
	//値域：[0, ∞)
	template<class C1, class C2>
	double operator()(C1 const& dist1, C2 const& dist2) const
	{
		using T1 = typename container_traits<C1>::value_type;
		using T2 = typename container_traits<C2>::value_type;

		assert(is_comparable(dist1, dist2, DistributionTag()));
		assert(!has_zero(dist2));

		return std::inner_product(std::begin(dist1), std::end(dist1), std::begin(dist2), 0.0, std::plus<double>(),
			[](T1 d1, T2 d2){ return d1 ? d1 * (std::log2(d1) - std::log2(d2)) : 0 ; }
		);
	}
/*
	//KL情報量
	//条件：distribution[i] は正の値 かつ 総和が 1
	//値域：[0, ∞)
	//失敗時：boost::none (if not use boost, return -1)
	template<class C1, class C2>
	auto operator()(C1 const& dist1, C2 const& dist2) const ->typename Just<double>::type
	{
		using T1 = typename container_traits<C1>::value_type;
		using T2 = typename container_traits<C2>::value_type;

		if(!is_comparable(dist1, dist2, DistributionTag()) || has_zero(dist2)) return Nothing(-1);

		return typename Just<double>::type(
			std::inner_product(std::begin(dist1), std::end(dist1), std::begin(dist2), 0.0, std::plus<double>(),
				[](T1 d1, T2 d2){ return d1 ? d1 * (std::log2(d1) - std::log2(d2)) : 0 ; }
			)
		);
	}
*/
};

const KL_Divergence kl_divergence;

}
#endif