/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_REGEX_HPP
#define SIG_UTIL_REGEX_HPP

#include "../helper/type_map.hpp"
#include "../helper/maybe.hpp"
#include <regex>


#if SIG_MSVC_ENV
#define NOMINMAX
#include <windows.h>
#pragma warning ( disable : 4996 )
#endif

#if !SIG_MSVC_ENV && SIG_ENABLE_BOOST
#include <boost/regex.hpp>
#endif

#if (__GLIBCXX__ || __GLIBCPP__)
#define SIG_USE_GLIBCPP 1
#endif

/// \file regex.hpp 正規表現ユーティリティ

namespace sig
{

#if SIG_MSVC_ENV

/// 与えられた文字中に含まれる、正規表現の特殊文字をエスケープする
inline auto escape_regex(std::string const& expression) ->std::string
{
	static const SIG_Regex escape_reg(R"(([(){}\[\]|^?$.+*\\]))");
	return SIG_RegexReplace(expression, escape_reg, std::string(R"(\$1)"));
}

/**
	\sa escape_regex(std::string const& expression)
*/
inline auto escape_regex(std::wstring const& expression) ->std::wstring
{
	static const SIG_WRegex escape_reg(LR"(([(){}\[\]|^?$.+*\\]))");
	return SIG_RegexReplace(expression, escape_reg, std::wstring(LR"(\$1)"));
}

#elif (SIG_USE_GLIBCPP && SIG_ENABLE_BOOST)

/// 与えられた文字中に含まれる、正規表現の特殊文字をエスケープする
inline auto escape_regex(std::string const& expression) ->std::string
{
	static const SIG_Regex escape_reg(R"(([(){}\[\]|^?$.+*\\]))");
	return SIG_RegexReplace(expression, escape_reg, std::string(R"(\\$1)"));
}

/**
	\sa escape_regex(std::string const& expression)
*/
inline auto escape_regex(std::wstring const& expression) ->std::wstring
{
	static const SIG_WRegex escape_reg(LR"(([(){}\[\]|^?$.+*\\]))");
	return SIG_RegexReplace(expression, escape_reg, std::wstring(LR"(\\$1)"));
}
#endif


#if SIG_MSVC_ENV || (SIG_USE_GLIBCPP && SIG_ENABLE_BOOST)

/// エスケープ処理を行い、regexオブジェクトを返す
/**
	\param expression 正規表現の文字列
	\return std::regex or std::wregex (libstdc++使用時 かつ boost使用時は boost::regex)
*/
template <class S>
auto make_regex(S const& expression) ->typename impl::Str2RegexSelector<impl::TString<S>>::regex
{
	return typename impl::Str2RegexSelector<impl::TString<S>>::regex(escape_regex(expression));
}


/// std::regex_search のラッパ関数
/**
	探索結果にアクセスしやすい様に二次元配列(vector<vector<string>>)に結果を格納している

	\param src 探索対象の文字列
	\param expression: 正規表現オブジェクト
	\return 探索成功時には、result[マッチした箇所の順番][マッチ内の参照の順番. 0は全文, 1以降は参照箇所]を内包したoptionalを返す．失敗時にはnothingを返す

	例：
	src = "test tes1 tes2".
	expression = std::regex("tes(\\d)").
	return : [[tes1, 1], [tes2, 2]]
*/
template <class S>
auto regex_search(S src, typename impl::Str2RegexSelector<impl::TString<S>>::regex const& expression)
	->Maybe<std::vector<std::vector<impl::TString<S>>>>
{
	using R = std::vector<std::vector<impl::TString<S>>>;
	typename impl::Str2RegexSelector<impl::TString<S>>::smatch match;
	auto tmp = impl::TString<S>(src);
	R d;

	while (SIG_RegexSearch(tmp, match, expression)){
		d.push_back(std::vector<impl::TString<S>>());
		for (auto const& m : match) d.back().push_back(m);
		tmp = match.suffix().str();
	}

	return d.empty() ? Nothing(std::move(d)) : Just<R>(std::move(d));
}
#endif

}

#endif