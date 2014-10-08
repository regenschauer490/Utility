/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_COSINE_SIMILARITY_HPP
#define SIG_UTIL_COSINE_SIMILARITY_HPP

#include <numeric>
#include "norm.hpp"
#include "comparable_check.hpp"

namespace sig
{

/// コサイン類似度（Cosine Similarity）
struct CosineSimilarity
{
	/**
	\param vec1 データ点1の座標ベクトル
	\param vec2 データ点2の座標ベクトル

	\return データ点間のコサイン距離
	\post 値域：[-1, 1]
	*/
	template<class C1, class C2>
	double operator()(C1 const& vec1, C2 const& vec2) const
	{
		using T = typename std::common_type<typename impl::container_traits<C1>::value_type, typename impl::container_traits<C2>::value_type>::type;

		assert(is_comparable(vec1, vec2, impl::NumericVectorTag()));

		return std::inner_product(std::begin(vec1), std::end(vec1), std::begin(vec2), static_cast<T>(0)) / (norm_L2(vec1) * norm_L2(vec2));
	}
};

/// コサイン類似度を求める関数（関数オブジェクト）
/**
	\param vec1 データ点1の座標ベクトル
	\param vec2 データ点2の座標ベクトル

	\return データ点間のコサイン距離
	\post 値域：[-1, 1]
*/
const CosineSimilarity cosine_similarity;

}
#endif