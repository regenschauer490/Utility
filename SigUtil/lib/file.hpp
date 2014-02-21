/*
Copyright(c) 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTILUTIL_FILE_HPP
#define SIG_UTILUTIL_FILE_HPP

#include "string.hpp"
#include "sigutil.hpp"

#include <fstream>
#include <locale>

#if SIG_MSVC_ENV
#include <windows.h>
#endif

#if SIG_ENABLE_BOOST
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;
#endif

/* 入出力関連 */

namespace sig{

	//ディレクトリ・ファイルパスの末尾に'/'or'\'があるかチェックし、付けるか外すかどうか指定
	inline FileString DirpassTailModify(FileString const& directory_pass, bool const has_slash)
	{
		if (directory_pass.empty()) return directory_pass;

		auto tail = directory_pass.back();

		if (has_slash){
			//付ける場合
			if (tail == '/' || tail == '\\') return directory_pass;
#if SIG_MSVC_ENV
			else return (directory_pass + L"/");
#else
			else return (directory_pass + "/");
#endif
		}
		else{
			if (tail != '/' && tail != '\\') return directory_pass;
			else{
				auto tmp = directory_pass;
				tmp.pop_back();
				return tmp;
			}
		}
	};


	//指定ディレクトリにあるファイル名を取得
	//directry_pass：調べたいディレクトリのパス
	//hidden_file：true->隠しファイルのみ, false->非隠しファイルのみ (Windows, Linux環境のみ)
	//extension：拡張子指定(オプション)
	//読み込み失敗: return -> nothing or empty-vector
	inline auto GetFileNames(FileString const& directory_pass, bool hidden_file, std::wstring extension = L"") ->Just<std::vector<std::wstring>>::type
	{
		typedef std::vector<std::wstring> ResultType;
		ResultType result;

#if SIG_MSVC_ENV
		WIN32_FIND_DATA fd;
		auto query = extension.empty() ? L"?*" : L"*" + extension;
		auto pass = DirpassTailModify(directory_pass, true) + query;
		auto hFind = FindFirstFile(pass.c_str(), &fd);

		if (hFind == INVALID_HANDLE_VALUE){
			return Nothing(std::move(result));
		}
		else{
			do{
				if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && BoolConsistency(hidden_file, (fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)))
				{
					result.push_back(std::wstring(fd.cFileName));
				}
			} while (FindNextFile(hFind, &fd));

			FindClose(hFind);
			return Just<ResultType>::type(std::move(result));
		}
#elif SIG_ENABLE_BOOST
		auto IsHidden = [](fs::path const& p){
			auto name = p.filename();
			if (name.c_str()[0] == '.' && name != ".." &&	name != ".") return true;
			else return false;
		};

		fs::directory_iterator end;
		for (fs::directory_iterator it(directory_pass); it != end; ++it)
		{
			if (!fs::is_directory(*it) && BoolConsistency(hidden_file, IsHidden(*it))){
				auto leaf = sig::Split(it->path().wstring(), L"\\").back();
				if (extension.empty()) result.push_back(leaf);
				else{
					auto query = L".*(" + RegexEscaper(extension) + L")";
					auto ext = RegexSearch(leaf, std::wregex(query));
					if (ext && (*ext)[0][1] == extension) result.push_back(leaf);
				}
			}
		}
		return Just<ResultType>::type(std::move(result));
#else
		std::cout << "this OS is not support. please include boost if any." << std::endl; 
		assert(false);
#endif
	}


	//指定ディレクトリにあるフォルダ名を取得
	//directry_pass：調べたいディレクトリのパス
	//hidden_file：true->隠しファイルのみ, false->非隠しファイルのみ (Windows, Linux環境のみ)
	//読み込み失敗: return -> nothing or empty-vector
	inline auto GetFolderNames(FileString const& directory_pass, bool hidden_file) ->Just<std::vector<std::wstring>>::type
	{
		typedef std::vector<std::wstring> ResultType;
		ResultType result;

#if SIG_MSVC_ENV
		WIN32_FIND_DATA fd;
		auto pass = DirpassTailModify(directory_pass, true) + L"*";
		auto hFind = FindFirstFile(pass.c_str(), &fd);

		if (hFind == INVALID_HANDLE_VALUE){
			return Nothing(std::move(result));
		}
		else{
			do{
				if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && BoolConsistency(hidden_file, (fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)))
				{
					auto tmp = std::wstring(fd.cFileName);
					if(tmp != L"." && tmp != L"..") result.push_back(tmp);
				}
			} while (FindNextFile(hFind, &fd));

			FindClose(hFind);
			return Just<ResultType>::type(std::move(result));
		}
#elif SIG_ENABLE_BOOST
		auto IsHidden = [](fs::path const& p){
			auto name = p.filename();
			if (name.c_str()[0] == '.' && name != ".." &&	name != ".") return true;
			else return false;
		};

		fs::directory_iterator end;
		for (fs::directory_iterator it(directory_pass); it != end; ++it)
		{
			if (fs::is_directory(*it) && BoolConsistency(hidden_file, IsHidden(*it))){
				result.push_back(sig::Split(it->path().wstring(), L"\\").back());
			}
		}
		return Just<ResultType>::type(std::move(result));
#else
		std::cout << "this OS is not support. please include boost if any." << std::endl; 
		assert(false);
#endif
	}


	//overwrite：上書き, append：末尾追記
	enum class WriteMode{ overwrite, append };

	//ファイル内容の初期化
	inline void FileClear(FileString const& file_pass)
	{
		std::ofstream ofs(file_pass);
		ofs << "";
	}


	//-- Save Text

	template <class S>
	void SaveLine(S const& src, typename FStreamSelector<TString<S>>::ofstream& ofs)
	{
		ofs << src << std::endl;
	}

	template <class C>
	void SaveLine(C const& src, typename FStreamSelector<typename container_traits<C>::value_type>::ofstream& ofs)
	{
		typename FStreamSelector<typename container_traits<C>::value_type>::ostreambuf_iterator streambuf_iter(ofs);
		for (auto const& str : src){
			std::copy(str.begin(), str.end(), streambuf_iter);
			streambuf_iter = '\n';
		}
	}

	//ファイルへ1行ずつ保存
	//src: 保存対象
	//file_pass: 保存先のディレクトリとファイル名（フルパス）
	//open_mode: 上書き(overwrite) or 追記(append)
	template <class S, typename std::enable_if<container_traits<TString<S>>::is_string>::type*& = enabler>
	void SaveLine(S src, FileString const& file_pass, WriteMode mode = WriteMode::overwrite)
	{
		static bool first = true;
		if (first){
			std::locale::global(std::locale(""));
			first = false;
		}

		auto const open_mode = mode == WriteMode::overwrite ? std::ios::out : std::ios::out | std::ios::app;
		typename FStreamSelector<TString<S>>::ofstream ofs(file_pass, open_mode);
		SaveLine(src, ofs);
	}
	//まとめて保存 (シーケンスな動的コンテナのみ対応)
	template <class C, typename std::enable_if<!container_traits<C>::is_string>::type*& = enabler>
	void SaveLine(C const& src, FileString const& file_pass, WriteMode mode = WriteMode::overwrite)
	{
		static bool first = true;
		if (first){
			std::locale::global(std::locale(""));
			first = false;
		}

		auto const open_mode = mode == WriteMode::overwrite ? std::ios::out : std::ios::out | std::ios::app;
		typename FStreamSelector<typename container_traits<C>::value_type>::ofstream ofs(file_pass, open_mode);
		SaveLine(src, ofs);
	}

	//保存するデータが数値の場合
	//デフォルトでは1要素1行で保存（delimiterで変更可）
	template <class C>
	void SaveNum(C const& src, FileString const& file_pass, WriteMode mode = WriteMode::overwrite, std::string delimiter = "\n")
	{
		SaveLine(CatStr(src, delimiter), file_pass, mode);
	}


	//-- Read Text

	template <class R>
	using IfsSelector = typename std::conditional<std::is_same<R, std::string>::value, std::ifstream, std::wifstream>::type;

	//conv: 読み込んだ文字列から任意型Rへの変換関数(文字列 -> 数値型へはReadNumを推奨) string or wstring -> R
	template <class C, class R = typename container_traits<C>::value_type>
	void ReadLine(C& empty_dest,
		IfsSelector<R>& ifs,
		std::function< R(typename std::conditional<std::is_same<R, std::string>::value, std::string, std::wstring>::type) > const& conv = nullptr)
	{
		static_assert(std::is_same<R, typename container_traits<C>::value_type>::value, "error in ReadLine: R and C::value_type don't match");

		typename std::conditional<std::is_same<R, std::string>::value, std::string, std::wstring>::type line;

		static bool first = true;
		if (first){
			std::locale::global(std::locale(""));
			first = false;
		}

		while (ifs && std::getline(ifs, line)){
			conv ? container_traits<C>::add_element(empty_dest, conv(std::move(line))) : container_traits<C>::add_element(empty_dest, std::move(line));
		}
	}

	template <class C, class R = typename container_traits<C>::value_type>
	void ReadLine(C& empty_dest,
		FileString const& file_pass,
		std::function< R(typename std::conditional<std::is_same<R, std::string>::value, std::string, std::wstring>::type)> const& conv = nullptr)
	{
		IfsSelector<R> ifs(file_pass);
		if (!ifs){
			FileOpenErrorPrint(file_pass);
			return;
		}
		ReadLine(empty_dest, ifs, conv);
	}

	template <class C, class R = typename container_traits<C>::value_type>
	void ReadNum(C& empty_dest, FileString const& file_pass, std::string delimiter = "")
	{
		static_assert(std::is_same<R, typename container_traits<C>::value_type>::value, "error in ReadNum: R and C::value_type don't match");

		IfsSelector<std::string> ifs(file_pass);
		std::string line;

		static bool first = true;
		if (first){
			std::locale::global(std::locale(""));
			first = false;
		}

		if (!ifs){
			FileOpenErrorPrint(file_pass);
			return;
		}
		while (ifs && std::getline(ifs, line)){
			auto split = Split(line, delimiter);
			for (auto v : split) container_traits<C>::add_element(empty_dest, Str2NumSelector<R>()(v));
		}
	}

	//読み込み失敗: return -> nothing
	template <class R, class C = std::vector<R>>
	auto ReadLine(IfsSelector<R>& ifs) ->typename Just<C>::type
	{
		C tmp;
		ReadLine(tmp, ifs);
		return tmp.size() ? typename Just<C>::type(std::move(tmp)) : Nothing(std::move(tmp));
	}

	template <class R, class C = std::vector<R>>
	auto ReadLine(FileString const& file_pass) ->typename Just<C>::type
	{
		IfsSelector<R> ifs(file_pass);
		if (!ifs){
			FileOpenErrorPrint(file_pass);
			return Nothing(C());
		}
		return ReadLine<R, C>(ifs);
	}

	template <class R, class C = std::vector<R>>
	auto ReadNum(FileString const& file_pass, std::string delimiter = "") ->typename Just<C>::type
	{
		C tmp;
		ReadNum(tmp, file_pass, delimiter);
		return tmp.size() ? typename Just<C>::type(std::move(tmp)) : Nothing(std::move(tmp));
	}

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