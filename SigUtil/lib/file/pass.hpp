/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_PASS_HPP
#define SIG_UTIL_PASS_HPP

#include "../helper/helper_modules.hpp"
#include "../helper/maybe.hpp"


#if SIG_MSVC_ENV
#define NOMINMAX
	#include <windows.h>
#elif SIG_USE_BOOST_FILESYSTEM
	#include <boost/filesystem.hpp>
	namespace fs = boost::filesystem;
#endif


/// \file pass.hpp ファイル・フォルダに関する情報取得

namespace sig
{

/// ディレクトリ・ファイルパスの末尾に'/'or'\'があるかチェックし、付けるか外すかどうかを指定して反映
/**
	\param directory_pass チェックするパスの文字列
	\param has_slash 末尾に'/'or'\'を付けるかどうか

	\return 修正したパスの文字列

	\code
	auto pass = SIG_TO_FPSTR("../SigUtil/example");

	auto mpass = modify_dirpass_tail(pass, true);	// "../SigUtil/example/"
	\endcode
*/
inline auto modify_dirpass_tail(FilepassString const& directory_pass, bool const has_slash) ->FilepassString
{
	if (directory_pass.empty()) return directory_pass;

	auto tail = directory_pass.back();

	if (has_slash){
		//付ける場合
		if (tail == '/' || tail == '\\') return directory_pass;
		else return directory_pass + SIG_TO_FPSTR("/");
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


/// 指定ディレクトリにあるファイル名を取得
/**
	\pre VisualStudio環境 または boostのインクルード
	
	\param directry_pass 調べたいディレクトリのパス
	\param hidden_file true:隠しファイルのみ, false:非隠しファイルのみ (Windows, Linux環境のみ)
	\param extension [option] 拡張子指定

	\return ファイル名の一覧
*/
inline auto get_file_names(
	FilepassString const& directory_pass,
	bool hidden_file,
	std::wstring extension = L""
)
	->Maybe<std::vector<std::wstring>>
{
	using ResultType = std::vector<std::wstring>;

	ResultType result;

#if SIG_MSVC_ENV
	WIN32_FIND_DATA fd;
	auto query = extension.empty() ? L"?*" : L"*" + extension;
	auto pass = modify_dirpass_tail(directory_pass, true) + query;
	auto hFind = FindFirstFile(pass.c_str(), &fd);

	if (hFind == INVALID_HANDLE_VALUE){
		return Nothing(std::move(result));
	}
	else{
		do{
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && Consistency(hidden_file, (fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)))
			{
				result.push_back(std::wstring(fd.cFileName));
			}
		} while (FindNextFile(hFind, &fd));

		FindClose(hFind);
		return Just<ResultType>(std::move(result));
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
		if (!fs::is_directory(*it) && Consistency(hidden_file, IsHidden(*it))){
			auto leaf = sig::split(it->path().wstring(), L"/").back();
			if (extension.empty()) result.push_back(leaf);
			else{
				auto query = L".*(" + escape_regex(extension) + L")";
				auto ext = sig::regex_search(leaf, SIG_WRegex(query));
				if (is_container_valid(ext) && fromJust(ext)[0][1] == extension) result.push_back(leaf);
			}
		}
	}
	return Just<ResultType>::type(std::move(result));
#else
	std::cout << "I don't support this envirnment which is default. please include boost if any." << std::endl; 
	assert(false);
#endif
}


/// 指定ディレクトリにあるフォルダ名を取得
/**
	\pre VisualStudio環境 または boostのインクルード

	\param directry_pass 調べたいディレクトリのパス
	\param hidden_file true:隠しファイルのみ, false:非隠しファイルのみ (Windows, Linux環境のみ)
	
	\return フォルダ名の一覧
*/
inline auto get_folder_names(
	FilepassString const& directory_pass,
	bool hidden_file
)
	->Maybe<std::vector<std::wstring>>
{
	using ResultType = std::vector<std::wstring>;

	ResultType result;

#if SIG_MSVC_ENV
	WIN32_FIND_DATA fd;
	auto pass = modify_dirpass_tail(directory_pass, true) + L"*";
	auto hFind = FindFirstFile(pass.c_str(), &fd);

	if (hFind == INVALID_HANDLE_VALUE){
		return Nothing(std::move(result));
	}
	else{
		do{
			if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && Consistency(hidden_file, (fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)))
			{
				auto tmp = std::wstring(fd.cFileName);
				if(tmp != L"." && tmp != L"..") result.push_back(tmp);
			}
		} while (FindNextFile(hFind, &fd));

		FindClose(hFind);
		return Just<ResultType>(std::move(result));
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
		if (fs::is_directory(*it) && Consistency(hidden_file, IsHidden(*it))){
			result.push_back(sig::split(it->path().wstring(), L"/").back());
		}
	}
	return Just<ResultType>(std::move(result));
#else
	std::cout << "I don't support this envirnment which is default. please include boost if any." << std::endl; 
	assert(false);
#endif
}

}

#endif
