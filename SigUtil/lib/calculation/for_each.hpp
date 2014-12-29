/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIGUTIL_FOREACH_HPP
#define SIGUTIL_FOREACH_HPP

#include "../helper/helper_modules.hpp"
#include "../helper/container_helper.hpp"


/// \file for_each.hpp かゆいところに手が届く反復処理関数

namespace sig
{
/// 複数コンテナを反復処理 
/**
	先頭の要素から順番に関数を適用し、いずれかのコンテナの末尾に到達するまで繰り返す．\n
	適用する関数オブジェクトの引数を参照(&)にすることで、値の変更が可能
	
	\param func 関数オブジェクト(引数の順番は渡すcontainersの順番に対応)
	\param containers 処理対象のコンテナ（\ref sig_container ）

	\sa for_each(F const& func, int init, Cs&&... containers)

	\code
	array<double, 4> data1{ 1.1, 2.2, 3.3, 4.4 };		// sig::array
	const std::list<int> data2{ 1, -3, 5 };
	const std::multiset<std::string> data3{ "a", "bb", "ccc" };

	for_each([](double& a, int b, std::string const& s)
	{
		auto tmp = b + static_cast<int>(s.length());
		a += tmp;
	},
		data1, data2, data3
	);

	data1;		// { 3.1, 1.2, 11.3, 4.4 }
	\endcode
*/
template <class F, class... Cs>
void for_each(F&& func, Cs&&... containers)
{
	const uint length = min(containers.size()...);
	iterative_assign(length, std::forward<F>(func), impl::begin(std::forward<Cs>(containers))...);
}
	
/// 複数コンテナを反復処理 (添字変数の利用可)
/**
	1反復毎に1ずつインクリメントされる添字変数を利用可 (initはその初期値). \n
	先頭の要素から順番に関数を適用し、いずれかのコンテナの末尾に到達するまで繰り返す．\n
	引数で渡す関数オブジェクト(orラムダ)の引数を参照(&)にすることで変更操作も可能

	\param func 関数オブジェクト(第1引数は添字変数.以降の引数の順番は渡すcontainersの順番に対応)
	\param init 添字変数の初期値
	\param containers 処理対象のコンテナ（\ref sig_container ）

	\code
	const std::vector<int> data1{ 1, -3, 5 };
	const std::list<double> data2{ 1.0, -3.0, 5.0 };
	const std::multiset<int, std::greater<int>> data3{ 1, -3, 5 };
	const std::map<int, char> data4{ {0, 'a'}, {1, 'b'}, {2, 'c'}, {3, 'd'} };
	array<std::string, 3> data5{ "A", "B", "C" };

	for_each([](int i, int a, double b, int c, char d, std::string& e)
	{
		e.append(std::to_string(i) + std::to_string(a + b) + d + std::to_string(c));
	},
		-1, data1, data2, data3, data4, data5
	);
	\endcode
*/
template <class F, class... Cs>
void for_each(F&& func, int init, Cs&&... containers)
{
	const uint length = min(containers.size()...);
	iterative_assign(length, init, std::forward<F>(func), impl::begin(std::forward<Cs>(containers))...);
}

}
#endif
