/*
Copyright(c) 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_CALCULATION_HPP
#define SIG_UTIL_CALCULATION_HPP

#include "sigutil.hpp"
#include "functional.hpp"

namespace sig
{
	// “ñ€‰‰Z (scalar and scalar)
	template <class OP, class T1, class T2>
	auto binary_operation(OP func, T1 v1, T2 v2) ->decltype(v1 + v2)
	{
		return func(v1, v2);
	}

	// “ñ€‰‰Z (element-wise: container and container)
	template <class OP, class C1, class C2>
	auto binary_operation(OP func, C1 const& c1, C2 const& c2)
		->typename container_traits<C1>::template rebind<decltype(eval(
		func,
		std::declval<typename container_traits<C1>::value_type>(),
		std::declval<typename container_traits<C2>::value_type>()
		))>
	{
		using T1 = typename container_traits<C1>::value_type;
		using T2 = typename container_traits<C2>::value_type;
		return zipWith([&](ParamType<T1> v1, ParamType<T2> v2){ return func(v1, v2); }, c1, c2);
	}

	// “ñ€‰‰Z (element-wise: container and scalar)
	template <class OP, class C, class T, class = typename container_traits<C>::value_type>
	auto binary_operation(OP func, C const& c, T v)
		->typename container_traits<C>::template rebind<decltype(eval(
		func,
		std::declval<typename container_traits<C>::value_type>(),
		v
		))>
	{
		using CT = typename container_traits<C>::value_type;
		using RT = decltype(eval(func, std::declval<typename container_traits<C>::value_type>(), v));
		using R = typename container_traits<C>::template rebind<RT>;

		R r;
		for (ParamType<CT> vc : c) container_traits<R>::add_element(r, func(vc, v));
		return r;
	}

	// “ñ€‰‰Z (element-wise: scalar and container)
	template <class OP, class T, class C, class = typename container_traits<C>::value_type>
	auto binary_operation(OP func, T v, C const& c)
		->typename container_traits<C>::template rebind<decltype(eval(
		func,
		std::declval<typename container_traits<C>::value_type>(),
		v
		))>
	{
		using CT = typename container_traits<C>::value_type;
		using RT = decltype(eval(func, std::declval<typename container_traits<C>::value_type>(), v));
		using R = typename container_traits<C>::template rebind<RT>;

		R r;
		for (ParamType<CT> vc : c) container_traits<R>::add_element(r, func(v, vc));
		return r;
	}


	/* ‰ÁŒ¸æœ‚Ì‰‰Z‚ğˆê”Ê“I‚É‹Lq‚·‚é‚½‚ß‚ÌŠÖ”ŒQ */

#define SIG_MakeBinaryOperation(FunctionName, Operator)\
	template <class T1, class T2>\
	auto FunctionName(T1 v1, T2 v2) ->decltype(v1 Operator v2)\
	{\
		return v1 Operator v2;\
	}\
\
	template <class C1, class C2, class = decltype(std::declval<typename container_traits<C1>::value_type>() Operator std::declval<typename container_traits<C2>::value_type>())>\
	auto FunctionName(C1 const& c1, C2 const& c2)\
	{\
		using T1 = typename container_traits<C1>::value_type;\
		using T2 = typename container_traits<C2>::value_type;\
		return binary_operation([](T1 v1, T2 v2){ return v1 Operator v2; }, c1, c2);\
	}\
\
	template <class C, class T, class = decltype(std::declval<typename container_traits<C>::value_type>() Operator std::declval<T>())>\
	auto FunctionName(C const& c, T v)\
	{\
		using CT = typename container_traits<C>::value_type;\
		return binary_operation([](CT v1, T v2){ return v1 Operator v2; }, c, v);\
	}\
\
	template <class T, class C, class = decltype(std::declval<typename container_traits<C>::value_type>() Operator std::declval<T>())>\
	auto FunctionName(T v, C const& c)\
	{\
		using CT = typename container_traits<C>::value_type; \
		return binary_operation([](T v1, CT v2){ return v1 Operator v2; }, v, c); \
	}\

	SIG_MakeBinaryOperation(plus, +);

	SIG_MakeBinaryOperation(minus, -);

	SIG_MakeBinaryOperation(multiplies, *);

	SIG_MakeBinaryOperation(divides, / );


	// ƒRƒ“ƒeƒi‚Ö‚Ì‘ã“ü‰‰Z (element-wise: container and container)
	template <class OP, class C1, class C2, typename std::enable_if<container_traits<C1>::exist && container_traits<C2>::exist>::type*& = enabler>
	void compound_assignment(OP const& assign_op, C1& dest, C2 const& src)
	{
		auto it1 = std::begin(dest), end1 = std::end(dest);
		auto it2 = std::begin(src), end2 = std::end(src);

		for(; it1 != end1 && it2 != end2; ++it1, ++it2){
			assign_op(*it1, *it2);
		}
	}

	// ƒRƒ“ƒeƒi‚Ö‚Ì‘ã“ü‰‰Z (element-wise: container and scalar)
	template <class OP, class C, class T, typename std::enable_if<container_traits<C>::exist && !container_traits<T>::exist>::type*& = enabler>
	void compound_assignment(OP const& assign_op, C& dest, T src)
	{
		for(auto& e : dest){
			assign_op(e, src);
		}
	}
}
#endif