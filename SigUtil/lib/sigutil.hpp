#ifndef SIG_UTIL_H_HPP
#define SIG_UTIL_H_HPP

#define SIG_ENABLE_BOOST 1		//boostが使用可能化
#define SIG_USE_OPTIONAL 1		//boost::optionalを使用するか(大規模データを扱う際にはオーバーヘッドが発生する)

#ifdef _WIN32
#define SIG_MSVC_ENV 1
#elif _WIN64
#define SIG_MSVC_ENV 1
#else
#define SIG_MSVC_ENV 0
#endif

#include <assert.h>
#include <string>
#include <vector>

#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <memory>
#include <random>
#include <functional>
#include <algorithm>
#include <array>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <numeric>
#include <regex>
#include <utility>

//#include "container_helper.hpp"
//#include "type_map.hpp"

#if SIG_ENABLE_BOOST

#include <boost/optional.hpp>
#include "boost/regex.hpp"
#include <boost/call_traits.hpp>
//#include <boost/format.hpp>
//#include <boost/range.hpp>
//#include <boost/range/algorithm.hpp>

#endif

namespace sig{
#undef max
#undef min

	extern void* enabler;

	/* typedef */
	using uint = std::size_t;
	using StrPtr = std::shared_ptr< std::string >;
	using C_StrPtr = std::shared_ptr< std::string const >;
	using WStrPtr = std::shared_ptr< std::wstring >;
	using C_WStrPtr = std::shared_ptr< std::wstring const >;

	using std::placeholders::_1;
	using std::placeholders::_2;


#if SIG_ENABLE_BOOST
	template <typename T>
	using maybe = boost::optional<T>;

	auto const nothing = boost::none;
#endif


//boost.call_traits の有効・無効に関係なくコードを統一的に記述するための処理
#if SIG_ENABLE_BOOST
	template <class T> using ParamType = typename boost::call_traits<T>::param_type;
#else
	template <class T>
	using ParamType = T const&;
#endif
		

//maybe(boost.optional)の有効・無効に関係なくコードを統一的に記述するための処理
#if SIG_ENABLE_BOOST && SIG_USE_OPTIONAL
	template <class T> struct Just{ typedef maybe<T> type; };
	template <class T> auto Nothing(T const& default_value)-> decltype(nothing){ return nothing; }
	template <class T> auto FromJust(maybe<T> const& sp){ return *sp; }
	template <class T> auto FromJust(maybe<T>&& sp){ return std::move(*sp); }
#else
	template <class T> struct Just{ typedef T type; };
	template <class T> T Nothing(T&& default_value){ return std::forward<T>(default_value); }
	template <class T> auto FromJust(T const& sp){ return sp; }
	template <class T> auto FromJust(T&& sp){ return std::forward<T>(sp); }
#endif

//ファイルパスの文字型の指定
#if SIG_MSVC_ENV
	using FileString = std::wstring;
	inline void FileOpenErrorPrint(FileString const& pass){ std::wcout << L"file open error: " << pass << std::endl; }
#if _MSC_VER <= 1800
#define SIG_MSVC_LT1800
#endif
#else
	using FileString = std::string;
	inline void FileOpenErrorPrint(FileString const& pass){ std::cout << "file open error: " << pass << std::endl; }
#endif

//正規表現ライブラリの指定 (gcc標準はバグが多いため避ける)
#if !SIG_MSVC_ENV && SIG_ENABLE_BOOST
using SIG_Regex = boost::regex;
using SIG_WRegex = boost::wregex;
using SIG_SMatch = boost::smatch;
using SIG_WSMatch = boost::wsmatch;
#define SIG_RegexSearch boost::regex_search
#define SIG_RegexReplace boost::regex_replace
#else
using SIG_Regex = std::regex;
using SIG_WRegex = std::wregex;
using SIG_SMatch = std::smatch;
using SIG_WSMatch = std::wsmatch;
#define SIG_RegexSearch std::regex_search
#define SIG_RegexReplace std::regex_replace
#endif


	/*
	template<uint I = 0, typename Func, typename... Ts>
	auto ForEach(std::tuple<Ts...> &, Func) ->typename std::enable_if<I == sizeof...(Ts), void>::type{}

	template<uint I = 0, typename Func, typename... Ts>
	auto ForEach(std::tuple<Ts...>& t, Func f) ->typename std::enable_if<I < sizeof...(Ts), void>::type
	{
		f(std::get<I>(t));
		ForEach<I + 1, Func, Ts...>(t, f);
	}

	template <class T>
	constexpr auto HasBegin(int) ->decltype(std::declval<T>().begin(), bool()){ return true; }

	template <class T>
	constexpr bool HasBegin(...){ return false; }
	*/
	
	/*
	template <class C>
	auto TestImpl(C c)
	{
		container_traits<C>::add_element(c, 1);
		return std::make_tuple(std::move(c));
	}
	template <class C, class... Cs>
	auto TestImpl(C c, Cs... cs)
	{
		container_traits<C>::add_element(c, 1);
		return std::tuple_cat(std::make_tuple(std::move(c)), TestImpl(cs...));
	}

	template <class T, size_t... I>
	auto Test(T const& tuple, std::index_sequence<I...>)
	{
		return TestImpl(std::vector<typename std::tuple_element<I, T>::type>{}...);
	}
	*/

	
	template <class B>
	constexpr B And(B cond){
		return cond;
	}
	template <class Bf, class... Be>
	constexpr Bf And(Bf cond, Be... conds){
		return cond && And(conds...);
	}

	template <class B>
	constexpr B Or(B cond){
		return cond;
	}
	template <class Bf, class... Be>
	constexpr Bf Or(Bf cond, Be... conds){
		return cond || Or(conds...);
	}

	//template <class T> struct NullDummy{ typedef T type; };
	//template <> struct NullDummy<void>{ typedef NullType type; };

	/*
#if _MSC_VER > 1800
	//container_traitsの拡張
	template<class C>
	struct map_associative_container_traits;

	template<template<class , class, class, class> class C, class K, class T, template<class> class O, class A>
	struct map_associative_container_traits<C<K, T, O<K>, A>>
	{
		using key_type = K;

		using value_type = T;

		using pair_type = std::pair<const K, T>;

		static void add_element(C<K, T, O<K>, A>& c, const pair_type& t)
		{
			c.insert(t);
		}

		static void concat(C<K, T, O<K>, A>& lhs, const C<K, T, O<K>, A>& rhs)
		{
			lhs.insert(rhs.begin(), rhs.end());
		}

		template<class U>
		using rebind = C<K, U, O<K>, typename A::template rebind<U>::other>;
	};

	template<class... Args>
	struct container_traits<std::map<Args...>> : public map_associative_container_traits<std::map<Args...>>
	{};

	template<class... Args>
	struct container_traits<std::multimap<Args...>> : public map_associative_container_traits<std::multimap<Args...>>
	{};
#endif
*/
	//template <class Container, class Sfinae = void> struct ContainerConstructor{ typedef Container type; };
	//template <class Container> struct ContainerConstructor<Container, typename std::enable_if<std::is_array<Container>::value>::type>{ typedef std::array<std::remove_extent<Container>, std::rank<Container>::value> type; };
	
	//template <class T, class D = void> struct HasBegin : std::true_type{};
	//template <class T> struct HasBegin<T, decltype(std::declval<T>().begin())> : std::false_type{};

	//template <typename T> constexpr auto has_reserve_method(int) -> decltype(std::declval<T>().reserve(0), bool()) { return true; }
	//template <typename T> constexpr bool has_reserve_method(...) { return false; }

	/*template <typename T> auto Reserve(T& t, size_t n) -> typename std::enable_if<has_reserve_method<T>(0), void>::type {
		std::cout << "true" << std::endl;
		t.reserve(n);
	}

	template <typename T> auto Reserve(T& t, size_t n) -> typename std::enable_if<!has_reserve_method<T>(0), void>::type {
		std::cout << "false" << std::endl;
	}*/
	
	/*template <typename T, typename std::enable_if<has_reserve_method<T>(0)>::type *& = enabler> void Reserve(T& t, size_t n){
		std::cout << "true" << std::endl;
		t.reserve(n);
	}

	template <typename T, typename std::enable_if<!has_reserve_method<T>(0)>::type *& = enabler> void Reserve(T& t, size_t n) {
		std::cout << "false" << std::endl;
	}*/

	//template <class T> auto Reserve(T& t, size_t n) ->decltype(t.reserve(n), void()){ t.reserve(n); std::cout << "true" << std::endl; }
	//template <class T> void Reserve(T& t, size_t n){ std::cout << "false" << std::endl; }


/* 便利関数 */

	//xor
	inline bool BoolXor(bool A, bool B){ return (A && !B) || (!A && B); }

	//AとBの真偽一致でtrueを返す (⇔ !xor)
	inline bool BoolConsistency(bool A, bool B){ return (A && B) || (!A && !B); }


	template <class T1, class T2>
	auto DeltaAbs(T1 v1, T2 v2) ->typename std::common_type<T1, T2>::type
	{
		return v1 < v2 ? v2 - v1 : v1 - v2;
	}

	//浮動小数点型にも使える等値比較
	template <class T1, class T2>
	bool Equal(T1 v1, T2 v2)
	{
		const auto dmin = std::numeric_limits<typename std::common_type<T1, T2>::type>::epsilon();

		return !(DeltaAbs(v1, v2) > dmin);
	}

	//指定範囲内の誤差を許した等値比較
	template <class T1, class T2>
	bool TolerantEqual(T1 v1, T2 v2, typename std::common_type<T1, T2>::type margin)
	{
		return margin ? !(DeltaAbs(v1, v2) > margin) : Equal(v1, v2);
	}

	//小数点以下の桁数取得 (ex: v=1.2300000 -> 2)
	inline uint Precision(double v)
	{
		uint keta = 0;
		double dv = v - int(v);
		
		while (true){
			if (Equal(dv, 0) || keta >= 15) break;
			v *= 10;
			dv = v - int(v);
			++keta;
		}

		return keta;
	}

	template <class T>
	auto Min(T v)
	{
		return v;
	}
	template <class T1, class T2>
	auto Min(T1 v1, T2 v2)
	{
		return std::min(v1, v2);
	}
	template <class T, class... Ts>
	auto Min(T v1, Ts... vs)
	{
		return std::min(v1, Min(vs...));
	}

	//V1 > V2 -> true
	template <uint V1, uint V2>
	struct Greater{ static constexpr bool value = V1 > V2 ? true : false; };

	//V1 < V2 -> true
	template <uint V1, uint V2>
	struct Less{ static constexpr bool value = V1 < V2 ? true : false; };


/* 修正・補正・追加・削除 */

	//範囲チェックと自動修正
	template <class T, class U>
	inline bool ModifyRange(T& val, U const& min, U const& max)
	{
		if(val<min){ val = min; return false; }
		if(val>max){ val = max; return false; }
		return true;
	}

	template <class T, class U>
	inline bool CheckRange(T const& val, U const& min, U const& max)
	{
		if(val<min){ return false; }
		if(val>max){ return false; }
		return true;
	}
}

#endif
