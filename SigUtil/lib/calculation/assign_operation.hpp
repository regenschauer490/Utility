﻿/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_ASSIGN_OPERATION_HPP
#define SIG_UTIL_ASSIGN_OPERATION_HPP

#include "../helper/helper.hpp"
#include "../helper/container_helper.hpp"


/// \file assign_operation.hpp ベクトル変数(コンテナ)向けの代入演算関数

namespace sig
{
/// コンテナへの代入演算 (element-wise: container and container)
/**
	indexが対応するsrcとdestの各要素に関数を適用し、その結果をdestコンテナに格納する

	\param assign_op srcとdestの各要素を引数に取る関数
	\param dest 代入先のコンテナ（\ref sig_container ）
	\param src 任意のコンテナ（\ref sig_container ）

	\code
	const array<int, 4> data0{ 1, 2, 3, 4 };		// sig::array
	std::list<double> data1{ 1.1, -3.3, 5.5 };

	compound_assignment([](double& v1, int v2){ v1 -= v2; }, data1, data0);

	data1;		// { 0.1, -5.3, 2.5 }
	\endcode
*/
template <class OP, class C1, class C2,
	typename std::enable_if<impl::container_traits<C1>::exist && impl::container_traits<C2>::exist>::type*& = enabler
>
void compound_assignment(
	OP const& assign_op,
	C1& dest,
	C2 const& src)
{
	for_each(assign_op, dest, src);
}

/// コンテナへの代入演算 (element-wise: container and scalar)
/**
	srcとdestの各要素に関数を適用し、その結果をdestコンテナに格納する

	\param assign_op srcの各要素を処理する関数
	\param dest 代入先のコンテナ（\ref sig_container ）
	\param src 任意の値

	\code
	std::vector<std::string> data{ "ein", "zwei", "drei" };

	compound_assignment([](std::string& str, std::string add){ str += add; }, data, std::string("-hander"));

	data;		// { "ein-hander", "zwei-hander", "drei-hander" }
	\endcode
*/
template <class OP, class C, class T,
	typename std::enable_if<impl::container_traits<C>::exist && !impl::container_traits<T>::exist>::type*& = enabler
>
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
