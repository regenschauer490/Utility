/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_RANDOM_HPP
#define SIG_UTIL_RANDOM_HPP

#include "../sigutil.hpp"
#include <random>


/* 乱数を手軽に扱うためのユーティリティ */

namespace sig
{
// 初期化時に指定した範囲の一様分布乱数を発生させるクラス
// デフォルトの乱数生成器: メルセンヌツイスター
template <class NumType, class Engine = std::mt19937>
class SimpleRandom 
{
	Engine engine_;		// 乱数生成アルゴリズム 
	
	typename std::conditional <
		std::is_integral<NumType>::value,
		std::uniform_int_distribution<int>,
		std::uniform_real_distribution<double>
	> ::type dist_;		// 一様分布

public:
	// min: 乱数最小値
	// max: 乱数最大値
	// debug: 乱数のシードを固定するか
	SimpleRandom(NumType min, NumType max, bool debug) : engine_(
		[debug](){
		std::random_device rnd;
		std::vector<unsigned long> v(10);
		if (debug) std::fill(v.begin(), v.end(), 0);
		else std::generate(v.begin(), v.end(), std::ref(rnd));
		std::seed_seq seq(v.begin(), v.end());

		return Engine{seq};
	}()
		),
		dist_(min, max){}

	NumType operator()()
	{
		return dist_(engine_);
	}
};


// 重複の無い一様分布の整数乱数を生成
// n:	発生させる値の個数
// min:	最小値
// max:	最大値
// debug:	乱数のシードを固定するか
template < template < class T, class = std::allocator<T>> class Container = std::vector >
Container<int> make_unique_numbers(uint n, int min, int max, bool debug)
{
	std::unordered_set<int> match;
	Container<int> result;
	SimpleRandom<int> Rand(0, max - min, debug);

	int r;
	for (uint i = 0; i < n; ++i){
		do{
			r = min + Rand();
		} while (match.find(r) != match.end());

		match.insert(r);
		result.push_back(r);
	}

	return result;
}

}

#endif