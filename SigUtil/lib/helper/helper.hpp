/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_HELPER_HPP
#define SIG_UTIL_HELPER_HPP

#include "../sigutil.hpp"
#include "../helper/type_map.hpp"
#include <sstream> 

/// \file helper.hpp 補助モジュール群

namespace sig
{

template <class B>
constexpr bool And(B cond){
	return cond;
}
/// 可変長 and
/**
	\param cond,conds bool変数 または operator boolが定義されたオブジェクト

	\return 全引数に対してandを取った結果
*/
template <class B1, class... Bs>
constexpr bool And(B1 cond, Bs... conds){
	return cond && And(conds...);
}


template <class B>
constexpr bool Or(B cond){
	return cond;
}
/// 可変長 or
/**
	\param cond,conds bool変数 または operator boolが定義されたオブジェクト

	\return 全引数に対してorを取った結果
*/
template <class B1, class... Bs>
constexpr bool Or(B1 cond, Bs... conds){
	return cond || Or(conds...);
}


/// xor
template <class B1, class B2>
constexpr bool Xor(B1 a, B2 b){
	return (a && !b) || (!a && b); 
}

/// AとBの真偽一致でtrueを返す (⇔ !xor)
template <class B1, class B2>
constexpr bool Consistency(B1 a, B2 b){
	return (a && b) || (!a && !b); 
}


template <class T>
#if !SIG_MSVC_ENV
constexpr
#endif
auto min(T v)
{
	return v;
}

template <class T1, class T2>
#if !SIG_MSVC_ENV
constexpr
#endif
auto min(T1 v1, T2 v2)
{
	return v1 < v2 ? v1 : v2;
}

/// 可変長 min
/**
	\param v,vs 大小比較可能な変数（数値を表すPOD型 または operator < が定義されたオブジェクト）

	\return 最小の要素
*/
template <class T, class... Ts>
#if !SIG_MSVC_ENV
constexpr
#endif
auto min(T v, Ts... vs)
{
	return min(v, min(vs...));
}


template <class T>
#if !SIG_MSVC_ENV
constexpr 
#endif
auto max(T v)
{
	return v;
}

template <class T1, class T2>
#if !SIG_MSVC_ENV
constexpr 
#endif
auto max(T1 v1, T2 v2)
{
	return v1 < v2 ? v2 : v1;
}

/// 可変長 max
/**
	\param v,vs 大小比較可能な変数（数値を表すPOD型 または operator < が定義されたオブジェクト）

	\return 最大の要素
*/
template <class T, class... Ts>
#if !SIG_MSVC_ENV
constexpr
#endif
auto max(T v, Ts... vs)
{
	return max(v, max(vs...));
}

/// 値が非数(NaN)でないかを確認
template <class T>
bool is_number(T x)
{
	// false if x is a NaN.
	return (x == x);
}

/// 値が非数(NaN)でなく、かつ無限大(Inf)でないかを確認
template <class T>
bool is_finite_number(T x)
{
	return (x <= DBL_MAX && x >= -DBL_MAX);
}


/// 2変数の差の絶対値を返す
template <class T>
constexpr T abs_delta(T v1, T v2)
{
	return v1 < v2 ? v2 - v1 : v1 - v2;
}

/// 2変数の差の絶対値を返す
template <class T1, class T2>
constexpr auto abs_delta(T1 v1, T2 v2)
{
	using T = typename std::common_type<T1, T2>::type;
	return abs_delta(static_cast<T>(v1), static_cast<T>(v2));
}

/// 数値の簡易等値比較（厳密な計算でない場合の使用を想定）
/**
	浮動小数点型の誤差をある程度許容．
	デフォルトでは 10^(-12) 未満の誤差を無視
*/
template <class T1, class T2, typename std::enable_if<!(impl::StringId<T1>::value || impl::StringId<T2>::value)>::type*& = enabler>
bool equal(T1 v1, T2 v2)
{
	const uint tolerant_rate = 10000;	//許容範囲の調整 (10^-16 * tolerant_rate)

	using T = typename std::common_type<T1, T2>::type;
	const T dmin = std::numeric_limits<T>::epsilon();

	return !(abs_delta(v1, v2) > tolerant_rate * dmin);
}

/// 文字列の等値比較
template <class S1, class S2, typename std::enable_if<(impl::StringId<S1>::value && impl::StringId<S2>::value)>::type*& = enabler>
bool equal(S1 v1, S2 v2)
{
	return static_cast<impl::TString<S1>>(v1) == static_cast<impl::TString<S2>>(v2);
}

/// 指定範囲内の誤差を許した等値比較
/**
	\param v1,v2 等値比較したい値
	\margin 許容する誤差

	\return 誤差を許容した等値比較の結果
*/
template <class T1, class T2>
bool equal_tolerant(T1 v1, T2 v2, typename std::common_type<T1, T2>::type margin)
{
	return margin ? !(abs_delta(v1, v2) > margin) : equal(v1, v2);
}

/// 値の範囲チェック
/**
	min ≦ val ≦ max であるか確認

	\param val 確認したい値
	\param min 範囲の下限
	\param max 範囲の上限

	\return valが範囲内に収まっているか
*/
template <class T, class U>
constexpr bool check_range(T const& val, U const& min, U const& max)
{
	return (min > val) || (val > max) ? false : true;
}

/// 値を範囲内に自動修正
/**
	min ≦ val ≦ max であるか確認し、範囲を超えている場合は最も近い範囲内の値（min または max）に修正

	\param val 確認したい値
	\param min 範囲の下限
	\param max 範囲の上限

	\return valが修正されたか
*/
template <class T, class U>
bool modify_range(T& val, U const& min, U const& max)
{
	if (val<min){ val = min; return false; }
	if (val>max){ val = max; return false; }
	return true;
}


/**
	v1 > v2 であるか確認
*/
template <class T1, class T2>
constexpr bool greater(T1 v1, T2 v2){ return v1 > v2 ? true : false; };

/**
	v1 < v2 であるか確認
*/
template <class T1, class T2>
constexpr bool less(T1 v1, T2 v2){ return v1 < v2 ? true : false; };


/// 引数の変数をそのまま返す関数オブジェクト
struct Identity
{
	template <class T>
	T operator()(T v) const{ return v; }
};

/// 引数の値をインクリメントした値を返す関数オブジェクト
struct Increment
{
	template <class T>
	T operator()(T v) const{ return ++v; }
};

/// 引数の値をデクリメントした値を返す関数オブジェクト
struct Decrement
{
	template <class T>
	T operator()(T v) const{ return --v; }
};

}
#endif
