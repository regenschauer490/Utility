/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_RANDOM_HPP
#define SIG_UTIL_RANDOM_HPP

#include "../sigutil.hpp"
#include <random>


/// \file random.hpp乱数を手軽に扱うためのユーティリティ

namespace sig
{
/// 初期化時に指定した範囲の一様分布乱数を発生させるクラス
/**
	\tparam T 発生する乱数の型
	\tparam Engine 乱数生成器（default：メルセンヌツイスター）

	\code
	SimpleRandom<int> rand_gen(-10, 10, true);	// [-10, 10]の一様分布乱数、デバッグモード(シード固定)

	std::cout << rand_gen() << std::endl;
	\endcode
*/
template <class T, class Engine = std::mt19937>
class SimpleRandom 
{
	mutable Engine engine_;		// 乱数生成アルゴリズム 
	
	typename std::conditional <
		std::is_integral<T>::value,
		std::uniform_int_distribution<T>,
		std::uniform_real_distribution<T>
	> ::type dist_;		// 一様分布

public:
	/// コンストラクタ
	/**
		\param min 乱数最小値
		\param max 乱数最大値
		\param debug 乱数のシードを固定するか
	*/
	SimpleRandom(T min, T max, bool debug) : engine_(
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

	/// 乱数を発生させて取得
	T operator()() const
	{
		return dist_(engine_);
	}
};


/// 重複の無い一様分布の整数乱数を生成
/**
	\param n 発生させる値の個数
	\param min 最小値
	\param max 最大値
	\param debug 乱数のシードを固定するか

	\return 生成した乱数を格納したコンテナ

	\code
	auto rints = make_unique_numbers(10, 0, 20, true);	// [0, 20]の一様分布から10個重複無くサンプル

	for (auto v : rints) std::cout << v << std::endl;
	\endcode
*/
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