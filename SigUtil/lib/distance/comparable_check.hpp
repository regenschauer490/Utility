/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_COMPARE_CHECK_HPP
#define SIG_UTIL_COMPARE_CHECK_HPP

#include "../helper/helper_modules.hpp"

namespace sig
{
namespace impl
{
struct NumericVectorTag{};
struct DistributionTag{};
}

// 2つのコンテナのサイズが同じか確認
template <class C1, class C2>
bool is_same_size(C1 const& v1, C2 const& v2)
{
	return v1.size() == v2.size();	//todo: define like sig::size()
}

/// 離散確率分布の性質を満たしているか確認
template <class C1, class C2>
bool is_valid_distribution(C1 const& v1, C2 const& v2)
{
	return sig::equal(std::accumulate(std::begin(v1), std::end(v1), 0.0), 1)
		&& sig::equal(std::accumulate(std::begin(v2), std::end(v2), 0.0), 1);
}

/// 要素の値に0があるか確認
template <class C>
bool has_zero(C const& v)
{
	return !std::accumulate(std::begin(v), std::end(v), true, [](bool s, bool e){ return s && e != 0; });
}

/// 2つのベクトルが比較可能か確認
template <class C1, class C2>
bool is_comparable(C1 const& v1, C2 const& v2, impl::NumericVectorTag)
{
	return is_same_size(v1, v2);
}

/// 2つの確率分布が比較可能か確認
template <class C1, class C2>
bool is_comparable(C1 const& v1, C2 const& v2, impl::DistributionTag)
{
	return is_same_size(v1, v2) && is_valid_distribution(v1, v2);
}

}
#endif
