/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_SAVE_HPP
#define SIG_UTIL_SAVE_HPP

#include "../helper/helper_modules.hpp"
#include "../helper/maybe.hpp"

#include <fstream>
#include <locale>


/// \file save.hpp ファイルの保存

namespace sig
{

/// SaveLine, SaveNum の保存に関する設定
enum class WriteMode{
	/**上書き*/
	overwrite,
	/**末尾追記*/
	append
};


/// ファイル内容の初期化
/**
	\param file_pass 初期化したいファイルのパス

	\code
	const auto dir = modify_dirpass_tail( SIG_TO_FPSTR("./src"), true);
	const auto fpass = dir + SIG_TO_FPSTR("test.txt");

	clear_file(fpass);
	\endcode
*/
inline void clear_file(FilepassString const& file_pass)
{
	std::ofstream ofs(file_pass);
	ofs << "";
}

//@{ 

/// ファイルへ1行ずつ保存
/**
	\param src 保存対象
	\param ofs std::ofstream or std::wofstream
*/
template <class T,
	typename std::enable_if<!impl::container_traits<T>::exist>::type*& = enabler
>
inline void save_line(
	T src,
	typename impl::FStreamSelector<T>::ofstream& ofs)
{
	ofs << src << std::endl;
}

/// ファイルへ1行ずつまとめて保存
/**
	\param src 保存対象（\ref sig_container ）
	\param ofs std::ofstream or std::wofstream
*/
template <class C,
	typename std::enable_if<impl::container_traits<C>::exist>::type*& = enabler
>
void save_line(
	C const& src,
	typename impl::FStreamSelector<typename impl::container_traits<C>::value_type>::ofstream& ofs)
{
	typename impl::FStreamSelector<typename impl::container_traits<C>::value_type>::ostreambuf_iterator streambuf_iter(ofs);
	for (auto const& str : src){
		std::copy(str.begin(), str.end(), streambuf_iter);
		streambuf_iter = '\n';
	}
}

/// ファイルへ1行ずつ保存
/**
	file_passで指定したテキストファイルに、srcの内容を書き込み、最後に改行する．\n
	指定したファイルが存在しない場合、新たにファイルが作成される．

	\param src 保存対象（非コンテナ）
	\param file_pass 保存先のパス（ファイル名含む）
	\param open_mode [option] 上書き(overwrite) or 追記(append)

	\code
	const auto dir = modify_dirpass_tail( SIG_TO_FPSTR("./example"), true);
	const auto fpass = dir + SIG_TO_FPSTR("test.txt");

	std::string str{ "str test" };
	std::wstring wstr{ L"wstr test" };

	save_line(str, fpass);						// 上書き
	save_line(wstr, fpass, WriteMode::append);	// 末尾に追記
	\endcode

	\code
	// test.txt
	str test
	wstr test

	\endcode
*/
template <class T,
	typename std::enable_if<!impl::container_traits<T>::exist>::type*& = enabler
>
void save_line(
	T src,
	FilepassString const& file_pass,
	WriteMode open_mode = WriteMode::overwrite)
{
	SIG_FILE_LOCALE_INIT

	const auto mode = open_mode == WriteMode::overwrite ? std::ios::out : std::ios::out | std::ios::app;
	typename impl::FStreamSelector<T>::ofstream ofs(file_pass, mode);
	save_line(src, ofs);
}

/// ファイルへ1行ずつまとめて保存
/**
	file_passで指定したテキストファイルに、srcの内容をコンテナの1要素を1行として書き込む．\n
	指定したファイルが存在しない場合、新たにファイルが作成される．

	\param src 保存対象（\ref sig_container ）
	\param file_pass 保存先のパス（ファイル名含む）
	\param open_mode [option] 上書き(overwrite) or 追記(append)

	\code
	const auto dir = modify_dirpass_tail( SIG_TO_FPSTR("./example"), true);
	const auto fpass = dir + SIG_TO_FPSTR("test.txt");

	std::vector<std::string> data{ "aaa", "bbb", "ccc" };
	save_line(data, fpass);
	\endcode

	\code
	// test.txt
	aaa
	bbb
	ccc

	\endcode
*/
template <class C,
	typename std::enable_if<impl::container_traits<C>::exist>::type*& = enabler
>
void save_line(
	C const& src,
	FilepassString const& file_pass,
	WriteMode open_mode = WriteMode::overwrite)
{
	SIG_FILE_LOCALE_INIT

	const auto mode = open_mode == WriteMode::overwrite ? std::ios::out : std::ios::out | std::ios::app;
	typename impl::FStreamSelector<typename impl::container_traits<C>::value_type>::ofstream ofs(file_pass, mode);
	save_line(src, ofs);
}

//@}

//@{

/// 数値列(ex:ベクトル)の保存
/**
	file_passで指定したテキストファイルに、数値が格納されたコンテナsrcの各要素を文字列として書き込む．

	\param src 保存対象（\ref sig_container ）
	\param file_pass 保存先のパス（ファイル名含む）
	\param delimiter 数値間の区切り文字
	\param open_mode [option] 上書き(overwrite) or 追記(append)

	\code
	const auto dir = modify_dirpass_tail( SIG_TO_FPSTR("./example"), true);
	const auto fpass = dir + SIG_TO_FPSTR("test.txt");

	const auto list_num = std::list<double>{-1.1, -2.2, -3.3};
	const auto uset_num = std::unordered_set<double>{1.1, 2.2, 3.3};

	save_num(list_num, fpass, ",", WriteMode::overwrite);
	save_num(uset_num, fpass, "\n", WriteMode::append);
	\endcode

	\code
	// test.txt
	-1.1,-2.2,-3.3
	1.1
	2.2
	3.3
	\endcode
*/
template <class C,
	typename std::enable_if<impl::container_traits<C>::exist && !impl::container_traits<typename impl::container_traits<C>::value_type>::exist>::type*& = enabler
>
void save_num(
	C const& src,
	FilepassString const& file_pass,
	std::string delimiter,
	WriteMode open_mode = WriteMode::overwrite)
{
	save_line(cat(src, delimiter), file_pass, open_mode);
}

/// 2次元配列の数値(ex:行列)を保存
/**
	\param src 保存対象の行列（\ref sig_container )
	\param file_pass 保存先のパス（ファイル名含む）
	\param delimiter 数値間の区切り文字
	\param open_mode [option] 上書き(overwrite) or 追記(append)

	\code
	const auto dir = modify_dirpass_tail( SIG_TO_FPSTR("./example"), true);
	const auto fpass = dir + SIG_TO_FPSTR("test.txt");

	const array<std::vector<int>, 3> mat = {	// sig::array
		{ 1, 2, 3 },
		{ 4, 5, 6, 7 },
		{ 8, 9 }
	};

	save_num(mat, fpass, ",");
	\endcode

	\code
	// test.txt
	1,2,3
	4,5,6,7
	8,9
	\endcode
*/
template <class CC,
	typename std::enable_if<impl::container_traits<typename impl::container_traits<CC>::value_type>::exist>::type*& = enabler
>
void save_num(
	CC const& src,
	FilepassString const& file_pass,
	std::string delimiter,
	WriteMode open_mode = WriteMode::overwrite)
{
	std::vector<std::string> tmp;

	for (auto const& line : src){
		tmp.push_back(cat(line, delimiter));
	}
	save_line(tmp, file_pass, open_mode);
}

//@}

	/*
	//csvで保存
	template <class Num>
	void SaveCSV(std::vector<std::vector<Num>> const& data, std::vector<std::string> const& row_names, std::vector<std::string> const& col_names, std::wstring const& out_fullpass)
	{
		std::ofstream ofs(out_fullpass);

		//first row: field name
		ofs << ",";
		for (uint i = 1; i < data[0].size() + 1; ++i){
			auto name = i - 1 < col_names.size() ? col_names[i - 1] : "";
			ofs << i << ". " << name << ",";
		}
		ofs << "\n";

		//first col: field name
		for (uint j = 0; j < data.size(); ++j){
			auto name = j < row_names.size() ? row_names[j] : "";
			ofs << j + 1 << ". " << name << ",";

			for (auto e : data[j]){
				ofs << e << ",";
			}
			ofs << "\n";
		}
	}
	*/
}

#endif
