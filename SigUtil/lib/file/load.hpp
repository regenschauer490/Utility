/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_LOAD_HPP
#define SIG_UTIL_LOAD_HPP

#include "../helper/helper.hpp"
#include "../helper/maybe.hpp"

#include <fstream>
#include <locale>


/// \file load.hpp ファイルの読込

namespace sig
{

template <class R>
using IfsSelector = typename impl::SameIf<R, std::string, std::ifstream, std::wifstream>::type;

// ファイルから1行ずつ読み込む
// empty_dest: 保存先の空のコンテナ
// ifs: std::ifstream or std::wifstream
// conv: 読み込んだ文字列から任意型Rへの変換関数(文字列 -> 数値型へはread_numを推奨) string or wstring -> R
template <class C, class R = typename impl::container_traits<C>::value_type>
bool load_line(
	C& empty_dest,
	IfsSelector<R>& ifs,
	std::function< R(typename impl::SameIf<R, std::string, std::string, std::wstring>::type) > const& conv = nullptr)
{
	static_assert(std::is_same<R, typename impl::container_traits<C>::value_type>::value, "error in load_line: R and C::value_type don't match");

	typename impl::SameIf<R, std::string, std::string, std::wstring>::type line;

	SIG_FILE_LOCALE_INIT

	while (ifs && std::getline(ifs, line)){
		conv ? impl::container_traits<C>::add_element(empty_dest, conv(std::move(line))) : impl::container_traits<C>::add_element(empty_dest, std::move(line));
	}
	return static_cast<bool>(ifs);
}

// ファイルから1行ずつ読み込む
// empty_dest: 保存先の空のコンテナ
// file_pass: 読み込み先のディレクトリとファイル名（フルパス）
// conv: 読み込んだ文字列から任意型Rへの変換関数(文字列 -> 数値型へはread_numを推奨) string or wstring -> R
template <class C, class R = typename impl::container_traits<C>::value_type>
bool load_line(
	C& empty_dest,
	FilepassString const& file_pass,
	std::function< R(typename impl::SameIf<R, std::string, std::string, std::wstring>::type)> const& conv = nullptr)
{
	IfsSelector<R> ifs(file_pass);
	if (!ifs){
		//FileOpenErrorPrint(file_pass);
		return false;
	}
	return load_line(empty_dest, ifs, conv);
}

// ファイルから1行ずつ読み込み、結果を返す
// R: 返す文字列型(std::string or std::wstring)
// ifs: std::ifstream or std::wifstream
// 読込失敗: return -> nothing (boost非使用時は空のコンテナ)
template <class R, class C = std::vector<R>>
auto load_line(IfsSelector<R>& ifs) ->Maybe<C>
{
	C tmp;
	load_line(tmp, ifs);
	return tmp.size() ? Just<C>(std::move(tmp)) : Nothing(std::move(tmp));
}

// ファイルから1行ずつ読み込み、結果を返す
// R: 返す文字列型(std::string or std::wstring)
// file_pass: 読み込み先のディレクトリとファイル名（フルパス）
// 読込失敗: return -> nothing (boost非使用時は空のコンテナ)
template <class R, class C = std::vector<R>>
auto load_line(FilepassString const& file_pass) ->Maybe<C>
{
	IfsSelector<R> ifs(file_pass);
	if (!ifs){
		//FileOpenErrorPrint(file_pass);
		return Nothing(C());
	}
	return load_line<R, C>(ifs);
}

template <class R, class C = std::vector<R>>
auto load_line(FilepassStringC file_pass) ->Maybe<C>
{
	return load_line<R, C>(static_cast<impl::TString<FilepassStringC>>(file_pass));
}


// 数値列を読み込む
// empty_dest: 保存先の空のコンテナ
// file_pass: 読み込み先のディレクトリとファイル名（フルパス）
// delimiter: 数値間の区切り文字を指定 (デフォルトは行区切り)
// return -> 読み込みの成否
template <class C, class RT = typename impl::container_traits<C>::value_type, typename std::enable_if<!impl::container_traits<typename impl::container_traits<C>::value_type>::exist>::type*& = enabler>
bool read_num(
	C& empty_dest,
	FilepassString const& file_pass,
	std::string delimiter = "\n")
{
	auto read_str = load_line<std::string>(file_pass);

	if (!isJust(read_str)) return false;

	if (delimiter == "\n"){
		for (auto const& line : fromJust(read_str)){
			impl::container_traits<C>::add_element(empty_dest, impl::Str2NumSelector<RT>()(line));
		}
	}
	else{
		// 遅いので注意
		auto sp = split(fromJust(read_str)[0], delimiter);
		for (auto v : sp) impl::container_traits<C>::add_element(empty_dest, impl::Str2NumSelector<RT>()(v));
	}
	return true;
}

// 2次元配列の数値(ex:行列)を読み込む
// empty_dest: 保存先の空のコンテナのコンテナ
// file_pass: 読み込み先のディレクトリとファイル名（フルパス）
// delimiter: 数値間の区切り文字を指定
// return -> 読み込みの成否
template <class CC, class RC = typename impl::container_traits<CC>::value_type, class RT = typename impl::container_traits<RC>::value_type>
bool read_num(
	CC& empty_dest,
	FilepassString const& file_pass, 
	std::string delimiter)
{
	auto read_str = load_line<std::string>(file_pass);
	if (!isJust(read_str)) return false;

	for (auto const& line : fromJust(read_str)){
		RC tmp;
		auto sp = split(line, delimiter);

		for (auto const& v : sp){
			impl::container_traits<RC>::add_element(tmp, impl::Str2NumSelector<RT>()(v));
		}
		impl::container_traits<CC>::add_element(empty_dest, std::move(tmp));
	}
	return true;
}

// 数値列を読み込み、結果のコンテナを返す
// file_pass: 読み込み先のディレクトリとファイル名（フルパス）
// delimiter: 数値間の区切り文字を指定 (デフォルトは行区切り)
// 読込失敗: return -> nothing (boost非使用時は空のコンテナ)
template <class C, typename std::enable_if<impl::container_traits<C>::exist && !impl::container_traits<typename impl::container_traits<C>::value_type>::exist>::type*& = enabler>
auto read_num(
	FilepassString const& file_pass,
	std::string delimiter = "\n"
	) ->Maybe<C>
{
	C tmp;
	read_num(tmp, file_pass, delimiter);
	return tmp.size() ? Just<C>(std::move(tmp)) : Nothing(std::move(tmp));
}

// 2次元配列の数値(ex:行列)を読み込む
// file_pass: 読み込み先のディレクトリとファイル名（フルパス）
// delimiter: 数値間の区切り文字を指定
// 読込失敗: return -> nothing (boost非使用時は空のコンテナ)
template <class CC, typename std::enable_if<impl::container_traits<typename impl::container_traits<CC>::value_type>::exist>::type*& = enabler>
auto read_num(
	FilepassString const& file_pass,
	std::string delimiter
	) ->Maybe<CC>
{
	CC tmp;
	read_num(tmp, file_pass, delimiter);
	return tmp.size() ? Just<CC>(std::move(tmp)) : Nothing(std::move(tmp));
}

}

#endif
