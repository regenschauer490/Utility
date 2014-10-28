/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_MANIPULATE_HPP
#define SIG_UTIL_MANIPULATE_HPP

#include "../helper/type_convert.hpp"
#include "../helper/container_helper.hpp"


/// \file manipulate.hpp 文字列処理ユーティリティ

namespace sig
{

/// 文字列(src)をある文字列(delimiter)を目印に分割する
/**
	\tparam CSeq  returnされるシーケンスコンテナの種類

	\param src 分割対象の文字列
	\param delimiter 分割の目印となる文字列

	\return 分割後の文字列が格納されたシーケンスコンテナ

	\code
	auto src = " one,2, 参 ";
	
	auto spl = split(src, ",");		// vector<string>{" one", "2", " 参 "}
	\endcode
*/
template <
	template <class T_, class = std::allocator<T_>> class CSeq = std::vector,
	class S = std::string,
	class TS = impl::string_t<S>
>
auto split(S const& src, impl::string_t<S> const& delimiter) ->CSeq<TS>
{
	CSeq<TS> result;
	const uint mag = delimiter.size();
	const uint length = src.size();
	uint pos = 0, ppos = 0;

	while (pos < length){
		if (src[pos] == delimiter[0]){
			// check to match delimiter
			bool cf = true;
			for (uint d = 1; d < mag; ++d){
				if (src[pos+d] != delimiter[d]){
					cf = false;
					break;
				}
			}

			if (cf){
				if (pos - ppos > 0) result.push_back(src.substr(ppos, pos - ppos));
				//impl::container_traits<CSeq>::add_element(result, src.substr(ppos, pos));
				pos += mag;
				ppos = pos;
			}
			else{
				pos += mag;
			}
		}
		else{
			++pos;
		}
	}
	if (pos - ppos > 0)result.push_back(src.substr(ppos, pos - ppos));

	return result;
}

/**
	\sa split(S const& src, impl::string_t<S> const& delimiter)
*/
template < template <class T_, class = std::allocator<T_>> class CSeq = std::vector >
auto split(char const* const src, char const* const delimiter) ->CSeq<std::string>
{
	return split<CSeq>(std::string(src), std::string(delimiter));
}

/**
	\sa split(S const& src, impl::string_t<S> const& delimiter)
*/
template < template < class T_, class = std::allocator<T_>> class CSeq = std::vector >
auto split(wchar_t const* const src, wchar_t const* const delimiter) ->CSeq<std::wstring>
{
	return split<CSeq>(std::wstring(src), std::wstring(delimiter));
}


namespace impl
{
//	コンテナに格納された各文字列(数値の場合は文字列に変換)を順番に結合して1つの文字列に(delimiterで区切り指定)
template <class It, class S, class OSS>
auto cat_str_impl(It begin, It end, S const& delimiter, OSS& osstream, std::locale osstream_locale = std::locale("")) ->decltype(osstream.str())
{
	if (begin == end) return osstream.str();

	std::locale::global(osstream_locale);
	std::locale ctype_default(std::locale::classic(), osstream_locale, std::locale::ctype);
	osstream.imbue(ctype_default);

	for (osstream << *begin++; begin != end; ++begin){
		osstream << delimiter << *begin;
	}
	return osstream.str();
}

}	// impl


/// コンテナ中の各文字列や数値を結合
/**
	conatiner中の要素が数値の場合、ostringstreamで出力される文字列に変換される

	\param container 文字列か数値が格納されたコンテナ（\ref sig_container ）
	\param delimiter 結合する文字列間に挿入される文字列
	\param osstream_locale [option] 出力に影響するロケール 

	\return 結合した文字列

	\code
	auto cat1 = cat(std::vector<std::string>{"eins", "zwei", "drei"}, "");
	auto cat2 = cat(std::list<std::wstring>{L"eins", L"zwei", L"drei"}, L",");

	assert(cat1 == "einszweidrei");
	assert(cat2 == L"eins,zwei,drei");
	\endcode
*/
template <class C,
	class CR = typename impl::remove_const_reference<C>::type,
	class S = impl::SStreamSelector< typename impl::container_traits<CR>::value_type>::string
>
auto cat(C&& container, typename std::identity<S>::type const& delimiter, std::locale osstream_locale = std::locale("")) ->S
{
	return impl::cat_str_impl(
		impl::begin(std::forward<C>(container)),
		impl::end(std::forward<C>(container)),
		delimiter,
		typename impl::SStreamSelector<S>::ostringstream{},
		osstream_locale
	);
}

/**
	containerがinitializer_listの場合

	\sa cat(C const& container, S const& delimiter, std::locale osstream_locale)
*/
template <class T, class S = typename impl::SStreamSelector<T>::string>
auto cat(std::initializer_list<T> container, typename std::identity<S>::type const& delimiter, std::locale osstream_locale = std::locale("")) ->S
{
	return impl::cat_str_impl(
		impl::begin(container),
		impl::end(container),
		delimiter,
		typename impl::SStreamSelector<T>::ostringstream{},
		osstream_locale
	);
}

}
#endif
