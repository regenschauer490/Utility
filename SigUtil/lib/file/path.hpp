/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIGUTIL_PATH_HPP
#define SIGUTIL_PATH_HPP

#include "../helper/helper_modules.hpp"
#include "../helper/maybe.hpp"
#include "../string/manipulate.hpp"
#include <fstream>

#if SIG_MSVC_ENV
#define NOMINMAX
	#include <windows.h>
#elif SIG_USE_BOOST_FILESYSTEM
	#include "../string/regex.hpp"
	#include <boost/filesystem.hpp>
	namespace fs = boost::filesystem;
#endif


/// \file path.hpp ファイル・フォルダに関する情報取得

namespace sig
{

/// ディレクトリ・ファイルパスの末尾に'/'or'\'があるかチェックし、付けるか外すかどうかを指定して反映
/**
	\param directory_path チェックするパスの文字列
	\param has_slash 末尾に'/'or'\'を付けるかどうか

	\return 修正したパスの文字列

	\code
	auto path = SIG_TO_FPSTR("../SigUtil/example");

	auto mpass = modify_dirpath_tail(path, true);	// "../SigUtil/example/"
	\endcode
*/
inline auto modify_dirpath_tail(FilepassString const& directory_path, bool const has_slash) ->FilepassString
{
	if (directory_path.empty()) return directory_path;

	auto tail = directory_path.back();

	if (has_slash){
		//付ける場合
		if (tail == '/' || tail == '\\') return directory_path;
		else return directory_path + SIG_TO_FPSTR("/");
	}
	else{
		if (tail != '/' && tail != '\\') return directory_path;
		else{
			auto tmp = directory_path;
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
	FilepassString const& directory_path,
	bool hidden_file,
	FilepassString const& extension = SIG_TO_FPSTR("")
)
	->Maybe<std::vector<FilepassString>>
{
	using ResultType = std::vector<FilepassString>;

	ResultType result;

#if SIG_MSVC_ENV
	WIN32_FIND_DATA fd;
	auto query = extension.empty() ? L"?*" : L"*" + extension;
	auto path = modify_dirpath_tail(directory_path, true) + query;
	auto hFind = FindFirstFile(path.c_str(), &fd);

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
#elif SIG_USE_BOOST
	auto IsHidden = [](fs::path const& p){
		auto name = p.filename();
		if (name.c_str()[0] == '.' && name != ".." &&	name != ".") return true;
		else return false;
	};

	fs::directory_iterator end;
	for (fs::directory_iterator it(directory_path); it != end; ++it)
	{
		if (!fs::is_directory(*it) && Consistency(hidden_file, IsHidden(*it))){
			auto leaf = sig::split(it->path().string(), "/").back();
			if (extension.empty()) result.push_back(leaf);
			else{
				auto query = ".*(" + sig::escape_regex(extension) + ")";
				auto ext = sig::regex_search(leaf, SIG_Regex(query));
				if (isJust(ext) && fromJust(ext)[0][1] == extension) result.push_back(leaf);
			}
		}
	}
	return Just<ResultType>(std::move(result));
#else
	std::cout << "This library doesn't support this envirnment which is default. please include boost if any." << std::endl; 
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
	FilepassString const& directory_path,
	bool hidden_file
)
	->Maybe<std::vector<FilepassString>>
{
	using ResultType = std::vector<FilepassString>;

	ResultType result;

#if SIG_MSVC_ENV
	WIN32_FIND_DATA fd;
	auto path = modify_dirpath_tail(directory_path, true) + L"*";
	auto hFind = FindFirstFile(path.c_str(), &fd);

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
#elif SIG_USE_BOOST
	auto IsHidden = [](fs::path const& p){
		auto name = p.filename();
		if (name.c_str()[0] == '.' && name != ".." &&	name != ".") return true;
		else return false;
	};

	fs::directory_iterator end;
	for (fs::directory_iterator it(directory_path); it != end; ++it)
	{
		if (fs::is_directory(*it) && Consistency(hidden_file, IsHidden(*it))){
			result.push_back(sig::split(it->path().string(), "/").back());
		}
	}
	return Just<ResultType>(std::move(result));
#else
	std::cout << "This library doesn't support this envirnment which is default. please include boost if any." << std::endl; 
	assert(false);
#endif
}


/// 指定ファイルが存在するか確認
/**
\pre VisualStudio環境 または boostのインクルード

\param file_path 調べたいファイルのパス

\return 存在する場合:true, 存在しない場合:false
*/
inline bool file_exists(FilepassString const& file_path)
{
#if SIG_MSVC_ENV
	WIN32_FIND_DATA FindFileData;
	HANDLE handle = FindFirstFile(file_path.c_str(), &FindFileData);
	bool found = handle != INVALID_HANDLE_VALUE;
	if (found) FindClose(handle);
	return found;
#elif SIG_USE_BOOST
	return boost::filesystem::exists(file_path);
#else
	std::cout << "This library doesn't support this envirnment which is default. please include boost if any." << std::endl;
	assert(false);
#endif
}


/// 指定ファイルの内容の初期化
/**
	\param file_path 初期化したいファイルのパス

	\code
	const auto dir = modify_dirpath_tail( SIG_TO_FPSTR("./src"), true);
	const auto fpass = dir + SIG_TO_FPSTR("test.txt");

	clear_file(fpass);
	\endcode
*/
inline void clear_file(FilepassString const& file_path)
{
	std::ofstream ofs(file_path);
	ofs << "";
}


/// 指定ファイルを削除
inline bool remove_file(FilepassString const& file_path)
{
#if SIG_MSVC_ENV
	if (sig::file_exists(file_path)){
		return DeleteFile(file_path.c_str());
	}
	else return false;
#elif SIG_USE_BOOST

#else
	std::cout << "This library doesn't support this envirnment which is default. please include boost if any." << std::endl;
	assert(false);
#endif
}


/// フォルダを新規作成
/**
*/
inline bool make_directory(FilepassString const& directory_path, bool make_intermediate_directories)
{
#if SIG_MSVC_ENV
	const auto mkdir = [](FilepassString const& dpath){
		return CreateDirectory(dpath.c_str(), NULL);
	};

	const auto ckrcr = [](){
		return GetLastError() == ERROR_PATH_NOT_FOUND;
	};
#elif SIG_USE_BOOST
	const auto mkdir = [](FilepassString const& dpath){
		return boost::filesystem::create_directory(boost::filesystem::path(dpath)));
	};
	const auto ckrcr = [](){
		return ;
	}
#else
	const auto mkdir = [](FilepassString const& dpath){return false; };
	const auto ckrcr = [](){ return false; };
	std::cout << "This library doesn't support this envirnment which is default. please include boost if any." << std::endl;
	assert(false);
#endif
	auto result = mkdir(directory_path);
	if (!result && make_intermediate_directories && ckrcr())
	{
		auto spl = [&](){
			auto tmp = split(directory_path, SIG_TO_FPSTR("\\"));
			if (tmp.size() > 1) return tmp;
			else return split(directory_path, SIG_TO_FPSTR("/"));
		}();
		spl.pop_back();
		if (make_directory(cat(spl, SIG_TO_FPSTR("\\")), make_intermediate_directories)){
			result = mkdir(directory_path);
		}
	}
	return result;
}

	
/// 指定フォルダの削除
inline bool remove_directory(FilepassString const& directory_path)
{
#if SIG_MSVC_ENV
	const auto rmdir = [](FilepassString const& path){
		return RemoveDirectory(path.c_str());
	};

	auto vfolders = get_folder_names(directory_path, false);
	auto hfolders = get_folder_names(directory_path, true);
	if (isJust(vfolders)){
		for (auto const& subdir : *vfolders){
			const auto path = directory_path + SIG_TO_FPSTR("/") + subdir;
			if (!remove_directory(path)) return false;
		}
	}
	if (isJust(hfolders)){
		for (auto const& subdir : *hfolders){
			const auto path = directory_path + SIG_TO_FPSTR("/") + subdir;
			if (!remove_directory(path)) return false;
		}
	}

	auto vfiles = get_file_names(directory_path, false);
	auto hfiles = get_file_names(directory_path, true);
	if (isJust(vfiles)){
		for (auto const& file : *vfiles){
			if(!remove_file(file)) return false;
		}
	}
	if (isJust(hfiles)){
		for (auto const& file : *hfiles){
			if(!remove_file(file)) return false;
		}
	}
	return rmdir(directory_path);

#elif SIG_USE_BOOST
	return boost::filesystem::remove_all(boost::filesystem::path(dir_path));
#else
	std::cout << "This library doesn't support this envirnment which is default. please include boost if any." << std::endl;
	assert(false);
#endif
}

}

#endif
