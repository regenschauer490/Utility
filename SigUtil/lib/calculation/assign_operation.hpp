/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIGUTIL_ASSIGN_OPERATION_HPP
#define SIGUTIL_ASSIGN_OPERATION_HPP

#include "../helper/helper_modules.hpp"
#include "../helper/container_helper.hpp"
#include "../calculation/for_each.hpp"

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
template <class F, class C1, class C2,
	typename std::enable_if<impl::container_traits<C1>::exist && impl::container_traits<C2>::exist>::type*& = enabler
>
void compound_assignment(
	F&& assign_op,
	C1& dest,
	C2 const& src)
{
	sig::for_each(std::forward<F>(assign_op), dest, src);
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
template <class F, class C, class T,
	typename std::enable_if<impl::container_traits<C>::exist && !impl::container_traits<T>::exist>::type*& = enabler
>
void compound_assignment(
	F&& assign_op,
	C& dest,
	T src)
{
	for (auto& e : dest){
		std::forward<F>(assign_op)(e, src);
	}
}


/// compound_assignment の第1引数に指定する代入関数のプリセット（加算代入）
/**
	\pre 被代入側のオブジェクトに operator+=関数が定義されていること
*/
struct assign_plus_t
{
	template <class T1, class T2>
	void operator()(T1& v1, T2&& v2) const
	{
		v1 += std::forward<T2>(v2);
	}
};

/// compound_assignment の第1引数に指定する代入関数のプリセット（減算代入）
/**
	\pre 被代入側のオブジェクトに operator-=関数が定義されていること
*/
struct assign_minus_t
{
	template <class T1, class T2>
	void operator()(T1& v1, T2&& v2) const
	{
		v1 -= std::forward<T2>(v2);
	}
};

/// compound_assignment の第1引数に指定する代入関数のプリセット（乗算代入）
/**
	\pre 被代入側のオブジェクトに operator*=関数が定義されていること
*/
struct assign_mult_t
{
	template <class T1, class T2>
	void operator()(T1& v1, T2&& v2) const
	{
		v1 *= std::forward<T2>(v2);
	}
};

/// compound_assignment の第1引数に指定する代入関数のプリセット（除算代入）
/**
\pre 被代入側のオブジェクトに operator/=関数が定義されていること
*/
struct assign_div_t
{
	template <class T1, class T2>
	void operator()(T1& v1, T2&& v2) const
	{
		v1 /= std::forward<T2>(v2);
	}
};

}
#endif
