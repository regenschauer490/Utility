/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIGUTIL_HPP
#define SIGUTIL_HPP

/*--------------------------------------- User Option --------------------------------------------------------------------*/

#define SIG_ENABLE_BOOST 1		// boostが使用可能か
#define SIG_USE_OPTIONAL 1		// boost::optionalを使用するか(大規模データを扱う際にはオーバーヘッドが影響する可能性あり)
#define SIG_PRINT_ERROR_MESSAGE 1	// エラー発生時にメッセージをコンソールに出力するか(ex:ファイル入出力)

/*------------------------------------------------------------------------------------------------------------------------*/

#define SIGUTIL_VERSION 095

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
#if SIG_MSVC_ENV
	#ifdef _DEBUG
	#define SIG_DEBUG_MODE 1
	#endif

	#if _MSC_VER < 1800
	static_assert(false, "require \"Visual C++ Compiler Nov 2013 CTP (CTP_Nov2013)\" to compile on msvc");
	#elif _MSC_VER == 1800
	#define SIG_MSVC_VER 120
	#elif _MSC_VER == 1900
	#define SIG_MSVC_VER 140
	#endif

#elif SIG_GCC_ENV
	#if __GNUC__ > 4 || (__GNUC__ == 5 && __GNUC_MINOR__ >= 0)
	#define SIG_GCC_GT5_0_0 1
	#elif (__GNUC__ == 4 && __GNUC_MINOR__ >= 9)
	#define SIG_GCC_GT4_9_0 1
	#elif (__GNUC__ == 4 && __GNUC_MINOR__ >= 8)
	#define SIG_GCC_GT4_8_0 1
	#endif

#elif SIG_CLANG_ENV
	#if __clang_major__ > 3 || (__clang_major__ == 3 && __clang_minor__ >= 4)
	#define SIG_CLANG_GT_3_4 1
	#elif __clang_major__ > 3 || (__clang_major__ == 3 && __clang_minor__ >= 5)
	#define SIG_CLANG_GT_3_5 1
	#endif
#endif


#define SIG_ENABLE_CONSTEXPR (SIG_MSVC_ENV && SIG_MSVC_VER > 140) || (SIG_GCC_ENV) || (SIG_CLANG_ENV)

#define SIG_ENABLE_MOVEITERATOR (SIG_MSVC_ENV) || (SIG_GCC_ENV && SIG_GCC_GT5_0_0) || (SIG_CLANG_ENV)

#define SIG_ENABLE_FOLDR SIG_MSVC_ENV || SIG_GCC_GT5_0_0 || SIG_CLANG_GT_3_5

#define SIG_ENABLE_TUPLE_ZIP (SIG_MSVC_VER > 140) || SIG_GCC_GT4_9_0 || SIG_CLANG_GT_3_5


#include <assert.h>
#include <string>
#include <memory>
#include <functional>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <utility>

#if SIG_MSVC_ENV || !SIG_ENABLE_BOOST
#include <regex>
#endif

#if SIG_ENABLE_BOOST
#include <boost/pool/pool_alloc.hpp>
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
	template <class T>
	using fragment_allocator = boost::fast_pool_allocator<T>;
#else
	template <class T>
	using fragment_allocator = std::allocator<T>;
#endif


// boost.call_traits の有効・無効に関係なくコードを統一的に記述するための処理
#if SIG_ENABLE_BOOST
	template <class T> using ParamType = typename boost::call_traits<T>::param_type;
#else
	template <class T>
	using ParamType = typename std::conditional<std::is_class<T>::value, T const&, T>::type;
#endif
		

// ファイルパスの文字型の指定
#if SIG_MSVC_ENV
	using FilepassString = std::wstring;
	using FilepassStringC = wchar_t const*;
	template <class T> FilepassString to_fpstring(T v){ return std::to_wstring(v);  }
	inline void FileOpenErrorPrint(FilepassString const& pass){ std::wcout << L"file open error: " << pass << std::endl; }
	#define SIG_TO_FPSTR(str) L ## str
	#define SIG_USE_BOOST_FILESYSTEM 0
#else
	using FilepassString = std::string;
	using FilepassStringC = char const*;
	template <class T> FilepassString to_fpstring(T v){ return std::to_string(v);  }
	inline void FileOpenErrorPrint(FilepassString const& pass){ std::cout << "file open error: " << pass << std::endl; }
	#define SIG_TO_FPSTR(str) str
	#define SIG_USE_BOOST_FILESYSTEM 1
#endif
	inline void print_ln(std::string const& str) { std::cout << str << std::endl; }
	inline void print_ln(std::wstring const& str) { std::wcout << str << std::endl; }


#define SIG_FILE_LOCALE_INIT\
	static bool first = true; \
	if (first){\
	std::locale::global(std::locale("")); \
	first = false; \
	}
}

#endif
