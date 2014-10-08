/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_JS_DIVERGENCE_HPP
#define SIG_JS_DIVERGENCE_HPP

#include "comparable_check.hpp"

namespace sig
{
/// JS情報量（Jensen-Shannon Divergence）
/**
	boost.optional有効時には値がラップされて返される
*/
struct JS_Divergence
{
#if SIG_ENABLE_BOOST && SIG_USE_OPTIONAL
	/**
	boost.optional有効時

	\param dist1 確率分布1
	\param dist2 確率分布2

	\pre dist1, dist2 の各要素は正の値 かつ 総和が 1

	\return 確率分布間の非類似度（ラップされている）．失敗時にはboost::none
	\post 値域：[0, ∞)
	*/
	template<class C1, class C2>
	auto operator()(C1 const& dist1, C2 const& dist2) const ->Just<double>
	{
		using T1 = typename impl::container_traits<C1>::value_type;
		using T2 = typename impl::container_traits<C2>::value_type;

		if (!is_comparable(dist1, dist2, impl::DistributionTag())) return Nothing(-1);

		return Just<double>(
			0.5 * std::inner_product(std::begin(dist1), std::end(dist1), std::begin(dist2), 0.0, std::plus<double>(),
			[](T1 d1, T2 d2){
				auto r = std::log2((d1 + d2) * 0.5);
				return (d1 ? d1 * (std::log2(d1) - r) : 0) + (d2 ? d2 * (std::log2(d2) - r) : 0);
			}
			)
		);
	}
#else
	/**
	boost.optional無効時

	\param dist1 確率分布1
	\param dist2 確率分布2

	\pre dist1, dist2 の各要素は正の値 かつ 総和が 1

	\return 確率分布間の非類似度
	\post 値域：[0, ∞)
	*/
	template<class C1, class C2>
	double operator()(C1 const& dist1, C2 const& dist2) const
	{
		using T1 = typename impl::container_traits<C1>::value_type;
		using T2 = typename impl::container_traits<C2>::value_type;

		assert(is_comparable(dist1, dist2, impl::DistributionTag()));

		return 0.5 * std::inner_product(std::begin(dist1), std::end(dist1), std::begin(dist2), 0.0, std::plus<double>(),
			[](T1 d1, T2 d2){ 
				auto r = std::log2((d1 + d2) * 0.5);
				return (d1 ? d1 * (std::log2(d1) - r) : 0) + (d2 ? d2 * (std::log2(d2) - r) : 0);
			}
		);
	}
#endif
};

/// JS情報量を求める関数（関数オブジェクト）
/**
	\param dist1 確率分布1
	\param dist2 確率分布2

	\return 確率分布間の非類似度（boost.optional有効時には値がラップされている）
	\post 値域：[0, ∞)
*/
const JS_Divergence js_divergence;

}
#endif