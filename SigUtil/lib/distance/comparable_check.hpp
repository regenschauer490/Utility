/*
Copyright(c) 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_COMPARE_CHECK_HPP
#define SIG_UTIL_COMPARE_CHECK_HPP

#include "../sigutil.hpp"

namespace sig
{
struct NumericVectorTag{};
struct DistributionTag{};

// 2�̃R���e�i�̃T�C�Y���������m�F
template <class C1, class C2>
bool is_same_size(C1 const& v1, C2 const& v2)
{
	return v1.size() == v2.size();	//todo: define like sig::size()
}

// ���U�m�����z�̐����𖞂����Ă��邩
template <class C1, class C2>
bool is_valid_distribution(C1 const& v1, C2 const& v2)
{
	return sig::equal(std::accumulate(std::begin(v1), std::end(v1), 0.0), 1)
		&& sig::equal(std::accumulate(std::begin(v2), std::end(v2), 0.0), 1);
}

template <class C>
bool has_zero(C const& v)
{
	return !std::accumulate(std::begin(v), std::end(v), true, [](bool s, bool e){ return s && e != 0; });
}

// 2�̃x�N�g������r�\���m�F
template <class C1, class C2>
bool is_comparable(C1 const& v1, C2 const& v2, NumericVectorTag)
{
	return is_same_size(v1, v2);
}

// 2�̊m�����z����r�\���m�F
template <class C1, class C2>
bool is_comparable(C1 const& v1, C2 const& v2, DistributionTag)
{
	return is_same_size(v1, v2) && is_valid_distribution(v1, v2);
}

}
#endif