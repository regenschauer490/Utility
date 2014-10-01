/*
Copyrightrandom_unique_numbers 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_BINARY_OPERATION_HPP
#define SIG_UTIL_BINARY_OPERATION_HPP

#include "../functional.hpp"


/// \file binary_operation.hpp スカラ変数とベクトル変数(+コンテナの種類)を気にせず使える二項演算関数

namespace sig
{

/// 二項演算 (scalar and scalar)
/**
	\param func 二項演算関数
	\param v1 引数1
	\param v2 引数2
	\return 演算結果
*/
template <class OP, class T1, class T2>
auto binary_operation(OP func, T1 v1, T2 v2) ->decltype(v1 + v2)
{
	return func(v1, v2);
}

/// 二項演算 (element-wise: container and container)
template <class OP, class C1, class C2>
auto binary_operation(OP func, C1 const& c1, C2 const& c2)
	->typename container_traits<C1>::template rebind<decltype(impl::eval(
	func,
	std::declval<typename container_traits<C1>::value_type>(),
	std::declval<typename container_traits<C2>::value_type>()
	))>
{
	using T1 = typename container_traits<C1>::value_type;
	using T2 = typename container_traits<C2>::value_type;
	return zipWith([&](ParamType<T1> v1, ParamType<T2> v2){ return func(v1, v2); }, c1, c2);
}

/// 二項演算 (element-wise: container and scalar)
template <class OP, class C, class T, class = typename container_traits<C>::value_type>
auto binary_operation(OP func, C const& c, T v)
	->typename container_traits<C>::template rebind<decltype(impl::eval(
	func,
	std::declval<typename container_traits<C>::value_type>(),
	v
	))>
{
	using CT = typename container_traits<C>::value_type;
	using RT = decltype(impl::eval(func, std::declval<typename container_traits<C>::value_type>(), v));
	using R = typename container_traits<C>::template rebind<RT>;

	R r;
	for (ParamType<CT> vc : c) container_traits<R>::add_element(r, func(vc, v));
	return r;
}

/// 二項演算 (element-wise: scalar and container)
template <class OP, class T, class C, class = typename container_traits<C>::value_type>
auto binary_operation(OP func, T v, C const& c)
	->typename container_traits<C>::template rebind<decltype(impl::eval(
	func,
	std::declval<typename container_traits<C>::value_type>(),
	v
	))>
{
	using CT = typename container_traits<C>::value_type;
	using RT = decltype(impl::eval(func, std::declval<typename container_traits<C>::value_type>(), v));
	using R = typename container_traits<C>::template rebind<RT>;

	R r;
	for (ParamType<CT> vc : c) container_traits<R>::add_element(r, func(v, vc));
	return r;
}


///  四則演算を一般的に記述するための関数群

#define SIG_MakeBinaryOperation(FunctionName, Operator)\
	template <class T1, class T2>\
	auto FunctionName(T1 v1, T2 v2) ->decltype(v1 Operator v2)\
	{\
		return v1 Operator v2;\
	}\
\
	template <class C1, class C2, class = decltype(std::declval<typename container_traits<C1>::value_type>() Operator std::declval<typename container_traits<C2>::value_type>())>\
	auto FunctionName(C1 const& c1, C2 const& c2)\
		->typename container_traits<C1>::template rebind<decltype(\
		std::declval<typename container_traits<C1>::value_type>() Operator std::declval<typename container_traits<C2>::value_type>()\
		)>\
	{\
		using T1 = typename container_traits<C1>::value_type;\
		using T2 = typename container_traits<C2>::value_type;\
		return binary_operation([](T1 v1, T2 v2){ return v1 Operator v2; }, c1, c2);\
	}\
\
	template <class C, class T, class = decltype(std::declval<typename container_traits<C>::value_type>() Operator std::declval<T>())>\
	auto FunctionName(C const& c, T v)\
		->typename container_traits<C>::template rebind<decltype(\
		std::declval<typename container_traits<C>::value_type>() Operator v\
		)>\
	{\
		using CT = typename container_traits<C>::value_type;\
		return binary_operation([](CT v1, T v2){ return v1 Operator v2; }, c, v);\
	}\
\
	template <class T, class C, class = decltype(std::declval<typename container_traits<C>::value_type>() Operator std::declval<T>())>\
	auto FunctionName(T v, C const& c)\
		->typename container_traits<C>::template rebind<decltype(\
		std::declval<typename container_traits<C>::value_type>() Operator v\
		)>\
	{\
		using CT = typename container_traits<C>::value_type; \
		return binary_operation([](T v1, CT v2){ return v1 Operator v2; }, v, c); \
	}\


/// 加法関数plusの自動生成
/**
	ex: plus(1.5, {1,2,3}). return : {2.5, 3.5, 4.5}
*/
SIG_MakeBinaryOperation(plus, +);

/// 減法関数minusの自動生成
SIG_MakeBinaryOperation(minus, -);

/// 乗法関数multの自動生成
SIG_MakeBinaryOperation(mult, *);
/// 乗法関数multipliesの自動生成
SIG_MakeBinaryOperation(multiplies, *);

/// 除法関数divの自動生成
SIG_MakeBinaryOperation(div, / );
/// 除法関数dividesの自動生成
SIG_MakeBinaryOperation(divides, / );

}

#endif