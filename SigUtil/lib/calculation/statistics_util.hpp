/*
Copyrightrandom_unique_numbers 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_STATIC_UTIL_HPP
#define SIG_UTIL_STATIC_UTIL_HPP

#include "../iteration.hpp"


/// \file statistics_util.hpp 基本的な統計関数、正規化・標準化などの関数

namespace sig
{

/// 総和
/**
	\tparam R 戻り値型（桁あふれの可能性がある場合には明示的に指定する）

	\param data 総和を求めたい値集合（\ref sig_container ）
	
	\return 計算結果

	\code
	std::vector<int> vec{1,2,3};
	std::set<double> set{1.1, 2.2, 3.3};

	int s1 = sum(vec);		// 6
	double s2 = sum(set);	// 6.6
	\endcode
*/
template <class R = void, class C = void>
auto sum(C const& data)
	->typename impl::SameIf<R, void, typename impl::container_traits<C>::value_type, R>::type
{
	using RT = typename impl::SameIf<R, void, typename impl::container_traits<C>::value_type, R>::type;

	return std::accumulate(std::begin(data), std::end(data), static_cast<RT>(0), std::plus<RT>{});
}

/// 総和
/**
	\tparam R 戻り値型（桁あふれの可能性がある場合には明示的に指定する）

	\param data 総和を求めたい値集合（\ref sig_container ）
	\param access_func コンテナの要素にアクセスし、そのオブジェクトから値を取得する関数

	\return 計算結果

	\code
	array<std::tuple<int, int>, 2> ar{std::make_tuple(1, -1), std::make_tuple(2, -2)};

	int s = sum(ar, [](std::tuple<int, int> const& e){ return std::get<1>(e); });	// -3
	\endcode
*/
template <class R = void, class C = void, class Pred = void>
auto sum(C const& data, Pred const& access_func)
	->typename impl::SameIf<R, void, decltype(impl::eval(access_func, std::declval<typename impl::container_traits<C>::value_type>())), R>::type
{
	using T = typename impl::container_traits<C>::value_type;
	using RT = typename impl::SameIf<R, void, decltype(impl::eval(access_func, std::declval<T>())), R>::type;

	return std::accumulate(std::begin(data), std::end(data), static_cast<RT>(0), [&](RT sum, T const& e){ return sum + access_func(e); });
}

/// 行列の指定行の総和
/**
	\tparam R 戻り値型（桁あふれの可能性がある場合には明示的に指定する）

	\param matrix 行列（ランダムアクセス可能な2次元コンテナ）
	\param index 総和を求めたい行番号

	\return 計算結果

	\code
	array<array<int, 3>, 2> mat{ { 1, 2, 3 }, { 4, 5, 6 } };	// sig::array

	int sr = sum_row(mat, 1);	// 15
	\endcode
*/
template <class R = void, class CC = void>
auto sum_row(CC const& matrix, uint index)
{
	assert(index < matrix.size());
	return sum<R>(matrix[index]);
}

/// 行列の指定列の総和
/**
	\tparam R 戻り値型（桁あふれの可能性がある場合には明示的に指定する）

	\param matrix 行列（ランダムアクセス可能な2次元コンテナ）
	\param index 総和を求めたい列番号

	\return 計算結果

	\code
	array<array<int, 3>, 2> mat{ { 1, 2, 3 }, { 4, 5, 6 } };	// sig::array

	int sc = sum_col(mat, 1);	// 7
	\endcode
*/
template <class R = void, class CC = void>
auto sum_col(CC const& matrix, uint index)
{
	using T = typename impl::container_traits<CC>::value_type;

	return sum<R>(matrix, [index](T const& row){ assert(index < row.size()); return row[index]; });
}


/// 総乗
/**
	\tparam R 戻り値型（桁あふれの可能性がある場合には明示的に指定する）

	\param data 総乗を求めたい値集合（\ref sig_container ）

	\return 計算結果

	\code
	std::vector<int> vec{2,3,4};
	std::set<double> set{1.1, 2.2, 3.3};

	int p1 = product(vec);		// 24
	double p2 = prodyct(set);	// 7.986
	\endcode
*/
template <class R = void, class C = void>
auto product(C const& data)
	->typename impl::SameIf<R, void, typename impl::container_traits<C>::value_type, R>::type
{
	using RT = typename impl::SameIf<R, void, typename impl::container_traits<C>::value_type, R>::type;

	return std::accumulate(std::begin(data), std::end(data), static_cast<RT>(1), std::multiplies<RT>{});
}

/// 総乗
/**
	\tparam R 戻り値型（桁あふれの可能性がある場合には明示的に指定する）

	\param data 総乗を求めたい値集合（\ref sig_container ）
	\param access_func コンテナの要素にアクセスし、そのオブジェクトから値を取得する関数

	\return 計算結果

	\code
	array<std::tuple<int, int>, 2> ar{std::make_tuple(1, -2), std::make_tuple(3, -4)};

	int p = product(ar, [](std::tuple<int, int> const& e){ return std::get<1>(e); });	// 8
	\endcode
*/
template <class R = void, class C = void, class Pred = void>
auto product(C const& data, Pred const& access_func)
	->typename impl::SameIf<R, void, decltype(impl::eval(access_func, std::declval<typename impl::container_traits<C>::value_type>())), R>::type
{
	using T = typename impl::container_traits<C>::value_type;
	using RT = typename impl::SameIf<R, void, decltype(impl::eval(access_func, std::declval<T>())), R>::type;

	return std::accumulate(std::begin(data), std::end(data), static_cast<RT>(1), [&](RT sum, T const& e){ return sum * access_func(e); });
}

/// 行列の指定行の総乗
/**
	\tparam R 戻り値型（桁あふれの可能性がある場合には明示的に指定する）

	\param matrix 行列（ランダムアクセス可能な2次元コンテナ）
	\param index 総乗を求めたい行番号

	\return 計算結果

	\code
	array<array<int, 3>, 2> mat{ { 1, 2, 3 }, { 4, 5, 6 } };	// sig::array

	int pr = product_row(mat, 1);	// 120
	\endcode
*/
template <class R = void, class CC = void>
auto product_row(CC const& matrix, uint index)
{
	assert(index < matrix.size());
	return product<R>(matrix[index]);
}

/// 行列の指定列の総乗
/**
	\tparam R 戻り値型（桁あふれの可能性がある場合には明示的に指定する）

	\param matrix 行列（ランダムアクセス可能な2次元コンテナ）
	\param index 総乗を求めたい列番号

	\return 計算結果

	\code
	array<array<int, 3>, 2> mat{ { 1, 2, 3 }, { 4, 5, 6 } };	// sig::array

	int pr = product_col(mat, 1);	// 10
	\endcode
*/
template <class R = void, class CC = void>
auto product_col(CC const& matrix, uint index)
{
	using T = typename impl::container_traits<CC>::value_type;

	return product<R>(matrix, [index](T const& row){ assert(index < row.size()); return row[index]; });
}


/// 平均
/**
	\param data 平均を求めたい値集合（\ref sig_container ）

	\return 計算結果

	\code
	std::vector<int> vec{1,2,3,4};

	double ave = average(vec);	// 2.5
	\endcode
*/
template <class C>
double average(C const& data)
{
	return static_cast<double>(sum<double>(data)) / data.size();
}

/// 分散
/**
	\param data 分散を求めたい値集合（\ref sig_container ）

	\return 計算結果

	\code
	std::vector<int> vec{1,2,3,4};

	double var = variance(vec);	// 1.25
	\endcode
*/
template <class C>
double variance(C const& data)
{
	using T = typename impl::container_traits<C>::value_type;
	double mean = average(data);
	return std::accumulate(std::begin(data), std::end(data), 0.0, [mean](double sum, T e){ return sum + std::pow(e - mean, 2); }) / data.size();
}

/// 正規化（Normalization）
/**
	最小値0、最大値1とし、各値が[0,1]の範囲に収まるように正規化

	\param data 正規化を行いたい値集合（\ref sig_container ）．要素型は浮動小数点型であることが条件

	\return なし

	\code
	array<double, 5> ar{ -5, -2, 0, 1, 5 };	// sig::array

	normalize(ar);
	ar;				// { 0, 0.3, 0.5, 0.6, 1 }
	\endcode
*/
template <class C, typename std::enable_if<std::is_floating_point<typename impl::container_traits<C>::value_type>::value>::type*& = enabler>
void normalize(C& data)
{
	using T = typename impl::container_traits<C>::value_type;
	T min = *std::begin(data);
	T max = *std::begin(data);

	for (auto e : data){
		if (e < min) min = e;
		else if (e > max) max = e;
	}
	T diff = max - min;
	for_each([min, max, diff](T& e){ e = (e - min) / (diff); }, data);
}

/// 正規化（Normalization）
/**
	最小値0、最大値1とし、各値が[0,1]の範囲に収まるように正規化．
	normalize(C& data)ではなく明示的にこちらを呼び出す場合は、第2引数を指定

	\param data 正規化を行いたい値集合（\ref sig_container ）

	\return 正規化を行った結果

	\code
	const array<double, 5> ar1{ -5, -1.5, 0, 0.5, 5 };		// sig::array
	array<double, 5> ar2{ -5, -1.5, 0, 0.5, 5 };			// sig::array

	auto normal1 = normalize(ar1);			// { 0, 0.35, 0.5, 0.55, 1 }

	// 第2引数はダミー(ar2はconstでないため、オーバーロード解決でこの関数を呼び出すために必要)
	auto normal2 = normalize(ar2, 0);		// { 0, 0.35, 0.5, 0.55, 1 }
	\endcode
*/
template <class R = double, class C = void>
auto normalize(C const& data, int dummy = 0)
	->typename impl::container_traits<C>::template rebind<R>
{
	using RT = typename impl::container_traits<C>::template rebind<R>;

	auto result = sig::copy<RT>(data);
	normalize(result);
	return result;
}

/// 標準化(Standardization)
/**
	正規分布N(0, 1)になるように正規化

	\param data 標準化を行いたい値集合（\ref sig_container ）．要素型は浮動小数点型であることが条件

	\return なし

	\code
	array<double, 5> ar{ -3, -1, 0, 1, 3 };	// sig::array

	standardize(ar);
	ar;				// { -0.75, -0.25, 0, 0.25, 0.75 }
	\endcode
*/
template <class C, typename std::enable_if<std::is_floating_point<typename impl::container_traits<C>::value_type>::value>::type*& = enabler>
void standardize(C& data)
{
	using T = typename impl::container_traits<C>::value_type;
	double mean = average(data);
	double var = variance(data);

	for_each([mean, var](T& e){ e = (e - mean) / var; }, data);
}

/// 標準化(Standardization)
/**
	正規分布N(0, 1)になるように正規化．
	standardize(C& data)ではなく明示的にこちらを呼び出す場合は、第2引数を指定
	
	\param data 標準化を行いたい値集合（\ref sig_container ）

	\return 標準化を行った結果

	\code
	const array<int, 5> ar1{  -3, -1, 0, 1, 3 };		// sig::array
	array<double, 5> ar2{ 0, 1, 2, 3, 4 };				// sig::array

	auto stnd1 = standardize(ar1);			// { -0.75, -0.25, 0, 0.25, 0.75}

	// 第2引数はダミー(ar2はconstでないため、オーバーロード解決でこの関数を呼び出すために必要)
	auto stnd2 = standardize(ar2, 0);		// { -1, -0.5, 0, 0.5, 1 }
	\endcode
*/ 
template <class R = double, class C = void>
auto standardize(C const& data, int dummy = 0)
	->typename impl::container_traits<C>::template rebind<R>
{
	using RT = typename impl::container_traits<C>::template rebind<R>;

	auto result = sig::copy<RT>(data);
	standardize(result);
	return result;
}

/// 確率分布の正規化
/**
	各値の総和が1になるよう[0,1]の範囲に正規化

	\param data 正規化を行いたい値集合（\ref sig_container ）．要素型は浮動小数点型であることが条件

	\return なし

	\code
	array<double, 5> ar{ 1, 2, 4, 2, 1 };

	normalize_dist(ar);
	ar;			// { 0.1, 0.2, 0.4, 0.2, 0.1 }
	\endcode
*/
template <class C, typename std::enable_if<std::is_floating_point<typename impl::container_traits<C>::value_type>::value>::type*& = enabler>
void normalize_dist(C& data)
{
	double sum = sig::sum(data);

	for (auto& e : data) e /= sum;
}

/// 確率分布の正規化
/**
	各値の総和が1になるよう[0,1]の範囲に正規化．
	normalize_dist(C& data)ではなく明示的にこちらを呼び出す場合は、第2引数を指定

	\param data 正規化を行いたい値集合（\ref sig_container ）

	\return 正規化を行った結果

	\code
	const array<int, 5> ar1{  1, 2, 4, 2, 1 };			// sig::array
	array<double, 5> ar2{ 0.2, 0.5, 1.4, 0.3, 0.8 };	// sig::array

	auto nd1 = normalize_dist(ar1);			// { 0.1, 0.2, 0.4, 0.2, 0.1 }

	// 第2引数はダミー(ar2はconstでないため、オーバーロード解決でこの関数を呼び出すために必要)
	auto nd2 = normalize_dist(ar2, 0);		// { 0.0625, 0.15625, 0.4375, 0.09375, 0.25 }
	\endcode
*/
template <class R = double, class C = void>
auto normalize_dist(C const& data, int dummy = 0)
->typename impl::container_traits<C>::template rebind<R>
{
	using RT = typename impl::container_traits<C>::template rebind<R>;

	auto result = sig::copy<RT>(data);
	normalize_dist(result);
	return result;
}

}

#endif