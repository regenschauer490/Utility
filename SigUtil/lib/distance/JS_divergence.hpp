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
	値はラップされて返される

	\sa js_divergence
*/
struct JS_Divergence
{
	template<class C1, class C2>
	auto operator()(C1 const& dist1, C2 const& dist2) const ->Maybe<double>
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
};

/// JS情報量を求める関数（関数オブジェクト）
/**
		\param dist1 確率分布1（\ref sig_container ）
		\param dist2 確率分布2（\ref sig_container ）

		\pre dist1, dist2 の各要素は0以上の値 かつ 総和が 1

		\return 確率分布間の非類似度（値は\ref sig_maybe で返される）

		\post 値域：[0, ∞)

		\code
		const array<double, 5> dist1{ 0.2, 0.1, 0, 0.4, 0.3 };		// sig::array
		const std::list<double> dist2{ 0.1, 0.3, 0.1, 0.4, 0.1 };

		auto m_js12 = js_divergence(dist1, dist2));	// js12 :: Maybe<double>
		auto m_js21 = js_divergence(dist2, dist1));	// js21 :: Maybe<double>

		double js12 = fromJust(m_js12);		// 0.137744...
		double js21 = *m_js21;				// 0.137744...
		\endcode
*/
const JS_Divergence js_divergence;

}
#endif