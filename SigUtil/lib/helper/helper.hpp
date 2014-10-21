/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_HELPER_HPP
#define SIG_UTIL_HELPER_HPP

#include "../sigutil.hpp"
#include "../helper/type_convert.hpp"
#include <sstream> 

/// \file helper.hpp 補助モジュール群

namespace sig
{

template <class B>
#if !(SIG_MSVC_VER == 140)
constexpr
#endif
bool And(B&& cond){
	return static_cast<bool>(cond);
}

/// 可変長 and
/**
	\param cond,conds bool変数 または operator boolが定義されたオブジェクト

	\return 全引数に対してandを取った結果

	\code
	static_assert(And(true, true, true, true), "");
	static_assert(!And(false, true, false, true, false), "");
	\endcode
*/
template <class B1, class... Bs>
#if !(SIG_MSVC_VER == 140)
constexpr
#endif
bool And(B1&& cond, Bs&&... conds){
	return And(std::forward<Bs>(conds)...) && cond;
}


template <class B>
#if !(SIG_MSVC_VER == 140)
constexpr
#endif
bool Or(B&& cond){
	return static_cast<bool>(cond);
}
/// 可変長 or
/**
	\param cond,conds bool変数 または operator boolが定義されたオブジェクト

	\return 全引数に対してorを取った結果

	\code
	static_assert(!Or(false, false, false), "");
	static_assert(Or(false, true, false, true, false), "");
	\endcode
*/
template <class B1, class... Bs>
#if !(SIG_MSVC_VER == 140)
constexpr
#endif
bool Or(B1&& cond, Bs&&... conds){
	return Or(conds...) || cond;
}


/// xor
/**
	\param cond,conds bool変数 または operator boolが定義されたオブジェクト

	\return xorの結果

	\code
	static_assert(!Xor(true, true), "");
	static_assert(Xor(true, false), "");
	static_assert(Xor(false, true), "");
	static_assert(!Xor(false, false), "");
	\endcode
*/
template <class B1, class B2>
#if !(SIG_MSVC_VER == 140)
constexpr
#endif
bool Xor(B1&& a, B2&& b){
	return (a && !b) || (!a && b); 
}

/// AとBの真偽一致でtrueを返す (⇔ !xor)
/**
	\param cond,conds bool変数 または operator boolが定義されたオブジェクト

	\return !xorの結果

	\code
	static_assert(Consistency(true, true), "");
	static_assert(!Consistency(true, false), "");
	static_assert(!Consistency(false, true), "");
	static_assert(Consistency(false, false), "");
	\endcode
*/
template <class B1, class B2>
#if !(SIG_MSVC_VER == 140)
constexpr
#endif
bool Consistency(B1&& a, B2&& b){
	return (a && b) || (!a && !b); 
}


template <class T>
#if !(SIG_MSVC_VER <= 120)
constexpr
#endif
auto min(T v) ->T
{
	return v;
}

template <class T1, class T2>
#if !(SIG_MSVC_VER <= 140)
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

	\code
	static_assert( min(1, -1, 0, 3, 2) == -1, "");
	\endcode
*/
template <class T, class... Ts>
#if !(SIG_MSVC_VER <= 140)
constexpr
#endif
auto min(T v, Ts... vs)
{
	return min(v, min(vs...));
}


template <class T>
#if !(SIG_MSVC_VER <= 120)
constexpr 
#endif
auto max(T v) ->T
{
	return v;
}

template <class T1, class T2>
#if !(SIG_MSVC_VER <= 140)
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

	\code
	static_assert( max(1, -1, 0, 3, 2) == 3, "");
	\endcode
*/
template <class T, class... Ts>
#if !(SIG_MSVC_VER <= 140)
constexpr
#endif
auto max(T v, Ts... vs)
{
	return max(v, max(vs...));
}


/// 値が非数(NaN)でないかを確認
/**
	\code
	double zero = 0;
	double nan = std::numeric_limits<double>::quiet_NaN();
	
	assert(is_number(0));
	assert(!is_number(nan));
	assert(!is_number(zero/zero));
	assert(!is_number(std::sqrt(-1)));
	\endcode
*/
template <class T>
constexpr bool is_number(T x)
{
	// false if x is a NaN.
	return (x == x);
}

/// 値が非数(NaN)でなく、かつ無限大(Inf)でないかを確認
/**
	\code
	double inf = std::numeric_limits<double>::infinity();
	double inf2 = product(sig::replicate<double>(1000, 10));	// 10^1000

	assert(is_finite_number(0));
	assert(!is_finite_number(inf));
	assert(!is_finite_number(inf2));
	\endcode
*/
template <class T>
constexpr bool is_finite_number(T x)
{
	return (x <= DBL_MAX && x >= -DBL_MAX);
}


template <class T>
constexpr T abs_delta(T v1, T v2)
{
	return v1 < v2 ? v2 - v1 : v1 - v2;
}

/// 2変数の差の絶対値を返す
/**
	\pre operator< , operator-

	\code
	static_assert( abs_delta(1, 3) == 2, "");
	static_assert( abs_delta(-2, -1) == 1, "");
	static_assert( abs_delta(3.0, 1.5) == 1.5, "");
	static_assert( abs_delta(3, 1.5) == 1.5, "");
	\endcode
*/
template <class T1, class T2>
#if !(SIG_MSVC_VER == 140)
constexpr
#endif
auto abs_delta(T1 v1, T2 v2) ->decltype(v1 < v2 ? v2 - v1 : v1 - v2)
{
	return v1 < v2 ? v2 - v1 : v1 - v2;
}


/// 数値の簡易等値比較（厳密な計算でない場合の使用を想定）
/**
	浮動小数点型の誤差をある程度許容．
	デフォルトでは 10^(-12) 未満の誤差を無視

	\code
	assert( equal(1, 1));
	assert( equal(1, 1u));
	assert( equal(1u, 1));
	assert( equal(1u, 1u));
	assert( equal(1, 1.0));
	assert( equal(1.0, 1));
	assert( equal('a', 'a'));

	int ct = 0;
	for (double f = 0.0; !equal(f, 1); f += 0.01, ++ct){
		if (ct >= 100) assert(false);
	}
	\endcode
*/
template <class T1, class T2, typename std::enable_if<!(impl::StringId<T1>::value || impl::StringId<T2>::value)>::type*& = enabler>
#if !(SIG_MSVC_VER <= 140)
constexpr
#endif
bool equal(T1&& v1, T2&& v2)
{
	constexpr uint tolerant_rate = 10000;	//許容範囲の調整 (10^-16 * tolerant_rate)

	using T = typename std::common_type<T1, T2>::type;
	const T dmin = std::numeric_limits<T>::epsilon();

	return !(abs_delta(std::forward<T1>(v1), std::forward<T2>(v2)) > tolerant_rate * dmin);
}


/// 文字列の等値比較
/**
	\code
	assert( equal("tes", "tes"));
	assert( equal(L"tes", L"tes"));
	assert( equal("tes", std::string("tes")));
	assert( equal(std::string("tes"), "tes"));
	assert( equal(L"tes", std::wstring(L"tes")));
	assert( equal(std::wstring(L"tes"), L"tes"));
	\endcode
*/
template <class S1, class S2, typename std::enable_if<(impl::is_string<S1>::value && impl::is_string<S2>::value)>::type*& = enabler>
bool equal2(S1&& v1, S2&& v2)
{
	return static_cast<impl::string_t<S1>>(std::forward<S1>(v1)) == static_cast<impl::string_t<S2>>(std::forward<S2>(v2));
}


/// 指定範囲内の誤差を許した等値比較
/**
	\param v1,v2 等値比較したい値
	\param margin 許容する誤差

	\return 誤差を許容した等値比較の結果

	\code
	assert( equal_tolerant(0.001, 0.002, 0.001));
	assert(!equal_tolerant(0.001, 0.003, 0.001));
	\endcode
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

	\code
	int cr = 5;
	assert( check_range(cr, 0, 10));
	assert(!check_range(cr, 0, 3));
	assert(cr == 5);
	\endcode
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

	\code
	int mr = 5;
	assert(sig::modify_range(mr, 0, 10));
	assert(mr == 5);
	assert(!sig::modify_range(mr, 0, 3));
	assert(mr == 3);
	\endcode
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

	\code
	static_assert( greater(1, 0), "");
	\endcode
*/
template <class T1, class T2>
constexpr bool greater(T1 v1, T2 v2){ return v1 > v2 ? true : false; };

/**
	v1 < v2 であるか確認

	\code
	static_assert( less(0, 1), "");
	\endcode
*/
template <class T1, class T2>
constexpr bool less(T1 v1, T2 v2){ return v1 < v2 ? true : false; };


/// 引数の変数をそのまま返す関数オブジェクト
struct Identity
{
	template <class T>
#if !(SIG_MSVC_VER == 140)
	constexpr
#endif
	T operator()(T v) const{ return v; }
};

/// 引数の値をインクリメントした値を返す関数オブジェクト
struct Increment
{
	template <class T>
#if !(SIG_MSVC_VER == 140)
	constexpr
#endif
	T operator()(T v) const{ return ++v; }
};

/// 引数の値をデクリメントした値を返す関数オブジェクト
struct Decrement
{
	template <class T>
#if !(SIG_MSVC_VER == 140)
	constexpr
#endif
	T operator()(T v) const{ return --v; }
};

}
#endif
