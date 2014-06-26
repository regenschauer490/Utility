#ifndef SIG_UTIL_HPP
#define SIG_UTIL_HPP

/*--------------------------------------- User Option --------------------------------------------------------------------*/

#define SIG_ENABLE_BOOST 1		// boostが使用可能か
#define SIG_USE_OPTIONAL 1		// boost::optionalを使用するか(大規模データを扱う際にはオーバーヘッドが発生する可能性あり)

/*------------------------------------------------------------------------------------------------------------------------*/

#define SIG_UTIL_VERSION 100

#if defined(_WIN32) || defined(_WIN64) // windows env
	#ifdef _MSC_VER
	#define SIG_MSVC_ENV 1
	#endif

#elif defined(__unix__) || defined(linux) // linux env
	#define SIG_LINUX_ENV 1

	#if defined(__clang__)
	#define SIG_CLANG_ENV 1
	#elif defined(__GNUC__)
	#define SIG_GCC_ENV 1
	#endif
#endif

// compiler version check
#ifdef SIG_MSVC_ENV
	#ifdef _DEBUG
	#define SIG_DEBUG_MODE 1
	#endif

	#if _MSC_VER < 1800
	static_assert(false, "require \"Visual C++ Compiler Nov 2013 CTP (CTP_Nov2013)\" to compile on msvc");
	#endif

#elif SIG_GCC_ENV
	#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 9)
	#define SIG_GCC_GT4_9_0 1
	#endif
	#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8)
	#define SIG_GCC_GT4_8_0 1
	#endif

#elif SIG_CLANG_ENV
	#if __clang_major__ > 3 || (__clang_major__ == 3 && __clang_minor__ >= 4)
	#define SIG_CLANG_GT_3_4 1
	#endif
	#if __clang_major__ > 3 || (__clang_major__ == 3 && __clang_minor__ >= 5)
	#define SIG_CLANG_GT_3_5 1
	#endif
#endif

#include <assert.h>
#include <string>
#include <array>
#include <vector>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <iomanip>
#include <memory>
#include <random>
#include <functional>
#include <algorithm>
#include <numeric>
#include <regex>
#include <utility>


#if SIG_ENABLE_BOOST
#include <boost/optional.hpp>
#include <boost/call_traits.hpp>
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
	template <class T> T fromJust(maybe<T> const& sp){ return *sp; }
	template <class T> T fromJust(maybe<T>&& sp){ return std::move(*sp); }
	template <class T> bool is_container_valid(T const& m){ return m; }
#else
	template <class T> struct Just{ typedef T type; };
	template <class T> T Nothing(T&& default_value){ return std::forward<T>(default_value); }
	template <class T> T fromJust(T const& sp){ return sp; }
	template <class T> T fromJust(T&& sp){ return std::forward<T>(sp); }
	template <class T> bool is_container_valid(T const& raw){ return !raw.empty(); }
#endif


// ファイルパスの文字型の指定
#if SIG_MSVC_ENV
	using FilepassString = std::wstring;
	using FilepassStringC = wchar_t const*;
	template <class T> FilepassString to_string(T v){ return std::to_wstring(v);  }
	inline void FileOpenErrorPrint(FilepassString const& pass){ std::wcout << L"file open error: " << pass << std::endl; }
	#define SIG_USE_BOOST_FILESYSTEM 0
#else
	using FilepassString = std::string;
	using FilepassStringC = char const*;
	template <class T> FilepassString to_string(T v){ return std::to_string(v);  }
	inline void FileOpenErrorPrint(FilepassString const& pass){ std::cout << "file open error: " << pass << std::endl; }
	#define SIG_USE_BOOST_FILESYSTEM 1
#endif

}

#endif
