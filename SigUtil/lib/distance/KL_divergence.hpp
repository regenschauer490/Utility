/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_KL_DIVERGENCE_HPP
#define SIG_KL_DIVERGENCE_HPP

#include "comparable_check.hpp"
#include "../helper/maybe.hpp"

namespace sig
{
/// KL情報量（Kullback–Leibler Divergence）
/**
	値はラップされて返される

	\sa kl_divergence
*/
struct KL_Divergence
{
	
	template<class C1, class C2>
	auto operator()(C1 const& dist1, C2 const& dist2) const ->Maybe<double>
	{
		using T1 = typename impl::container_traits<C1>::value_type;
		using T2 = typename impl::container_traits<C2>::value_type;

		if(!is_comparable(dist1, dist2, impl::DistributionTag()) || has_zero(dist2)) return Nothing(-1.0);

		return Just<double>(
			std::inner_product(std::begin(dist1), std::end(dist1), std::begin(dist2), 0.0, std::plus<double>(),
				[](T1 d1, T2 d2){ return d1 ? d1 * (std::log2(d1) - std::log2(d2)) : 0 ; }
			)
		);
	}
};

/// KL情報量を求める関数（関数オブジェクト）
/**
	\param dist1 確率分布1（\ref sig_container ）
	\param dist2 確率分布2（\ref sig_container ）

	\pre dist1の各要素は0以上の値 かつ dist2の各要素は正の値 かつ 総和が 1

	\return 確率分布間の非類似度（値は\ref sig_maybe で返される）

	\post 値域：[0, ∞)

	\code
	const array<double, 5> dist1{ 0.2, 0.1, 0, 0.4, 0.3 };		// sig::array
	const std::list<double> dist2{ 0.1, 0.3, 0.1, 0.4, 0.1 };

	auto m_kl12 = kl_divergence(dist1, dist2));		// kl12 :: Maybe<double>
	// auto m_kl21 = kl_divergence(dist2, dist1));	// error because dist1 has element whose value is 0

	double kl12 = fromJust(m_kl12);		// 0.51699...
	\endcode
*/
const KL_Divergence kl_divergence;

}
#endif