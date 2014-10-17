/*
Copyrightrandom_unique_numbers 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_BINARY_OPERATION_HPP
#define SIG_UTIL_BINARY_OPERATION_HPP

#include "../functional/high_order.hpp"


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
auto binary_operation(OP&& func, T1&& v1, T2&& v2) ->decltype(std::forward<T1>(v1) +std::forward<T1>(v2))
{
	return std::forward<OP>(func)(std::forward<T1>(v1), std::forward<T2>(v2));
}

/// 二項演算 (element-wise: container and container)
template <class OP, class C1, class C2,
	class CR1 = typename impl::remove_const_reference<C1>::type,
	class CR2 = typename impl::remove_const_reference<C2>::type,
	class CT1 = typename impl::container_traits<typename std::remove_reference<CR1>::type>::value_type,
	class CT2 = typename impl::container_traits<typename std::remove_reference<CR2>::type>::value_type
>
auto binary_operation(OP&& func, C1&& c1, C2&& c2)
	->typename impl::container_traits<CR1>::template rebind<decltype(impl::eval(
		std::forward<OP>(func),
		std::declval<CT1>(),
		std::declval<CT2>()
	))>
{
	return zipWith(
		[&](ParamType<CT1> v1, ParamType<CT2> v2){ return std::forward<OP>(func)(v1, v2); },
		std::forward<C1>(c1),
		std::forward<C2>(c2)
	);
}

/// 二項演算 (element-wise: container and scalar)
template <class OP, class C, class T,
	class CR = typename impl::remove_const_reference<C>::type,
	class CT = typename impl::container_traits<CR>::value_type,
	class = typename impl::container_traits<CR>::value_type
>
auto binary_operation(OP&& func, C&& c, T&& v)
	->typename impl::container_traits<CR>::template rebind<decltype(impl::eval(
		std::forward<OP>(func),
		std::declval<CT>(),
		v
	))>
{
	using RT = decltype(impl::eval(std::forward<OP>(func), std::declval<CT>(), v));
	using R = typename impl::container_traits<CR>::template rebind<RT>;
	using AT = impl::actual_element<C>::type;

	R r;
	for (auto&& e : std::forward<C>(c)){
		impl::container_traits<R>::add_element(r, std::forward<OP>(func)(std::forward<AT>(e), v));
	}
	return r;
}

/// 二項演算 (element-wise: scalar and container)
template <class OP, class T, class C,
	class CT = typename impl::container_traits<typename impl::remove_const_reference<C>::type>::value_type,
	class CR = typename impl::remove_const_reference<C>::type,
	class = typename impl::container_traits<CR>::value_type
>
auto binary_operation(OP&& func, T&& v, C&& c)
	->typename impl::container_traits<CR>::template rebind<decltype(impl::eval(
		std::forward<OP>(func),
		v,
		std::declval<CT>()
	))>
{
	using RT = decltype(impl::eval(func, v, std::declval<CT>()));
	using R = typename impl::container_traits<CR>::template rebind<RT>;
	using AT = impl::actual_element<C>::type;

	R r;
	for (auto&& e : std::forward<C>(c)){
		impl::container_traits<R>::add_element(r, std::forward<OP>(func)(v, std::forward<AT>(e)));
	}
	return r;
}


///  四則演算を一般的に記述するための関数群

#define SIG_MakeBinaryOperation(FunctionName, Operator)\
	template <class T1, class T2>\
	auto FunctionName(T1&& v1, T2&& v2) ->decltype(v1 Operator v2)\
	{\
		return v1 Operator v2;\
	}\
\
	template <class C1, class C2,\
		class CR1 = typename impl::remove_const_reference<C1>::type, \
		class CR2 = typename impl::remove_const_reference<C2>::type, \
		class CT1 = typename impl::container_traits<typename std::remove_reference<CR1>::type>::value_type,\
		class CT2 = typename impl::container_traits<typename std::remove_reference<CR2>::type>::value_type\
	>\
	auto FunctionName(C1&& c1, C2&& c2)\
		->typename impl::container_traits<CR1>::template rebind<decltype(\
			std::declval<CT1>() Operator std::declval<CT2>()\
		)>\
	{\
		using AT1 = impl::actual_element<C1>::type;\
		using AT2 = impl::actual_element<C2>::type;\
		return binary_operation([](AT1 v1, AT2 v2){ return v1 Operator v2; }, std::forward<C1>(c1), std::forward<C2>(c2)); \
	}\
\
	template <class C, class T,\
		class CR = typename impl::remove_const_reference<C>::type,\
		class CT = typename impl::container_traits<CR>::value_type\
	>\
	auto FunctionName(C&& c, T&& v)\
		->typename impl::container_traits<CR>::template rebind<decltype(\
			std::declval<CT>() Operator v\
		)>\
	{\
		using AT = impl::actual_element<C>::type;\
		return binary_operation([](AT v1, T v2){ return v1 Operator v2; }, std::forward<C>(c), std::forward<T>(v)); \
	}\
\
	template <class T, class C,\
		class CT = typename impl::container_traits<typename impl::remove_const_reference<C>::type>::value_type, \
		class CR = typename impl::remove_const_reference<C>::type\
	>\
	auto FunctionName(T&& v, C&& c)\
		->typename impl::container_traits<CR>::template rebind<decltype(\
			v Operator std::declval<CT>()\
		)>\
	{\
		using AT = impl::actual_element<C>::type;\
		return binary_operation([](T v1, AT v2){ return v1 Operator v2; }, std::forward<T>(v), std::forward<C>(c)); \
	}\


/// 加法関数plusの自動生成
/**
	\code
	std::vector<int> vec{1,2,3};
	std::list<int> list{3,2,1};

	auto p1 = plus(1, 2.5);			// 3.5
	auto p2 = plus(1.5, vec);		// vector<double>{2.5, 3.5, 4.5}
	auto p3 = plus(list, 2);		// list<int>{5, 4, 3}
	auto p4 = plus(vec, list);		// vector<int>{4, 4, 4}
	\endcode
*/
SIG_MakeBinaryOperation(plus, +);

/// 減法関数minusの自動生成
/**
	\sa SIG_MakeBinaryOperation(plus, +)
*/
SIG_MakeBinaryOperation(minus, -);

/// 乗法関数multの自動生成
/**
	\sa SIG_MakeBinaryOperation(plus, +)
*/
SIG_MakeBinaryOperation(mult, *);
SIG_MakeBinaryOperation(multiplies, *);

/// 除法関数divの自動生成
/**
	\sa SIG_MakeBinaryOperation(plus, +)
*/
SIG_MakeBinaryOperation(div, / );
SIG_MakeBinaryOperation(divides, / );

}

#endif