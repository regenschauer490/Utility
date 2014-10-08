/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_FOREACH_HPP
#define SIG_UTIL_FOREACH_HPP

#include "helper/helper.hpp"
#include "helper/container_helper.hpp"


/* かゆいところに手が届く反復処理関数 */

namespace sig
{
#undef max
#undef min

// 複数コンテナを反復処理 (先頭から順番、インデックスは共通)
// 引数で渡す関数オブジェクト(orラムダ)の引数を参照(&)にすることで変更操作も可能
// func: 関数オブジェクト(引数の順番は渡すコンテナの順番に対応)
template <class F, class... Cs>
void for_each(F const& func, Cs&... containers)
{
	const uint length = min(containers.size()...);
	iterative_assign(length, func, std::begin(containers)...);
}
	
// 複数コンテナを反復処理 (先頭から順番、インデックスは共通)
// 1反復毎に1ずつインクリメントされる添字変数を利用可 (initはその初期値)
// 引数で渡す関数オブジェクト(orラムダ)の引数を参照(&)にすることで変更操作も可能
// func: 関数オブジェクト(第1引数は添字変数.以降の引数の順番は渡すコンテナの順番に対応)
template <class F, class... Cs>
void for_each(F const& func, int init, Cs&... containers)
{
	const uint length = min(containers.size()...);
	iterative_assign(length, init, func, std::begin(containers)...);
}

// containersの各要素に対して、zip関数を適用しつつfoldで結果を集約
// fold_zipWith f1 f2 y xs = foldl f1 y (zipWith f2 xs)
template <class F1, class F2, class T, class... Cs>
auto fold_zipWith(F1 const& zip, F2 const& fold, T init, Cs&... containers)
{
	using TR = decltype(impl::eval(
		zip,
		std::declval<typename impl::container_traits<Cs>::value_type>()...
	));
	using R = decltype(impl::eval(
		fold,
		init,
		std::declval<T>()
	));

	TR result = init;
	const uint length = min(containers.size()...);
	iterative_fold(length, result, zip, fold, std::begin(containers)...);
	return result;
}

// コンテナから指定条件を満たす要素を抽出する
// pred: 添字と要素値を取り、boolを返す関数
template <class F, class C>
auto filter(F const& pred, C const& container, int init)
{
	C result;
	for (auto const& e : container){
		if (pred(init, e)) impl::container_traits<C>::add_element(result, e);
		++init;
	}
	return result;
}

// コンテナへの代入演算 (element-wise: container and container)
template <class OP, class C1, class C2, typename std::enable_if<impl::container_traits<C1>::exist && impl::container_traits<C2>::exist>::type*& = enabler>
void compound_assignment(
	OP const& assign_op,
	C1& dest,
	C2 const& src)
{
	for_each(assign_op, dest, src);
}

// コンテナへの代入演算 (element-wise: container and scalar)
template <class OP, class C, class T, typename std::enable_if<impl::container_traits<C>::exist && !impl::container_traits<T>::exist>::type*& = enabler>
void compound_assignment(
	OP const& assign_op,
	C& dest,
	T src)
{
	for (auto& e : dest){
		assign_op(e, src);
	}
}

#define SIG_MakeConpoundAssignmentFunctor(Name, Operator)\
template <class T>\
struct assign_ ## Name \
{\
	void operator()(T& dest, ParamType<T> value) const{ dest Operator value; }\
}

SIG_MakeConpoundAssignmentFunctor(plus, += );
SIG_MakeConpoundAssignmentFunctor(minus, -= );
SIG_MakeConpoundAssignmentFunctor(mult, *= );
SIG_MakeConpoundAssignmentFunctor(div, /= );

}
#endif
