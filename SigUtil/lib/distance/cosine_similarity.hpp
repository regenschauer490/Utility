/*
Copyright(c) 2014 Akihiro Nishimura

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
struct CosineSimilarity
{
	//コサイン類似度
	//値域：[-1, 1]
	template<class C1, class C2>
	double operator()(C1 const& vec1, C2 const& vec2) const
	{
		using T = typename std::common_type<typename container_traits<C1>::value_type, typename container_traits<C2>::value_type>::type;

		assert(is_comparable(vec1, vec2, NumericVectorTag()));

		return std::inner_product(std::begin(vec1), std::end(vec1), std::begin(vec2), static_cast<T>(0)) / (norm_L2(vec1) * norm_L2(vec2));
	}
/*
	//コサイン類似度
	//値域：[-1, 1]
	//失敗時：boost::none (if not use boost, return 0)
	template<class C1, class C2>
	auto operator()(C1 const& vec1, C2 const& vec2) const ->typename Just<double>::type
	{
		using T = std::common_type<typename container_traits<C1>::value_type, typename container_traits<C2>::value_type>::type;

		if(!is_comparable(vec1, vec2, NumericVectorTag())) return Nothing(0);

		return typename Just<double>::type(std::inner_product(std::begin(vec1), std::end(vec1), std::begin(vec2), static_cast<T>(0)) / (norm_L2(vec1) * norm_L2(vec2)));
	}
*/
};

const CosineSimilarity cosine_similarity;

}
#endif