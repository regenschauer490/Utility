/*
Copyrightc 2014 Akihiro Nishimura

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

namespace impl
{
	template <class T>
	using IfsSelector = typename impl::SameIf<
		T,
		std::string,
		std::ifstream,
		typename impl::SameIf<
		T,
		std::wstring,
		std::wifstream,
		std::basic_ifstream<T>
		>::type
	>::type;
}

//@{ 

/// ファイルから1行ずつ読み込む（ifstreamを指定）
/**
	\param empty_dest 保存先のコンテナ（\ref sig_container )
	\param ifs std::ifstream or std::wifstream

	\return 読み込みの成否
*/
template <
	class IFS_CHAR, 
	class C, 
	class R = typename impl::container_traits<C>::value_type
>
bool load_line(
	C& empty_dest,
	std::basic_ifstream<IFS_CHAR>& ifs)
{
	std::basic_string<IFS_CHAR> line;

	SIG_FILE_LOCALE_INIT

	while (ifs && std::getline(ifs, line)){
		impl::container_traits<C>::add_element(empty_dest, std::move(line));
	}
	return static_cast<bool>(ifs);
}

/// ファイルから1行ずつ読み込む（ファイル名を指定）
/**
	\param empty_dest 保存先のコンテナ（\ref sig_container )
	\param file_pass 保存先のパス（ファイル名含む）

	\return 読み込みの成否

	\code
	const auto dir = modify_dirpass_tail( SIG_TO_FPSTR("./example"), true);
	const auto fpass = dir + SIG_TO_FPSTR("test.txt");

	std::vector<std::string> input;
	load_line(input, fpass);
	\endcode
*/
template <
	class C,
	class R = typename impl::container_traits<C>::value_type
>
bool load_line(
	C& empty_dest,
	FilepassString const& file_pass)
{
	impl::IfsSelector<R> ifs(file_pass);

	if (!ifs){
		//FileOpenErrorPrint(file_pass);
		return false;
	}
	return load_line(empty_dest, ifs);
}

//@}

//@{ 

/*
// ファイルから1行ずつ読み込み、結果を返す（ifstreamを指定）
// R: 返す文字列型(std::string or std::wstring)
// ifs: std::ifstream or std::wifstream
// 読込失敗: return -> nothing (boost非使用時は空のコンテナ)
template <class IST, class R = IST, class C = std::vector<R>>
auto load_line(impl::IfsSelector<IST>& ifs) ->Maybe<C>
{
C tmp;
load_line(tmp, ifs);
return tmp.size() ? Just<C>(std::move(tmp)) : Nothing(std::move(tmp));
}
*/

/// ファイルから1行ずつ読み込み、結果を返す（ファイル名を指定）
/**
	\tparam ISTR [option] 読み込んだ文字列を保持する型（std::string or std::wstring）

	\param file_pass 読み込むファイルのパス

	\return 読み込み結果（値は\ref sig_maybe で返される）

	\code
	const auto dir = modify_dirpass_tail( SIG_TO_FPSTR("./example"), true);
	const auto fpass = dir + SIG_TO_FPSTR("test.txt");

	auto input = load_line(fpass);

	auto& data = *input;		// std::vector<std::string>
	\endcode
*/
template <
	class ISTR = std::string,
	class C = std::vector<ISTR>
>
auto load_line(FilepassString const& file_pass) ->Maybe<C>
{
	C tmp;
	impl::IfsSelector<ISTR> ifs(file_pass);

	if (!ifs){
		//FileOpenErrorPrint(file_pass);
		return Nothing(C());
	}

	load_line(tmp, ifs);

	return tmp.empty() ? Nothing(tmp) : Just<C>(std::move(tmp));
}

/**
	file_pass が const char* , const wcahr_t* である場合のオーバーロード

	\sa load_line(FilepassString const& file_pass)
*/
template <
	class ISTR = std::string,
	class R = ISTR,
	class C = std::vector<R>
>
auto load_line(FilepassStringC file_pass) ->Maybe<C>
{
	return load_line<ISTR, C>(static_cast<impl::string_t<FilepassStringC>>(file_pass));
}

//@}

//@{ 

/// ファイルから1行ずつ読み込み、同時に変換処理を行う（ifstreamを指定）
/**
	\param empty_dest 保存先のコンテナ（\ref sig_container )
	\param ifs std::ifstream or std::wifstream
	\param conv 読み込んだ文字列から任意型Rへの変換関数(文字列 -> 数値型へはload_numを推奨)
*/
template <
	class IFS_CHAR,
	class F,
	class C,
	class R = typename impl::container_traits<C>::value_type
>
bool load_line(
	C& empty_dest,
	std::basic_ifstream<IFS_CHAR>& ifs,
	F const& conv)
{
	std::basic_string<IFS_CHAR> line;

	SIG_FILE_LOCALE_INIT

	while (ifs && std::getline(ifs, line)){
		impl::container_traits<C>::add_element(empty_dest, conv(std::move(line)));
	}
	return static_cast<bool>(ifs);
}

/// ファイルから1行ずつ読み込み、同時に変換処理を行う（ファイル名を指定）
/**
	\tparam ISTR [option] 読み込んだ文字列を保持する型（std::string or std::wstring）

	\param empty_dest 保存先のコンテナ（\ref sig_container )
	\param file_pass 保存先のパス（ファイル名含む）
	\param conv 読み込んだ文字列から任意型Rへの変換関数(文字列 -> 数値型へはload_numを推奨)

	\code
	struct Test{
		int a;
		Test(double v, double p) : a(std::pow(v, p)){}
	};

	const auto dir = modify_dirpass_tail( SIG_TO_FPSTR("./example"), true);
	const auto fpass = dir + SIG_TO_FPSTR("test.txt");

	std::vector<Test> input;

	load_line(
		input,
		fpass,
		[&](std::string&& s){
			auto tmp = split(s, ",");		// example: s = "2.73,0.001"
			return Test(tmp[0], tmp[1]);
		}
	);
	\endcode
*/
template <
	class ISTR = std::string,
	class F,
	class C,
	class R = typename impl::container_traits<C>::value_type
>
bool load_line(
	C& empty_dest,
	FilepassString const& file_pass,
	F const& conv)
{
	impl::IfsSelector<
		typename impl::SameIf<R, std::string, R,
		typename impl::SameIf<R, std::wstring, R, ISTR>::type
		>::type
	> ifs(file_pass);

	if (!ifs){
		//FileOpenErrorPrint(file_pass);
		return false;
	}
	return load_line(empty_dest, ifs, conv);
}

//@}

//@{ 

/// 数値列を読み込む
/**
	\param empty_dest 保存先のコンテナ（\ref sig_container )
	\param file_pass 保存先のパス（ファイル名含む）
	\param delimiter [option] 数値間の区切り文字

	\return 読み込みの成否

	\exception std::invalid_argument（数値に変換できない場合）, std::out_of_range（intやdouble等で変換可能な値の範囲を超えている場合）\n

	\code
	const auto dir = modify_dirpass_tail( SIG_TO_FPSTR("./example"), true);
	const auto fpass1 = dir + SIG_TO_FPSTR("test1.txt");
	const auto fpass2 = dir + SIG_TO_FPSTR("test2.txt");

	std::vector<double> input1;
	std::set<double> input2;

	load_num(input1, fpass1);
	load_num(input2, fpass2, ",");
	\endcode

	\code
	// test1.txt
	1.1
	2.2
	3.3
	\endcode

	\code
	// test2.txt
	1.1,2.2,3.3
	\endcode
*/
template <
	class C,
	class RT = typename impl::container_traits<C>::value_type, typename std::enable_if<!impl::container_traits<typename impl::container_traits<C>::value_type>::exist>::type*& = enabler
>
bool load_num(
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
		auto sp = split(fromJust(read_str)[0], delimiter);
		for (auto v : sp) impl::container_traits<C>::add_element(empty_dest, impl::Str2NumSelector<RT>()(v));
	}
	return true;
}

/// 数値列を読み込み、結果を返す
/**
	\tparam R 数値の型（int, double等）
	\tparam C [option] コンテナの型（\ref sig_container )

	\param file_pass 保存先のパス（ファイル名含む）
	\param delimiter [option] 数値間の区切り文字

	\return 読み込み結果（値は\ref sig_maybe で返される）

	\exception  std::invalid_argument（数値に変換できない場合）, std::out_of_range（intやdouble等で変換可能な値の範囲を超えている場合）\n

	\code
	const auto dir = modify_dirpass_tail( SIG_TO_FPSTR("./example"), true);
	const auto fpass1 = dir + SIG_TO_FPSTR("test1.txt");
	const auto fpass2 = dir + SIG_TO_FPSTR("test2.txt");
	
	auto input1 = load_num<double>(fpass1);
	auto input2 = load_num<double, std::set<double>>(fpass2, ",");
	\endcode

	\code
	// test1.txt
	1.1
	2.2
	3.3
	\endcode

	\code
	// test2.txt
	1.1,2.2,3.3
	\endcode
*/
template <
	class R,
	class C = std::vector<R>, typename std::enable_if<impl::container_traits<C>::exist && !impl::container_traits<typename impl::container_traits<C>::value_type>::exist>::type*& = enabler
>
auto load_num(
	FilepassString const& file_pass,
	std::string delimiter = "\n"
	) ->Maybe<C>
{
	C tmp;
	load_num(tmp, file_pass, delimiter);
	return tmp.size() ? Just<C>(std::move(tmp)) : Nothing(std::move(tmp));
}

//@}

//@{ 

/// 2次元配列の数値(ex:行列)を読み込む
/**
	\param empty_dest 保存先のコンテナ（\ref sig_container )
	\param file_pass 保存先のパス（ファイル名含む）
	\param delimiter 数値間の区切り文字
	
	\return 読み込みの成否

	\exception  std::invalid_argument（数値に変換できない場合）, std::out_of_range（intやdouble等で変換可能な値の範囲を超えている場合）\n

	\code
	const auto dir = modify_dirpass_tail( SIG_TO_FPSTR("./example"), true);
	const auto fpass = dir + SIG_TO_FPSTR("test.txt");

	array<std::vector<int>, 3> input_mat;

	load_num2d(input_mat, fpass, ",");
	assert(input_mat[2][0], 7);
	\endcode

	\code
	// test.txt
	1,2,3
	4,5,6,7
	8,9
	\endcode
*/
template <
	class CC,
	class RC = typename impl::container_traits<CC>::value_type,
	class RT = typename impl::container_traits<RC>::value_type
>
bool load_num2d(
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


/// 2次元配列の数値(ex:行列)を読み込む
/**
	\tparam R 数値の型（int, double等）
	\tparam CC [option] コンテナの型（\ref sig_container )

	\param file_pass 保存先のパス（ファイル名含む）
	\param delimiter 数値間の区切り文字
	
	\return 読み込み結果（値は\ref sig_maybe で返される）

	\exception  std::invalid_argument（数値に変換できない場合）, std::out_of_range（intやdouble等で変換可能な値の範囲を超えている場合）\n

	\code
	const auto dir = modify_dirpass_tail( SIG_TO_FPSTR("./example"), true);
	const auto fpass = dir + SIG_TO_FPSTR("test.txt");

	auto input_mat = load_num2d<double>(input_mat, fpass, ",");
	assert(input_mat[2][0], 7);
	\endcode

	\code
	// test.txt
	1,2,3
	4,5,6,7
	8,9
	\endcode
*/
template <
	class R,
	class CC = std::vector<std::vector<R>>,
	typename std::enable_if<impl::container_traits<typename impl::container_traits<CC>::value_type>::exist>::type*& = enabler
>
auto load_num2d(
	FilepassString const& file_pass,
	std::string delimiter
	) ->Maybe<CC>
{
	CC tmp;
	load_num2d(tmp, file_pass, delimiter);
	return tmp.size() ? Just<CC>(std::move(tmp)) : Nothing(std::move(tmp));
}

//@}

}
#endif
