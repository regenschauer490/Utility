#ifndef SIG_UTIL_HPP
#define SIG_UTIL_HPP

#define SIG_ENABLE_BOOST 1		//boostが使用可能化
#define SIG_USE_OPTIONAL 1		//boost::optionalを使用するか(大規模データを扱う際にはオーバーヘッドが発生する可能性あり)

#ifdef _WIN32
#define SIG_MSVC_ENV 1
#elif _WIN64
#define SIG_MSVC_ENV 1
#else
#ifdef __GNUC__
#define SIG_GCC_ENV 1
#endif
#endif

#ifdef SIG_MSVC_ENV
	#ifdef _DEBUG
	#define SIG_DEBUG_MODE 1
	#endif
	#if _MSC_VER < 1800
	#define SIG_MSVC_LT1800 1
	static_assert(false, "require \"Visual C++ Compiler Nov 2013 CTP (CTP_Nov2013)\" to compile on msvc");
	#endif
#else
	#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 8)
	#define SIG_GCC_GT4_9_0 1
	#endif
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


// boost.call_traits の有効・無効に関係なくコードを統一的に記述するための処理
#if SIG_ENABLE_BOOST
	template <class T> using ParamType = typename boost::call_traits<T>::param_type;
#else
	template <class T>
	using ParamType = typename std::conditional<std::is_class<T>::value, T const&, T>::type;
#endif
		

// maybe(boost.optional)の有効・無効に関係なくコードを統一的に記述するための処理
#if SIG_ENABLE_BOOST && SIG_USE_OPTIONAL
	template <class T> struct Just{ typedef maybe<T> type; };
	template <class T> auto Nothing(T const& default_value)-> decltype(nothing){ return nothing; }
	template <class T> auto fromJust(maybe<T> const& sp){ return *sp; }
	template <class T> auto fromJust(maybe<T>&& sp){ return std::move(*sp); }
	template <class T> bool is_container_valid(T const& m){ return m; }
#else
	template <class T> struct Just{ typedef T type; };
	template <class T> T Nothing(T&& default_value){ return std::forward<T>(default_value); }
	template <class T> auto fromJust(T const& sp){ return sp; }
	template <class T> auto fromJust(T&& sp){ return std::forward<T>(sp); }
	template <class T> bool is_container_valid(T const& raw){ return !raw.empty(); }
#endif


// ファイルパスの文字型の指定
#if SIG_MSVC_ENV
	using FilepassString = std::wstring;
	inline void FileOpenErrorPrint(FilepassString const& pass){ std::wcout << L"file open error: " << pass << std::endl; }
#else
	using FilepassString = std::string;
	inline void FileOpenErrorPrint(FilepassString const& pass){ std::cout << "file open error: " << pass << std::endl; }
#endif


// 正規表現ライブラリの指定 (gcc標準はバグが多いため避ける)
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

}

#endif
