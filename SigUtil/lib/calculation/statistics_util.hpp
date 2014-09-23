/*
Copyrightrandom_unique_numbers 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_STATIC_UTIL_HPP
#define SIG_UTIL_STATIC_UTIL_HPP

#include "../iteration.hpp"


/* 基本的な統計関数、正規化・標準化などの関数 */

namespace sig
{

// 総和
// R: 戻り値型（桁あふれの可能性がある場合には明示的に指定する）
template <class R = void, class C = void>
auto sum(C const& data)
	->typename impl::SameIf<R, void, typename container_traits<C>::value_type, R>::type
{
	using RT = typename impl::SameIf<R, void, typename container_traits<C>::value_type, R>::type;

	return std::accumulate(std::begin(data), std::end(data), static_cast<RT>(0), std::plus<RT>{});
}

// access_func: コンテナの要素にアクセスし、そのオブジェクトから値を取得する関数を指定
template <class R = void, class C = void, class Pred = void>
auto sum(C const& data, Pred const& access_func)
	->typename impl::SameIf<R, void, decltype(eval(access_func, std::declval<typename container_traits<C>::value_type>())), R>::type
{
	using T = typename container_traits<C>::value_type;
	using RT = typename impl::SameIf<R, void, decltype(eval(access_func, std::declval<T>())), R>::type;

	return std::accumulate(std::begin(data), std::end(data), static_cast<RT>(0), [&](RT sum, T const& e){ return sum + access_func(e); });
}

// 行列の指定行の総和
// R: 戻り値型（桁あふれの可能性がある場合には明示的に指定する）
template <class R = void, class CC = void>
auto sum_row(CC const& matrix, uint index)
{
	assert(index < matrix.size());
	return sum<R>(matrix[index]);
}

// 行列の指定列の総和
// R: 戻り値型（桁あふれの可能性がある場合には明示的に指定する）
template <class R = void, class CC = void>
auto sum_col(CC const& matrix, uint index)
{
	using T = typename container_traits<CC>::value_type;

	return sum<R>(matrix, [index](T const& row){ assert(index < row.size()); return row[index]; });
}


// 総乗
// R: 戻り値型（桁あふれの可能性がある場合には明示的に指定する）
template <class R = void, class C = void>
auto product(C const& data)
	->typename impl::SameIf<R, void, typename container_traits<C>::value_type, R>::type
{
	using RT = typename impl::SameIf<R, void, typename container_traits<C>::value_type, R>::type;

	return std::accumulate(std::begin(data), std::end(data), static_cast<RT>(1), std::multiplies<RT>{});
}

// access_func: コンテナの要素にアクセスし、そのオブジェクトから値を取得する関数を指定
template <class R = void, class C = void, class Pred = void>
auto product(C const& data, Pred const& access_func)
	->typename impl::SameIf<R, void, decltype(eval(access_func, std::declval<typename container_traits<C>::value_type>())), R>::type
{
	using T = typename container_traits<C>::value_type;
	using RT = typename impl::SameIf<R, void, decltype(eval(access_func, std::declval<T>())), R>::type;

	return std::accumulate(std::begin(data), std::end(data), static_cast<RT>(1), [&](RT sum, T const& e){ return sum * access_func(e); });
}

// 行列の指定行の総乗
// R: 戻り値型（桁あふれの可能性がある場合には明示的に指定する）
template <class R = void, class CC = void>
auto product_row(CC const& matrix, uint index)
{
	assert(index < matrix.size());
	return product<R>(matrix[index]);
}

// 行列の指定列の総乗
// R: 戻り値型（桁あふれの可能性がある場合には明示的に指定する）
template <class R = void, class CC = void>
auto product_col(CC const& matrix, uint index)
{
	using T = typename container_traits<CC>::value_type;

	return product<R>(matrix, [index](T const& row){ assert(index < row.size()); return row[index]; });
}


// 平均
template <class I = void, class C = void>
double average(C const& data)
{
	return static_cast<double>(sum<I>(data)) / data.size();
}

// 分散
template <class I = void, class C = void>
double variance(C const& data)
{
	using T = typename container_traits<C>::value_type;
	double mean = average<I>(data);
	return std::accumulate(std::begin(data), std::end(data), 0.0, [mean](double sum, T e){ return sum + std::pow(e - mean, 2); }) / data.size();
}

// 正規化(Normalization)
// 最小値0、最大値1とし、各値が[0,1]の範囲に収まるように正規化
template <class C, typename std::enable_if<std::is_floating_point<typename container_traits<C>::value_type>::value>::type*& = enabler>
void normalize(C& data)
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
}

// 元のコンテナに変更は行わず、結果を返す
// normalize(C& data)ではなく明示的にこちらを呼び出す場合は、第2引数を指定
template <class R = double, class C = void>
auto normalize(C const& data, int dummy = 0)
	->typename container_traits<C>::template rebind<R>
{
	using RT = typename container_traits<C>::template rebind<R>;

	auto result = sig::copy<RT>(data);
	normalize(result);
	return result;
}

// 確率密度関数の正規化
// 各値の総和が1になるように正規化
template <class C, typename std::enable_if<std::is_floating_point<typename container_traits<C>::value_type>::value>::type*& = enabler>
void normalize_dist(C& data)
{
	double sum = sig::sum(data);

	for (auto& e : data) e /= sum;
}

// 元のコンテナに変更は行わず、結果を返す
// normalize_dist(C& data)ではなく明示的にこちらを呼び出す場合は、第2引数を指定
template <class R = double, class C = void>
auto normalize_dist(C const& data, int dummy = 0)
	->typename container_traits<C>::template rebind<R>
{
	using RT = typename container_traits<C>::template rebind<R>;

	auto result = sig::copy<RT>(data);
	normalize_dist(result);
	return result;
}

// 標準化(Standardization)
// 正規分布N(0, 1)になるように正規化
template <class C, typename std::enable_if<std::is_floating_point<typename container_traits<C>::value_type>::value>::type*& = enabler>
void standardize(C& data)
{
	using T = typename container_traits<C>::value_type;
	double mean = average(data);
	double var = variance(data);

	for_each([mean, var](T& e){ e = (e - mean) / var; }, data);
}

// 元のコンテナに変更は行わず、結果を返す
// standardize(C& data)ではなく明示的にこちらを呼び出す場合は、第2引数を指定
template <class R = double, class C = void>
auto standardize(C const& data, int dummy = 0)
	->typename container_traits<C>::template rebind<R>
{
	using RT = typename container_traits<C>::template rebind<R>;

	auto result = sig::copy<RT>(data);
	standardize(result);
	return result;
}

}

#endif