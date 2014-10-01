/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_KL_DIVERGENCE_HPP
#define SIG_KL_DIVERGENCE_HPP

#include "comparable_check.hpp"

namespace sig
{
/// KL情報量（Kullback–Leibler Divergence）
/**
	boost.optional有効時には値がラップされて返される
*/
struct KL_Divergence
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
		using T1 = typename container_traits<C1>::value_type;
		using T2 = typename container_traits<C2>::value_type;

		if(!is_comparable(dist1, dist2, impl::DistributionTag()) || has_zero(dist2)) return Nothing(-1);

		return Just<double>(
			std::inner_product(std::begin(dist1), std::end(dist1), std::begin(dist2), 0.0, std::plus<double>(),
				[](T1 d1, T2 d2){ return d1 ? d1 * (std::log2(d1) - std::log2(d2)) : 0 ; }
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
		using T1 = typename container_traits<C1>::value_type;
		using T2 = typename container_traits<C2>::value_type;

		assert(is_comparable(dist1, dist2, impl::DistributionTag()));
		assert(!has_zero(dist2));

		return std::inner_product(std::begin(dist1), std::end(dist1), std::begin(dist2), 0.0, std::plus<double>(),
			[](T1 d1, T2 d2){ return d1 ? d1 * (std::log2(d1) - std::log2(d2)) : 0; }
		);
	}
#endif
};

/// KL情報量を求める関数（関数オブジェクト）
/**
	\param dist1 確率分布1
	\param dist2 確率分布2

	\return 確率分布間の非類似度（boost.optional有効時には値がラップされている）
	\post 値域：[0, ∞)
*/
const KL_Divergence kl_divergence;

}
#endif