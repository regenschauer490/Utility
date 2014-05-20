/*
Copyright(c) 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_FOREACH_HPP
#define SIG_UTIL_FOREACH_HPP

#include "helper.hpp"
#include "container_helper.hpp"


/* かゆいところに手が届く反復処理関数 */

namespace sig
{
#undef max
#undef min

// 複数コンテナを反復処理 (先頭から順番、インデックスは共通)
// 引数で渡す関数オブジェクト(やラムダ)の引数を参照(&)にすることで変更操作も可能
template <class F, class... Cs>
auto for_each(F const& func, Cs&... containers)
{
	const uint length = min(containers.size()...);
	iterative_assign(length, func, std::begin(containers)...);
}
	
// 複数コンテナを反復処理 (先頭から順番、インデックスは共通)
// 1反復毎に1ずつインクリメントされる変数もある (initはその初期値)
// 引数で渡す関数オブジェクト(やラムダ)の引数を参照(&)にすることで変更操作も可能
template <class F, class... Cs>
auto for_each(F const& func, int init, Cs&... containers)
{
	const uint length = min(containers.size()...);
	iterative_assign(length, init, func, std::begin(containers)...);
}


// コンテナへの代入演算 (element-wise: container and container)
template <class OP, class C1, class C2, typename std::enable_if<container_traits<C1>::exist && container_traits<C2>::exist>::type*& = enabler>
void compound_assignment(
	OP const& assign_op,
	C1& dest,
	C2 const& src)
{
	for_each(assign_op, dest, src);
}

// コンテナへの代入演算 (element-wise: container and scalar)
template <class OP, class C, class T, typename std::enable_if<container_traits<C>::exist && !container_traits<T>::exist>::type*& = enabler>
void compound_assignment(
	OP const& assign_op,
	C& dest,
	T src)
{
	for (auto& e : dest){
		assign_op(e, src);
	}
}

}
#endif
