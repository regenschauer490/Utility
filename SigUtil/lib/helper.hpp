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
constexpr B and(B cond){
	return cond;
}
template <class Bf, class... Be>
constexpr Bf and(Bf cond, Be... conds){
	return cond && and(conds...);
}

// 可変長 or
template <class B>
constexpr B or(B cond){
	return cond;
}
template <class Bf, class... Be>
constexpr Bf or(Bf cond, Be... conds){
	return cond || or(conds...);
}

// 可変長 min
template <class T>
auto min(T v)
{
	return v;
}
template <class T1, class T2>
auto min(T1 v1, T2 v2)
{
	return std::min(v1, v2);
}
template <class T, class... Ts>
auto min(T v1, Ts... vs)
{
	return std::min(v1, min(vs...));
}

// 可変長 max
template <class T>
auto max(T v)
{
	return v;
}
template <class T1, class T2>
auto max(T1 v1, T2 v2)
{
	return std::max(v1, v2);
}
template <class T, class... Ts>
auto max(T v1, Ts... vs)
{
	return std::max(v1, max(vs...));
}

// V1 > V2 -> true
template <class T1, class T2>
constexpr bool greater(T1 v1, T2 v2){ return v1 > v2 ? true : false; };

// V1 < V2 -> true
template <class T1, class T2>
constexpr bool less(T1 v1, T2 v2){ return v1 < v2 ? true : false; };


/* 実行時用 */

// xor
template <class B1, class B2>
inline bool xor(B1 a, B2 b){ return (a && !b) || (!a && b); }

// AとBの真偽一致でtrueを返す (⇔ !xor)
template <class B1, class B2>
inline bool consistency(B1 a, B2 b){ return (a && b) || (!a && !b); }


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