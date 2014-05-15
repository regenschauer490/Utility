/*
Copyright(c) 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_HELPER_HPP
#define SIG_UTIL_HELPER_HPP

#include "sigutil.hpp"
#include <sstream> 

/* 補助モジュール */

namespace sig
{
#undef max
#undef min

/* コンパイル時用 */

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
auto Min(T v)
{
	return v;
}
template <class T, class... Ts>
#if !SIG_MSVC_ENV
constexpr
#endif
auto Min(T v1, Ts... vs)
{
	return std::min(v1, Min(vs...));
}

// 可変長 max
template <class T>
#if !SIG_MSVC_ENV
constexpr 
#endif
auto Max(T v)
{
	return v;
}
template <class T, class... Ts>
#if !SIG_MSVC_ENV
constexpr
#endif
auto Max(T v1, Ts... vs)
{
	return std::max(v1, Max(vs...));
}

// V1 > V2 -> true
template <class T1, class T2>
constexpr bool Greater(T1 v1, T2 v2){ return v1 > v2 ? true : false; };

// V1 < V2 -> true
template <class T1, class T2>
constexpr bool Less(T1 v1, T2 v2){ return v1 < v2 ? true : false; };


/* 実行時用 */

//2変数の差の絶対値を返す
template <class T1, class T2>
auto abs_delta(T1 v1, T2 v2) ->typename std::common_type<T1, T2>::type
{
	return v1 < v2 ? v2 - v1 : v1 - v2;
}

//浮動小数点型にも使える等値比較
template <class T1, class T2>
bool equal(T1 v1, T2 v2)
{
	const auto dmin = std::numeric_limits<typename std::common_type<T1, T2>::type>::epsilon();

	return !(abs_delta(v1, v2) > dmin);
}

//指定範囲内の誤差を許した等値比較
template <class T1, class T2>
bool equal_tolerant(T1 v1, T2 v2, typename std::common_type<T1, T2>::type margin)
{
	return margin ? !(abs_delta(v1, v2) > margin) : equal(v1, v2);
}

//範囲チェック (min ≦ val ≦ max)
template <class T, class U>
inline bool check_range(T const& val, U const& min, U const& max)
{
	if (val<min){ return false; }
	if (val>max){ return false; }
	return true;
}

//範囲自動修正 (min ≦ val ≦ max)
template <class T, class U>
inline bool modify_range(T& val, U const& min, U const& max)
{
	if (val<min){ val = min; return false; }
	if (val>max){ val = max; return false; }
	return true;
}

//別のコンテナに要素をコピーする用
template <class RC, class C>
auto copy(C const& src) ->RC
{
	RC dest;
	for (auto const& e : src) container_traits<RC>::add_element(dest, e);
	return dest;
}

}
#endif