/*
Copyright(c) 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_CALCULATION_HPP
#define SIG_UTIL_CALCULATION_HPP

#include "sigutil.hpp"
#include "functional.hpp"
#include "iteration.hpp"


/* スカラ変数とベクトル変数(+コンテナの種類)を気にせず使える演算関数 */

namespace sig
{
// 二項演算 (scalar and scalar)
template <class OP, class T1, class T2>
auto binary_operation(OP func, T1 v1, T2 v2) ->decltype(v1 + v2)
{
	return func(v1, v2);
}

// 二項演算 (element-wise: container and container)
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

// 二項演算 (element-wise: container and scalar)
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

// 二項演算 (element-wise: scalar and container)
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


/* 加減乗除の演算を一般的に記述するための関数群 */

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

SIG_MakeBinaryOperation(plus, +);

SIG_MakeBinaryOperation(minus, -);

SIG_MakeBinaryOperation(multiplies, *);

SIG_MakeBinaryOperation(divides, / );


// 総和
// R: 戻り値型（桁あふれの可能性がある場合に明示的に指定する）
template <class R = void, class C = void>
auto sum(C const& data)
{
	using RT = typename SameIf<R, void, typename container_traits<C>::value_type, R>::type;
	return std::accumulate(std::begin(data), std::end(data), static_cast<RT>(0), std::plus<RT>{});
}

// access_func: コンテナの要素にアクセスし、そのオブジェクトから値を取得する関数を指定
template <class R = void, class C = void, class Pred = void>
auto sum(C const& data, Pred const& access_func)
{
	using T = typename container_traits<C>::value_type;
	using RT = typename SameIf<R, void, decltype(eval(access_func, std::declval<T>())), R>::type;

	return std::accumulate(std::begin(data), std::end(data), static_cast<RT>(0), [&](RT sum, T const& e){ return sum + access_func(e); });
}

// 行列の指定行の総和
// R: 戻り値型（桁あふれの可能性がある場合に明示的に指定する）
template <class R = void, class CC = void>
auto sum_row(CC const& matrix, uint index)
{
	assert(index < matrix.size());
	return sum(matrix[index]);
}

// 行列の指定列の総和
// R: 戻り値型（桁あふれの可能性がある場合に明示的に指定する）
template <class R = void, class CC = void>
auto sum_col(CC const& matrix, uint index)
{
	using T = typename container_traits<CC>::value_type;
	return sum(matrix, [index](T const& row){ assert(index < row.size()); return row[index]; });
}


// 総乗
// R: 戻り値型（桁あふれの可能性がある場合に明示的に指定する）
template <class R = void, class C = void>
auto product(C const& data)
{
	using RT = typename SameIf<R, void, typename container_traits<C>::value_type, R>::type;
	return std::accumulate(std::begin(data), std::end(data), static_cast<RT>(1), std::multiplies<RT>{});
}

// access_func: コンテナの要素にアクセスし、そのオブジェクトから値を取得する関数を指定
template <class R = void, class C = void, class Pred = void>
auto product(C const& data, Pred const& access_func)
{
	using T = typename container_traits<C>::value_type;
	using RT = typename SameIf<R, void, decltype(eval(access_func, std::declval<T>())), R>::type;

	return std::accumulate(std::begin(data), std::end(data), static_cast<RT>(1), [&](RT sum, T const& e){ return sum * access_func(e); });
}

// 行列の指定行の総乗
// R: 戻り値型（桁あふれの可能性がある場合に明示的に指定する）
template <class R = void, class CC = void>
auto product_row(CC const& matrix, uint index)
{
	assert(index < matrix.size());
	return product(matrix[index]);
}

// 行列の指定列の総乗
// R: 戻り値型（桁あふれの可能性がある場合に明示的に指定する）
template <class R = void, class CC = void>
auto product_col(CC const& matrix, uint index)
{
	using T = typename container_traits<CC>::value_type;
	return product(matrix, [index](T const& row){ assert(index < row.size()); return row[index]; });
}


// 平均
template <class C>
double average(C const& data)
{
	return static_cast<double>(sum(data)) / data.size();
}

// 分散
template <class C>
double variance(C const& data)
{
	using T = typename container_traits<C>::value_type;
	double mean = average(data);
	return std::accumulate(std::begin(data), std::end(data), 0.0, [mean](double sum, T e){ return sum + std::pow(e - mean, 2); }) / data.size();
}

// 正規化(Normalization)
// 最小値0、最大値1とし、各値が[0,1]の範囲に収まるように正規化
template <class C, typename std::enable_if<std::is_floating_point<typename container_traits<C>::value_type>::value>::type*& = enabler>
bool normalize(C& data)
{
	using T = typename container_traits<C>::value_type;
	T min = *std::begin(data);
	T max = *std::begin(data);

	for (auto e : data){
		if (e < min) min = e;
		else if (e > max) max = e;
	}
	T diff = max - min;
	for_each([min, max, diff](T& e){ e = (e - min) / (diff); }, data);
	return true;
}

template <class R = double, class C = void>
auto normalize(C const& data)
{
	using RT = typename container_traits<C>::template rebind<R>;

	auto result = sig::copy<RT>(data);
	normalize(result);
	return result;
}

// 確率密度関数の正規化
// 各値の総和が1になるように正規化
template <class C, typename std::enable_if<std::is_floating_point<typename container_traits<C>::value_type>::value>::type*& = enabler>
bool normalize_dist(C& data)
{
	double sum = sig::sum(data);

	for (auto& e : data) e /= sum;
	return true;
}

template <class R = double, class C = void>
auto normalize_dist(C const& data)
{
	using RT = typename container_traits<C>::template rebind<R>;

	auto result = sig::copy<RT>(data);
	normalize_dist(result);
	return result;
}

// 標準化(Standardization)
// 正規分布N(0, 1)になるように正規化
template <class C, typename std::enable_if<std::is_floating_point<typename container_traits<C>::value_type>::value>::type*& = enabler>
bool standardize(C& data)
{
	using T = typename container_traits<C>::value_type;
	double mean = average(data);
	double var = variance(data);

	for_each([mean, var](T& e){ e = (e - mean) / var; }, data);
	return true;
}

template <class R = double, class C = void>
auto standardize(C const& data)
{
	using RT = typename container_traits<C>::template rebind<R>;

	auto result = sig::copy<RT>(data);
	standardize(result);
	return result;
}

}
#endif