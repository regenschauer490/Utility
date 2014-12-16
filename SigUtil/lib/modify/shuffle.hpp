/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_SHUFFLE_HPP
#define SIG_UTIL_SHUFFLE_HPP

#include "../sigutil.hpp"
#include "../tools/random.hpp"

/// \file shuffle.hpp コンテナの要素をシャッフル

namespace sig
{
/// コンテナの要素をシャッフル
/**
	\param container シャッフル対処のコンテナ（\ref sig_container ）
	
	\code
	std::vector<int> data{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };

	shuffle(data);
	\endcode
*/
template <class C>
void shuffle(C& container)
{
	static SimpleRandom<double> myrand(0.0, 1.0, false);

	std::random_shuffle(std::begin(container), std::end(container), [&](std::ptrdiff_t max){ return static_cast<std::ptrdiff_t>(myrand() * max); });
}


namespace impl
{

template <class C>
void shuffle_impl(uint loop, C const& seq){}

template <class C, class It, class... Its>
void shuffle_impl(uint loop, C const& seq, It iter, Its... iters)
{
	auto ori_iter = iter;
	std::unordered_map<uint, typename It::value_type> map;

	for (uint i=0; i<loop; ++i, ++iter){
		map[seq[i]] = *iter;
	}
	for (uint i = 0; i < loop; ++i, ++ori_iter){
		*ori_iter = map[i];
	}

	shuffle_impl(loop, seq, iters...);
}
}	//impl

/// 複数のコンテナの同じインデックスの要素を対応させながらシャッフル
/**
	先頭から、コンテナの中で最小のインデックスまでの範囲をシャッフルする．\n
	ex: c1[1, 2, 3, 4, 5], c2[1, 2, 3] -> c1'[3, 1, 2, 4, 5], c2'[3, 1, 2]

	\param containers 任意個のコンテナ（\ref sig_container ）

	\code
	array<double, 5> data1{ 1.1, 2.2, 3.3, 4.4, 5.5 };
	std::vector<int> data2{ 1, 5, 3, -2 };
	std::list<std::string> data3{ "a", "b", "c" }

	shuffle(data1, data2, data3);		// 各コンテナの[0]~[2]までが対応しながらシャッフルされる
	\endcode
*/
template <class... Cs>
void shuffle(Cs&... containers)
{
	uint size = min(containers.size()...);
	auto rseq = make_unique_numbers(size, 0, size - 1, false);
		
	impl::shuffle_impl(size, rseq, std::begin(containers)...);
}

}
#endif
