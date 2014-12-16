/*
Copyrightrandom_unique_numbers 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_BINARY_OPERATION_HPP
#define SIG_UTIL_BINARY_OPERATION_HPP

#include "../helper/helper_modules.hpp"
#include "../helper/container_helper.hpp"


/// \file binary_operation.hpp スカラ変数とベクトル変数(+コンテナの種類)を気にせず使える二項演算関数
/** \file
	主に、四則計算をスカラ変数（ex. int a）とベクトル変数（ex. vector<double> v）を区別なくelement-wiseな計算を統一的に記述することが可能になる．\n
	すなわち、R言語やnumpyの様な記述を行うことができる

	\code
		int a = 2;
		std::vector<double> vec{ 1.1, 2.2, 3.3 };
		sig::array<int, 5> ar{ 1, 2, 3, 4, 5 };

		using sig::operator*;

		auto r1 = a * vec;	// std::vector<double>{ 2.2, 4.4, 6.6 }
		auto r2 = ar * vec;	// sig::array<double, 5>{ 1.1, 2.2, 9.9, ※, ※ }.　※は未初期化値を意味している.詳しくは array.hpp を参照
	\endcode

	ただし、計算結果を戻り値として返すため、使用する\ref sig_container がメモリ領域を動的確保するコンテナの場合、速度パフォーマンスを重要視する処理には適していない．
*/

namespace sig
{

/// 二項演算 (scalar and scalar)
/**
	\param func 二項演算関数
	\param v1 引数1
	\param v2 引数2

	\return 演算結果
*/
template <class OP, class T1, class T2,
	typename std::enable_if<
		(!impl::container_traits<typename impl::remove_const_reference<T1>::type>::exist) && (!impl::container_traits<typename impl::remove_const_reference<T2>::type>::exist)
	>::type*& = enabler
>
auto binary_operation(OP&& func, T1&& v1, T2&& v2)
	->typename impl::remove_const_reference< decltype(impl::eval(std::forward<OP>(func), std::forward<T1>(v1), std::forward<T2>(v2)))>::type
{
	return std::forward<OP>(func)(std::forward<T1>(v1), std::forward<T2>(v2));
}

/// 二項演算 (element-wise: container and container)
template <class OP, class C1, class C2,
	class CR1 = typename impl::remove_const_reference<C1>::type,
	class CR2 = typename impl::remove_const_reference<C2>::type,
	class AT1 = typename impl::forward_element<C1>::type,
	class AT2 = typename impl::forward_element<C2>::type,
	typename std::enable_if<
		impl::container_traits<CR1>::exist && impl::container_traits<CR2>::exist
	>::type*& = enabler
>
auto binary_operation(OP&& func, C1&& c1, C2&& c2)
	->typename impl::container_traits<CR1>::template rebind<
		typename impl::remove_const_reference<decltype(impl::eval(
			std::forward<OP>(func),
			std::declval<AT1>(),
			std::declval<AT2>()
		))>::type
	>
{
	using RT = decltype(impl::eval(std::forward<OP>(func), std::declval<AT1>(), std::declval<AT2>()));
	using R = typename impl::container_traits<CR1>::template rebind<RT>;

	const uint length = min(c1.size(), c2.size());
	R result = impl::container_traits<R>::make(length);

	iterative_make(length, result, std::forward<OP>(func), impl::begin(std::forward<C1>(c1)), impl::begin(std::forward<C2>(c2)));
	return result;
}

/// 二項演算 (element-wise: container and scalar)
template <class OP, class C, class T,
	class CR = typename impl::remove_const_reference<C>::type,
	class ET = typename impl::forward_element<C>::type,
	typename std::enable_if<
		impl::container_traits<CR>::exist && (!impl::container_traits<typename impl::remove_const_reference<T>::type>::exist)
	>::type*& = enabler
>
auto binary_operation(OP&& func, C&& c, T&& v)
	->typename impl::container_traits<CR>::template rebind<
		typename impl::remove_const_reference<decltype(impl::eval(
			std::forward<OP>(func),
			std::declval<ET>(),
			v
		))>::type
	>
{
	using RT = decltype(impl::eval(std::forward<OP>(func), std::declval<ET>(), v));
	using R = typename impl::container_traits<CR>::template rebind<RT>;

	R result = impl::container_traits<R>::make(c.size());

	for (auto&& e : std::forward<C>(c)){
		impl::container_traits<R>::add_element(result, std::forward<OP>(func)(static_cast<ET>(e), v));
	}
	return result;
}

/// 二項演算 (element-wise: scalar and container)
template <class OP, class T, class C,
	class ET = typename impl::forward_element<C>::type,
	class CR = typename impl::remove_const_reference<C>::type,
	typename std::enable_if<
		(!impl::container_traits<typename impl::remove_const_reference<T>::type>::exist) && impl::container_traits<CR>::exist
	>::type*& = enabler
>
auto binary_operation(OP&& func, T&& v, C&& c)
	->typename impl::container_traits<CR>::template rebind<
		typename impl::remove_const_reference<decltype(impl::eval(
			std::forward<OP>(func),
			v,
			std::declval<ET>()
		))>::type
	>
{
	using RT = decltype(impl::eval(std::forward<OP>(func), v, std::declval<ET>()));
	using R = typename impl::container_traits<CR>::template rebind<RT>;

	R result = impl::container_traits<R>::make(c.size());

	for (auto&& e : std::forward<C>(c)){
		impl::container_traits<R>::add_element(result, std::forward<OP>(func)(v, static_cast<ET>(e)));
	}
	return result;
}



template <class T1, class T2>
auto plus(T1&& v1, T2&& v2)
	->decltype(binary_operation(plus_t(), std::forward<T1>(v1), std::forward<T2>(v2)))
{
	return binary_operation(plus_t(), std::forward<T1>(v1), std::forward<T2>(v2));
}


template <class T1, class T2>
auto minus(T1&& v1, T2&& v2)
	->decltype(binary_operation(minus_t(), std::forward<T1>(v1), std::forward<T2>(v2)))
{
	return binary_operation(minus_t(), std::forward<T1>(v1), std::forward<T2>(v2));
}


template <class T1, class T2>
auto mult(T1&& v1, T2&& v2)
	->decltype(binary_operation(mult_t(), std::forward<T1>(v1), std::forward<T2>(v2)))
{
	return binary_operation(mult_t(), std::forward<T1>(v1), std::forward<T2>(v2));
}


template <class T1, class T2>
auto div(T1&& v1, T2&& v2)
	->decltype(binary_operation(div_t(), std::forward<T1>(v1), std::forward<T2>(v2)))
{
	return binary_operation(div_t(), std::forward<T1>(v1), std::forward<T2>(v2));
}


template <class T1, class T2,
	class TR1 = typename impl::remove_const_reference<T1>::type,
	class TR2 = typename impl::remove_const_reference<T2>::type,
	typename std::enable_if<
		impl::container_traits<TR1>::exist || impl::container_traits<TR2>::exist
	>::type*& = enabler
>
auto operator+(T1&& v1, T2&& v2)
	->decltype(binary_operation(plus_t(), std::forward<T1>(v1), std::forward<T2>(v2)))
{
	return binary_operation(plus_t(), std::forward<T1>(v1), std::forward<T2>(v2));
}



template <class T1, class T2,
	class TR1 = typename impl::remove_const_reference<T1>::type,
	class TR2 = typename impl::remove_const_reference<T2>::type,
	typename std::enable_if<
		impl::container_traits<TR1>::exist || impl::container_traits<TR2>::exist
	>::type*& = enabler
>
auto operator-(T1&& v1, T2&& v2)
	->decltype(binary_operation(minus_t(), std::forward<T1>(v1), std::forward<T2>(v2)))
{
	return binary_operation(minus_t(), std::forward<T1>(v1), std::forward<T2>(v2));
}



template <class T1, class T2,
	class TR1 = typename impl::remove_const_reference<T1>::type,
	class TR2 = typename impl::remove_const_reference<T2>::type,
	typename std::enable_if<
		impl::container_traits<TR1>::exist || impl::container_traits<TR2>::exist
	>::type*& = enabler
>
auto operator*(T1&& v1, T2&& v2)
	->decltype(binary_operation(mult_t(), std::forward<T1>(v1), std::forward<T2>(v2)))
{
	return binary_operation(mult_t(), std::forward<T1>(v1), std::forward<T2>(v2));
}



template <class T1, class T2,
	class TR1 = typename impl::remove_const_reference<T1>::type,
	class TR2 = typename impl::remove_const_reference<T2>::type,
	typename std::enable_if<
		impl::container_traits<TR1>::exist || impl::container_traits<TR2>::exist
	>::type*& = enabler
>
auto operator/(T1&& v1, T2&& v2)
	->decltype(binary_operation(div_t(), std::forward<T1>(v1), std::forward<T2>(v2)))
{
	return binary_operation(div_t(), std::forward<T1>(v1), std::forward<T2>(v2));
}



/*
///  四則演算を一般的に記述するための関数群
#define SIG_MakeBinaryOperation(FunctionName, Operator)\
	template <class T1, class T2,\
		typename std::enable_if<\
			(!impl::container_traits<typename impl::remove_const_reference<T1>::type>::exist) && (!impl::container_traits<typename impl::remove_const_reference<T2>::type>::exist)\
		>::type*& = enabler\
	>\
	auto FunctionName(T1&& v1, T2&& v2) ->typename impl::remove_const_reference<decltype(std::forward<T1>(v1) Operator std::forward<T2>(v2))>::type\
	{\
		return std::forward<T1>(v1) Operator std::forward<T2>(v2); \
	}\
\
	template <class C1, class C2,\
		class CR1 = typename impl::remove_const_reference<C1>::type, \
		class CR2 = typename impl::remove_const_reference<C2>::type, \
		class AT1 = typename impl::forward_element<C1>::type,\
		class AT2 = typename impl::forward_element<C2>::type,\
		typename std::enable_if<\
			impl::container_traits<CR1>::exist && impl::container_traits<CR2>::exist\
		>::type*& = enabler\
	>\
	auto FunctionName(C1&& c1, C2&& c2)\
		->typename impl::container_traits<CR1>::template rebind<\
			typename impl::remove_const_reference<decltype(\
				std::declval<AT1>() Operator std::declval<AT2>()\
			)>::type\
		>\
	{\
		return binary_operation([](AT1 v1, AT2 v2){ return static_cast<AT1>(v1) Operator static_cast<AT2>(v2); }, std::forward<C1>(c1), std::forward<C2>(c2)); \
	}\
\
	template <class C, class T,\
		class CR = typename impl::remove_const_reference<C>::type,\
		class ET = typename impl::forward_element<C>::type,\
		typename std::enable_if<\
			impl::container_traits<CR>::exist && (!impl::container_traits<typename impl::remove_const_reference<T>::type>::exist)\
		>::type*& = enabler\
	>\
	auto FunctionName(C&& c, T&& v)\
		->typename impl::container_traits<CR>::template rebind<\
			typename impl::remove_const_reference<decltype(\
				std::declval<ET>() Operator v\
			)>::type\
		>\
	{\
		return binary_operation([](ET v1, T v2){ return static_cast<ET>(v1) Operator v2; }, std::forward<C>(c), std::forward<T>(v)); \
	}\
\
	template <class T, class C,\
		class ET = typename impl::forward_element<C>::type, \
		class CR = typename impl::remove_const_reference<C>::type,\
		typename std::enable_if<\
			(!impl::container_traits<typename impl::remove_const_reference<T>::type>::exist) && impl::container_traits<CR>::exist\
		>::type*& = enabler\
	>\
	auto FunctionName(T&& v, C&& c)\
		->typename impl::container_traits<CR>::template rebind<\
			typename impl::remove_const_reference<decltype(\
				v Operator std::declval<ET>()\
			)>::type\
		>\
	{\
		return binary_operation([](T v1, ET v2){ return v1 Operator static_cast<ET>(v2); }, std::forward<T>(v), std::forward<C>(c)); \
	}\
*/

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
//SIG_MakeBinaryOperation(plus, +);

/// 減法関数minusの自動生成
/**
	\sa SIG_MakeBinaryOperation(plus, +)
*/
//SIG_MakeBinaryOperation(minus, -);

/// 乗法関数multの自動生成
/**
	\sa SIG_MakeBinaryOperation(plus, +)
*/
//SIG_MakeBinaryOperation(mult, *);
//SIG_MakeBinaryOperation(multiplies, *);

/// 除法関数divの自動生成
/**
	\sa SIG_MakeBinaryOperation(plus, +)
*/
//SIG_MakeBinaryOperation(div, / );
//SIG_MakeBinaryOperation(divides, / );


}
#endif