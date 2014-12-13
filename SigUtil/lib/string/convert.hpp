/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_CONVERT_HPP
#define SIG_UTIL_CONVERT_HPP

#include "../helper/container_traits.hpp"


#if SIG_MSVC_ENV
#define NOMINMAX
#include <windows.h>
#pragma warning ( disable : 4996 )
#endif

#if (__GLIBCXX__ || __GLIBCPP__)
#define SIG_USE_GLIBCPP 1
#endif

#if (SIG_MSVC_ENV && SIG_MSVC_VER != 140) && !(SIG_USE_GLIBCPP)
#include <codecvt>
/// <codecvt>を使用するか (GCC非対応)
#define SIG_ENABLE_CODECVT 1
#endif


/// \file convert.hpp 文字コード変換

namespace sig
{

/// ワイド文字 -> マルチバイト文字 
/**
	ex: Windows環境では UTF-16 -> Shift-JIS

	\param src 変換対象の文字列

	\return 変換後の文字列

	\code
	const std::wstring wstr(L"aあ亜");

	std::string str = wstr_to_str(wstr);
	\endcode
*/
inline auto wstr_to_str(std::wstring const& src)
	->std::string //Just<std::string>
{
	size_t mbs_size = src.length() * MB_CUR_MAX + 1;
	if (mbs_size < 2 || src == L"\0") return std::string();
	char *mbs = new char[mbs_size];

#if SIG_MSVC_ENV && SIG_DEBUG_MODE
	size_t num;
	int error = wcstombs_s(&num, mbs, mbs_size, src.c_str(), src.length() * MB_CUR_MAX + 1);
#else
	int error = wcstombs(mbs, src.c_str(), src.length() * MB_CUR_MAX + 1);
#endif
	std::string dest(mbs);
	delete[] mbs;

	return error == -1 ? std::string() : dest; //error ? Nothing(dest) : Just<std::string>(std::move(dest));
}

/// ワイド文字 -> マルチバイト文字 
/**
	コンテナの全要素をまとめて変換
	ex: Windows環境では UTF-16 -> Shift-JIS

	\param src 変換対象の文字列集合が格納されたコンテナ（\ref sig_container ）

	\return 変換後の文字列集合が格納されたコンテナ

	\sa wstr_to_str(std::wstring const& src)

	\code
	const std::vector<std::wstring> wsvec{L"a", L"あ", L"亜");

	auto svec = wstr_to_str(wsvec);	// std::vector<std::string>
	\endcode
*/
template <class C,
	class R = typename impl::container_traits<C>::template rebind<std::string>,
	typename std::enable_if<std::is_same<typename impl::container_traits<C>::value_type, std::wstring>::value>::type*& = enabler
>
auto wstr_to_str(C const& src) ->R
{
	R result = impl::container_traits<R>::make(src.size());

	for (auto const& str : src){
		std::string r = wstr_to_str(str);
		if (!r.empty()) impl::container_traits<R>::add_element(result, std::move(r));
	}

	return result;
}

/// マルチバイト文字 -> ワイド文字
/**
	ex: Windows環境では Shift-JIS -> UTF-16
	
	\param src 変換対象の文字列

	\return 変換後の文字列

	\code
	const std::string str = "aあ亜";

	std::wstring wstr = str_to_wstr(str);
	\endcode
*/
inline auto str_to_wstr(std::string const& src) ->std::wstring //Just<std::wstring>
{
	size_t wcs_size = src.length() + 1;
	if (wcs_size < 2 || src == "\0") return std::wstring();
	wchar_t *wcs = new wchar_t[wcs_size];

#if SIG_MSVC_ENV && SIG_DEBUG_MODE
	size_t num;
	int error = mbstowcs_s(&num, wcs, wcs_size, src.c_str(), src.length() + 1);
#else
	int error = mbstowcs(wcs, src.c_str(), src.length() + 1);
#endif
	std::wstring dest(wcs);
	delete[] wcs;
	return error == -1 ? std::wstring() : dest; //error ? Nothing(dest) : Just<std::wstring>(std::move(dest));
}

/// マルチバイト文字 -> ワイド文字
/**
	コンテナの全要素をまとめて変換
	ex: Windows環境では Shift-JIS -> UTF-16

	\param src 変換対象の文字列が格納されたコンテナ（\ref sig_container ）

	\return 変換後の文字列が格納されたコンテナ

	\sa  str_to_wstr(std::string const& src)

	\code
	const std::vector<std::string> svec{"a", "あ", "亜");

	auto wsvec = str_to_wstr(svec);		// std::vector<std::wstring>
	\endcode
*/
template <class C,
	class R = typename impl::container_traits<C>::template rebind<std::wstring>,
	typename std::enable_if<std::is_same<typename impl::container_traits<C>::value_type, std::string>::value>::type*& = enabler
>
auto str_to_wstr(C const& src) ->R
{
	R result = impl::container_traits<R>::make(src.size());

	for (auto const& str : src){
		std::wstring r = str_to_wstr(str);
		if (!r.empty()) impl::container_traits<R>::add_element(result, std::move(r));
	}

	return result;
}

#if SIG_ENABLE_CODECVT
/// UTF-8 -> UTF-16
/**
	\pre <codecvt>が必要

	\param src 変換対象の文字列

	\return 変換後の文字列
*/
inline auto utf8_to_utf16(std::string const& src) ->std::u16string
{
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;

	return convert.from_bytes(src);
}

/// UTF-16 -> UTF-8
/**
	\pre <codecvt>が必要

	\param src 変換対象の文字列

	\return 変換後の文字列
*/
inline auto utf16_to_utf8(std::u16string const& src) ->std::string
{
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;

	return convert.to_bytes(src);
}

/// UTF-8 -> UTF-32
/**
	\pre <codecvt>が必要

	\param src 変換対象の文字列

	\return 変換後の文字列
*/
inline auto utf8_to_utf32(std::string const& src) ->std::u32string
{
	std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> utf32conv;

	return utf32conv.from_bytes(src);
}

/// UTF-32 -> UTF-8
/**
	\pre <codecvt>が必要

	\param src 変換対象の文字列

	\return 変換後の文字列
*/
inline auto utf32_to_utf8(std::u32string const& src) ->std::string
{
	std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> utf32conv;

	return utf32conv.to_bytes(src);
}
#endif

#if SIG_MSVC_ENV

/// ShiftJIS -> UTF-16
/**
	\pre Windows環境のみ

	\param src 変換対象の文字列

	\return 変換後の文字列
*/
inline auto sjis_to_utf16(std::string const& src) ->std::u16string //Just<std::u16string>
{
	const int dest_size = ::MultiByteToWideChar(CP_ACP, 0, src.c_str(), -1, NULL, 0);
	BYTE* buf = new BYTE[dest_size * 2 + 2];

	bool is_succeed = ::MultiByteToWideChar(CP_ACP, 0, src.c_str(), -1, reinterpret_cast<LPWSTR>(buf), dest_size) > 0 ? true : false;
	std::u16string dest(reinterpret_cast<char16_t*>(buf));
	delete[] buf;

	return is_succeed ? dest : std::u16string(); //is_succeed ? Just<std::u16string>(std::move(dest)) : Nothing(std::u16string());
}

/// UTF-16 -> ShiftJIS
/**
	\pre Windows環境のみ

	\param src 変換対象の文字列

	\return 変換後の文字列
*/
inline auto utf16_to_sjis(std::u16string const& src) ->std::string //Just<std::string>
{
	LPCWSTR srcp = reinterpret_cast<LPCWSTR>(src.c_str());
	const int dest_size = ::WideCharToMultiByte(CP_ACP, 0, srcp, -1, NULL, 0, NULL, NULL);
	BYTE* buf = new BYTE[dest_size * 2];

	bool is_succeed = ::WideCharToMultiByte(CP_ACP, 0, srcp, -1, reinterpret_cast<LPSTR>(buf), dest_size, NULL, NULL) > 0 ? true : false;
	std::string dest(reinterpret_cast<char*>(buf));
	delete[] buf;

	return is_succeed ? dest : std::string(); //is_succeed ? Just<std::string>(std::move(dest)) : Nothing(std::string());
}
#endif

#ifdef SIG_ENABLE_CODECVT 
/// ShiftJIS -> UTF-8
/**
	\pre Windows環境のみ

	\param src 変換対象の文字列

	\return 変換後の文字列
*/
inline auto sjis_to_utf8(std::string const& src) ->std::string
{
	return utf16_to_utf8(sjis_to_utf16(src));
}

/// UTF-8 -> ShiftJIS
/**
	\pre Windows環境のみ

	\param src 変換対象の文字列

	\return 変換後の文字列
*/
inline auto utf8_to_sjis(std::string const& src) ->std::string
{
	return utf16_to_sjis(utf8_to_utf16(src));
}
#endif

}
#endif