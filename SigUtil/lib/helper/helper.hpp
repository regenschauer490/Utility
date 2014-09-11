/*
Copyright(c) 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_HELPER_HPP
#define SIG_UTIL_HELPER_HPP

#include "../sigutil.hpp"
#include "../traits/type_map.hpp"
#include <sstream> 

/* 補助モジュール */

namespace sig
{
#undef max
#undef min

// 可変長 and
template <class B>
constexpr bool And(B cond){
	return cond;
}
template <class B1, class... Bs>
constexpr bool And(B1 cond, Bs... conds){
	return cond && And(conds...);
}

// 可変長 or
template <class B>
constexpr bool Or(B cond){
	return cond;
}
template <class B1, class... Bs>
constexpr bool Or(B1 cond, Bs... conds){
	return cond || Or(conds...);
}

// xor
template <class B1, class B2>
constexpr bool Xor(B1 a, B2 b){
	return (a && !b) || (!a && b); 
}

// AとBの真偽一致でtrueを返す (⇔ !xor)
template <class B1, class B2>
constexpr bool Consistency(B1 a, B2 b){
	return (a && b) || (!a && !b); 
}

// 可変長 min
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
template <class T, class... Ts>
#if !SIG_MSVC_ENV
constexpr
#endif
auto min(T v1, Ts... vs)
{
	return min(v1, min(vs...));
}

// 可変長 max
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
template <class T, class... Ts>
#if !SIG_MSVC_ENV
constexpr
#endif
auto max(T v1, Ts... vs)
{
	return max(v1, max(vs...));
}

// test if not a NAN
template <class T>
bool is_number(T x)
{
	// This looks like it should always be true, 
	// but it's false if x is a NaN.
	return (x == x);
}

// test if not a NaN and not an infinity
template <class T>
bool is_finite_number(T x)
{
	return (x <= DBL_MAX && x >= -DBL_MAX);
}


// 2変数の差の絶対値を返す
template <class T>
constexpr T abs_delta(T v1, T v2)
{
	return v1 < v2 ? v2 - v1 : v1 - v2;
}

template <class T1, class T2>
constexpr auto abs_delta(T1 v1, T2 v2)
{
	using T = typename std::common_type<T1, T2>::type;
	return abs_delta(static_cast<T>(v1), static_cast<T>(v2));
}

// 厳密な計算でない場合に使用する簡易等値比較 (浮動小数点型の誤差をある程度許容)
template <class T1, class T2, typename std::enable_if<!(impl::StringId<T1>::value || impl::StringId<T2>::value)>::type*& = enabler>
bool equal(T1 v1, T2 v2)
{
	const uint tolerant_rate = 10000;	//許容範囲の調整 (10^-16 * tolerant_rate)

	using T = typename std::common_type<T1, T2>::type;
	const T dmin = std::numeric_limits<T>::epsilon();

	return !(abs_delta(v1, v2) > tolerant_rate * dmin);
}

template <class S1, class S2, typename std::enable_if<(impl::StringId<S1>::value && impl::StringId<S2>::value)>::type*& = enabler>
bool equal(S1 v1, S2 v2)
{
	return static_cast<impl::TString<S1>>(v1) == static_cast<impl::TString<S2>>(v2);
}

// 指定範囲内の誤差を許した等値比較
template <class T1, class T2>
bool equal_tolerant(T1 v1, T2 v2, typename std::common_type<T1, T2>::type margin)
{
	return margin ? !(abs_delta(v1, v2) > margin) : equal(v1, v2);
}

// 範囲チェック (min ≦ val ≦ max)
template <class T, class U>
constexpr bool check_range(T const& val, U const& min, U const& max)
{
	return (min > val) || (val > max) ? false : true;
}

// 範囲自動修正 (min ≦ val ≦ max)
template <class T, class U>
bool modify_range(T& val, U const& min, U const& max)
{
	if (val<min){ val = min; return false; }
	if (val>max){ val = max; return false; }
	return true;
}

// V1 > V2 -> true
template <class T1, class T2>
constexpr bool greater(T1 v1, T2 v2){ return v1 > v2 ? true : false; };

// V1 < V2 -> true
template <class T1, class T2>
constexpr bool less(T1 v1, T2 v2){ return v1 < v2 ? true : false; };


/* 関数オブジェクト */

struct Identity
{
	template <class T>
	T operator()(T v) const{ return v; }
};

struct Increment
{
	template <class T>
	T operator()(T v) const{ return ++v; }
};

struct Decrement
{
	template <class T>
	T operator()(T v) const{ return --v; }
};

}
#endif
