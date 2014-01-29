#ifndef __SIG_UTIL_FILE__
#define __SIG_UTIL_FILE__

#include "string.hpp"
#include "sigutil.hpp"

#if SIG_WINDOWS_ENV
#include <windows.h>
#endif

#if SIG_ENABLE_BOOST
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;
#endif

/* 入出力関連 */

namespace sig{

	//ディレクトリ・ファイルパスの末尾に'/'or'\'があるかチェックし、付けるか外すかどうか指定
	inline std::wstring DirpassTailModify(std::wstring const& directory_pass, bool const has_slash)
	{
		if (directory_pass.empty()) return directory_pass;

		auto tail = directory_pass.back();

		if (has_slash){
			//付ける場合
			if (tail == '/' || tail == '\\') return directory_pass;
			else return (directory_pass + L"/");
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
	//args -> directry_pass: 調べたいディレクトリのパス, extension:拡張子指定(オプション)
	//読み込み失敗: return -> nothing or empty-vector
	inline auto GetFileNames(std::wstring const& directory_pass, std::wstring extension = L"") ->MaybeReturn<std::vector<std::wstring>>::type
	{
		typedef std::vector<std::wstring> ResultType;
		ResultType result;

#if SIG_WINDOWS_ENV
		WIN32_FIND_DATA fd;
		auto query = extension.empty() ? L"?*" : L"*" + extension;
		auto pass = DirpassTailModify(directory_pass, true) + query;
		auto hFind = FindFirstFile(pass.c_str(), &fd);

		if (hFind == INVALID_HANDLE_VALUE){
			return Nothing(std::move(result));
		}
		else{
			do{
				//フォルダは無視
				if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !(fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
				{
					result.push_back(std::wstring(fd.cFileName));
				}
			} while (FindNextFile(hFind, &fd));

			FindClose(hFind);
			return MaybeReturn<ResultType>::type(std::move(result));
		}
#elif SIG_ENABLE_BOOST
		fs::directory_iterator end;
		for (fs::directory_iterator it(directory_pass); it != end; ++it)
		{
			if (!fs::is_directory(*it)){
				auto leaf = sig::Split(it->path().wstring(), L"\\").back();
				if (extension.empty()) result.push_back(leaf);
				else{
					auto query = L".*(" + RegexEscaper(extension) + L")";
					auto ext = RegexSearch(leaf, std::wregex(query));
					if (ext && (*ext)[0][1] == extension) result.push_back(leaf);
				}
			}
		}
		return MaybeReturn<ResultType>::type(std::move(result));
#else
		static_asseet(false, "this OS is not support. please include boost if any.");
#endif
	}


	//指定ディレクトリにあるフォルダ名を取得
	//args -> directry_pass: 調べたいディレクトリのパス
	//読み込み失敗: return -> nothing or empty-vector
/*	inline auto GetFolderNames(std::wstring const& directory_pass) ->MaybeReturn<std::vector<std::wstring>>::type
	{
#if SIG_WINDOWS_ENV
		WIN32_FIND_DATA fd;
		auto pass = DirpassTailModify(directory_pass, true) + L"*." + extension;
		auto hFind = FindFirstFile(pass.c_str(), &fd);

		if (hFind == INVALID_HANDLE_VALUE){
			return Nothing(std::move(result));
		}
		else{
			do{
				//フォルダは無視
				if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !(fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
				{
					result.push_back(std::wstring(fd.cFileName));
				}
			} while (FindNextFile(hFind, &fd));

			FindClose(hFind);
			return MaybeReturn<ResultType>::type(std::move(result));
		}
#elif SIG_ENABLE_BOOST
		fs::directory_iterator end;
		for (fs::directory_iterator it(fs::current_path()); it != end; ++it)
		{
			if (!fs::is_directory(*it))	std::cout << it->path() << std::endl;
		}
#else
		static_asseet(false, "this OS is not support. please include boost if any.");
#endif
	}*/


	//for type map
	template <class FILE_STRING> struct OFS_SELECTION{};
	template<> struct OFS_SELECTION<char const*>{
		typedef std::ofstream fstream;
		typedef std::ostreambuf_iterator<char> fstreambuf_iter;
	};
	template<> struct OFS_SELECTION<std::string>{
		typedef std::ofstream fstream;
		typedef std::ostreambuf_iterator<char> fstreambuf_iter;
	};
	template<> struct OFS_SELECTION<wchar_t const*>{
		typedef std::wofstream fstream;
		typedef std::ostreambuf_iterator<wchar_t> fstreambuf_iter;
	};
	template<> struct OFS_SELECTION<std::wstring>{
		typedef std::wofstream fstream;
		typedef std::ostreambuf_iterator<wchar_t> fstreambuf_iter;
	};

	template <class FILE_STRING> struct IFS_SELECTION{};
	template<> struct IFS_SELECTION<std::string>{
		typedef std::ifstream fstream;
		typedef std::istreambuf_iterator<char> fstreambuf_iter;
	};
	template<> struct IFS_SELECTION<std::wstring>{
		typedef std::wifstream fstream;
		typedef std::istreambuf_iterator<wchar_t> fstreambuf_iter;
	};

	template <class NUM> struct S2NUM_SELECTION{};
	template <> struct S2NUM_SELECTION<int>{
		int operator()(std::string s){ return std::stoi(s); }
	};
	template <> struct S2NUM_SELECTION<long>{
		long operator()(std::string s){ return std::stol(s); }
	};
	template <> struct S2NUM_SELECTION<long long>{
		long long operator()(std::string s){ return std::stoll(s); }
	};
	template <> struct S2NUM_SELECTION<unsigned int>{
		unsigned int operator()(std::string s){ return std::stoul(s); }
	};
	template <> struct S2NUM_SELECTION<unsigned long>{
		unsigned long operator()(std::string s){ return std::stoul(s); }
	};
	template <> struct S2NUM_SELECTION<unsigned long long>{
		unsigned long long operator()(std::string s){ return std::stoull(s); }
	};
	template <> struct S2NUM_SELECTION<float>{
		float operator()(std::string s){ return std::stof(s); }
	};
	template <> struct S2NUM_SELECTION<double>{
		double operator()(std::string s){ return std::stod(s); }
	};

	enum class WriteMode{ overwrite, append };


	inline void RemakeFile(std::wstring const& file_pass)
	{
		std::ofstream ofs(file_pass);
		ofs << "";
	}

	//-- Save Text

	template <class String>
	inline void SaveLine(String const& src, typename OFS_SELECTION<String>::fstream& ofs)
	{
		ofs << src << std::endl;
	}

	template <class String>
	inline void SaveLine(std::vector<String> const& src, typename OFS_SELECTION<String>::fstream& ofs)
	{
		typename OFS_SELECTION<String>::fstreambuf_iter streambuf_iter(ofs);
		for (auto const& str : src){
			std::copy(str.begin(), str.end(), streambuf_iter);
			streambuf_iter = '\n';
		}
	}

	//ファイルへ1行ずつ保存
	//args -> src: 保存対象, file_pass: 保存先のディレクトリとファイル名（フルパス）, open_mode: 上書き(overwrite) or 追記(append)
	template <class String>
	inline void SaveLine(String src, std::wstring const& file_pass, WriteMode mode)
	{
		static bool first = true;
		if (first){
			std::locale::global(std::locale(""));
			first = false;
		}

		std::ios::open_mode const open_mode = mode == WriteMode::overwrite ? std::ios::out : std::ios::out | std::ios::app;
		typename OFS_SELECTION<typename std::decay<String>::type>::fstream ofs(file_pass, open_mode);
		SaveLine(src, ofs);
	}
	template <class String>
	inline void SaveLine(std::vector<String> const& src, std::wstring const& file_pass, WriteMode mode)
	{
		static bool first = true;
		if (first){
			std::locale::global(std::locale(""));
			first = false;
		}

		std::ios::open_mode const open_mode = mode == WriteMode::overwrite ? std::ios::out : std::ios::out | std::ios::app;
		typename OFS_SELECTION<String>::fstream ofs(file_pass, open_mode);
		SaveLine(src, ofs);
	}

	template <class Num>
	inline void SaveLineNum(std::vector<Num> const& src, std::wstring const& file_pass, WriteMode mode, std::string delimiter = "\n")
	{
		SaveLine(String::CatStr(src, delimiter), file_pass, mode);
	}


	//-- Read Text

	template <class R>
	inline void ReadLine(std::vector<R>& empty_dest, typename IFS_SELECTION<R>::fstream& ifs, std::function< R(typename std::conditional<std::is_same<typename IFS_SELECTION<R>::fstream, std::ifstream>::value, std::string, std::wstring>::type)> const& conv = nullptr)
	{
		typename std::conditional<std::is_same<typename IFS_SELECTION<R>::fstream, std::ifstream>::value, std::string, std::wstring>::type line;

		while (ifs && getline(ifs, line)){
			conv ? empty_dest.push_back(conv(std::move(line))) : empty_dest.push_back(std::move(line));
		}
	}

	template <class R>
	inline void ReadLine(std::vector<R>& empty_dest, std::wstring const& file_pass, std::function< R(typename std::conditional<std::is_same<typename IFS_SELECTION<R>::fstream, std::ifstream>::value, std::string, std::wstring>::type)> const& conv = nullptr)
	{
		typename IFS_SELECTION<R>::fstream ifs(file_pass);
		if (!ifs){
			wprintf(L"file open error: %s \n", file_pass);
			return;
		}
		ReadLine(empty_dest, ifs);
	}

	template <class Num>
	inline void ReadLineNum(std::vector<Num>& empty_dest, std::wstring const& file_pass)
	{
		typename IFS_SELECTION<std::string>::fstream ifs(file_pass);
		S2NUM_SELECTION<Num> conv;
		std::string line;
		if (!ifs){
			wprintf(L"file open error: %s \n", file_pass);
			return;
		}
		while (ifs && getline(ifs, line))
			empty_dest.push_back(conv(std::move(line)));
	}

#if SIG_ENABLE_BOOST

	//読み込み失敗: return -> maybe == nothing
	template <class String>
	inline maybe<std::vector<String>> ReadLine(typename IFS_SELECTION<String>::fstream& ifs)
	{
		typedef std::vector<String> R;
		R tmp;
		ReadLine(tmp, ifs);
		return tmp.size() ? maybe<R>(std::move(tmp)) : nothing;
	}

	template <class String>
	inline maybe<std::vector<String>> ReadLine(std::wstring const& file_pass)
	{
		typename IFS_SELECTION<String>::fstream ifs(file_pass);
		if (!ifs){
			std::wcout << L"file open error: " << file_pass << std::endl;
			return nothing;
		}
		return ReadLine<String>(ifs);
	}

	template <class Num>
	inline maybe<std::vector<Num>> ReadLineNum(std::wstring const& file_pass)
	{
		std::vector<Num> tmp;
		ReadLineNum<Num>(tmp, file_pass);
		return tmp.size() ? maybe<std::vector<Num>>(std::move(tmp)) : nothing;
	}

#endif

	//csvで保存
	template <class Num>
	inline void SaveCSV(std::vector<std::vector<Num>> const& data, std::vector<std::string> const& row_names, std::vector<std::string> const& col_names, std::wstring const& out_fullpass)
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
}

#endif