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
	//directry_pass：調べたいディレクトリのパス
	//hidden_file：true->隠しファイルのみ, false->非隠しファイルのみ (Windows, Linux環境のみ)
	//extension：拡張子指定(オプション)
	//読み込み失敗: return -> nothing or empty-vector
	inline auto GetFileNames(std::wstring const& directory_pass, bool hidden_file, std::wstring extension = L"") ->Just<std::vector<std::wstring>>::type
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
		static_asseet(false, "this OS is not support. please include boost if any.");
#endif
	}


	//指定ディレクトリにあるフォルダ名を取得
	//directry_pass：調べたいディレクトリのパス
	//hidden_file：true->隠しファイルのみ, false->非隠しファイルのみ (Windows, Linux環境のみ)
	//読み込み失敗: return -> nothing or empty-vector
	inline auto GetFolderNames(std::wstring const& directory_pass, bool hidden_file) ->Just<std::vector<std::wstring>>::type
	{
		typedef std::vector<std::wstring> ResultType;
		ResultType result;

#if SIG_WINDOWS_ENV
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
		static_asseet(false, "this OS is not support. please include boost if any.");
#endif
	}


	//for type map
	template <class FILE_STRING> struct OfsSelector{};
	template<> struct OfsSelector<char const*>{
		typedef std::ofstream fstream;
		typedef std::ostreambuf_iterator<char> fstreambuf_iter;
	};
	template<> struct OfsSelector<std::string>{
		typedef std::ofstream fstream;
		typedef std::ostreambuf_iterator<char> fstreambuf_iter;
	};
	template<> struct OfsSelector<wchar_t const*>{
		typedef std::wofstream fstream;
		typedef std::ostreambuf_iterator<wchar_t> fstreambuf_iter;
	};
	template<> struct OfsSelector<std::wstring>{
		typedef std::wofstream fstream;
		typedef std::ostreambuf_iterator<wchar_t> fstreambuf_iter;
	};

	template <class FILE_STRING> struct IfsSelector{};
	template<> struct IfsSelector<std::string>{
		typedef std::ifstream fstream;
		typedef std::istreambuf_iterator<char> fstreambuf_iter;
	};
	template<> struct IfsSelector<std::wstring>{
		typedef std::wifstream fstream;
		typedef std::istreambuf_iterator<wchar_t> fstreambuf_iter;
	};

	template <class NUM> struct Str2NumSelector{};
	template <> struct Str2NumSelector<int>{
		int operator()(std::string s){ return std::stoi(s); }
	};
	template <> struct Str2NumSelector<long>{
		long operator()(std::string s){ return std::stol(s); }
	};
	template <> struct Str2NumSelector<long long>{
		long long operator()(std::string s){ return std::stoll(s); }
	};
	template <> struct Str2NumSelector<unsigned int>{
		unsigned int operator()(std::string s){ return std::stoul(s); }
	};
	template <> struct Str2NumSelector<unsigned long>{
		unsigned long operator()(std::string s){ return std::stoul(s); }
	};
	template <> struct Str2NumSelector<unsigned long long>{
		unsigned long long operator()(std::string s){ return std::stoull(s); }
	};
	template <> struct Str2NumSelector<float>{
		float operator()(std::string s){ return std::stof(s); }
	};
	template <> struct Str2NumSelector<double>{
		double operator()(std::string s){ return std::stod(s); }
	};

	//overwrite：上書き, append：末尾追記
	enum class WriteMode{ overwrite, append };

	//ファイル内容の初期化
	inline void FileClear(std::wstring const& file_pass)
	{
		std::ofstream ofs(file_pass);
		ofs << "";
	}

	//-- Save Text

	template <class String>
	void SaveLine(String const& src, typename OfsSelector<TString<String>>::fstream& ofs)
	{
		ofs << src << std::endl;
	}

	template <class String, template <class T_, class = std::allocator<T_>> class Container >
	void SaveLine(Container<String> const& src, typename OfsSelector<String>::fstream& ofs)
	{
		typename OfsSelector<String>::fstreambuf_iter streambuf_iter(ofs);
		for (auto const& str : src){
			std::copy(str.begin(), str.end(), streambuf_iter);
			streambuf_iter = '\n';
		}
	}

	//ファイルへ1行ずつ保存
	//src: 保存対象
	//file_pass: 保存先のディレクトリとファイル名（フルパス）
	//open_mode: 上書き(overwrite) or 追記(append)
	template <class String>
	void SaveLine(String src, std::wstring const& file_pass, WriteMode mode = WriteMode::overwrite)
	{
		static bool first = true;
		if (first){
			std::locale::global(std::locale(""));
			first = false;
		}

		std::ios::open_mode const open_mode = mode == WriteMode::overwrite ? std::ios::out : std::ios::out | std::ios::app;
		typename OfsSelector<typename std::decay<String>::type>::fstream ofs(file_pass, open_mode);
		SaveLine(src, ofs);
	}
	//まとめて保存 (シーケンスな動的コンテナのみ対応)
	template <class String, template <class T_, class = std::allocator<T_>> class Container>
	void SaveLine(Container<String> const& src, std::wstring const& file_pass, WriteMode mode = WriteMode::overwrite)
	{
		static bool first = true;
		if (first){
			std::locale::global(std::locale(""));
			first = false;
		}

		std::ios::open_mode const open_mode = mode == WriteMode::overwrite ? std::ios::out : std::ios::out | std::ios::app;
		typename OfsSelector<String>::fstream ofs(file_pass, open_mode);
		SaveLine(src, ofs);
	}

	//保存するデータが数値の場合
	//デフォルトでは1要素1行で保存（delimiterで変更可）
	template <class Num, template <class T_, class = std::allocator<T_>> class Container>
	void SaveNum(Container<Num> const& src, std::wstring const& file_pass, WriteMode mode = WriteMode::overwrite, std::string delimiter = "\n")
	{
		SaveLine(CatStr(src, delimiter), file_pass, mode);
	}


	//-- Read Text

	template <class R, template <class T_, class = std::allocator<T_>> class Container = std::vector>
	void ReadLine(Container<R>& empty_dest, typename IfsSelector<R>::fstream& ifs, std::function< R(typename std::conditional<std::is_same<typename IfsSelector<R>::fstream, std::ifstream>::value, std::string, std::wstring>::type)> const& conv = nullptr)
	{
		typename std::conditional<std::is_same<typename IfsSelector<R>::fstream, std::ifstream>::value, std::string, std::wstring>::type line;

		while (ifs && getline(ifs, line)){
			conv ? empty_dest.push_back(conv(std::move(line))) : empty_dest.push_back(std::move(line));
		}
	}

	template <class R, template <class T_, class = std::allocator<T_>> class Container = std::vector>
	void ReadLine(Container<R>& empty_dest, std::wstring const& file_pass, std::function< R(typename std::conditional<std::is_same<typename IfsSelector<R>::fstream, std::ifstream>::value, std::string, std::wstring>::type)> const& conv = nullptr)
	{
		typename IfsSelector<R>::fstream ifs(file_pass);
		if (!ifs){
			wprintf(L"file open error: %s \n", file_pass);
			return;
		}
		ReadLine(empty_dest, ifs);
	}

	template <class Num, template <class T_, class = std::allocator<T_>> class Container = std::vector>
	void ReadNum(Container<Num>& empty_dest, std::wstring const& file_pass, std::string delimiter = "")
	{
		typename IfsSelector<std::string>::fstream ifs(file_pass);
		Str2NumSelector<Num> conv;
		std::string line;
		if (!ifs){
			wprintf(L"file open error: %s \n", file_pass);
			return;
		}
		while (ifs && getline(ifs, line)){
			auto split = Split(line, delimiter);
			for(auto v : split) empty_dest.push_back(conv(v));
		}
	}

#if SIG_ENABLE_BOOST

	//読み込み失敗: return -> maybe == nothing
	template <class String, template <class T_, class = std::allocator<T_>> class Container = std::vector>
	maybe<Container<String>> ReadLine(typename IfsSelector<String>::fstream& ifs)
	{
		typedef Container<String> R;
		R tmp;
		ReadLine(tmp, ifs);
		return tmp.size() ? maybe<R>(std::move(tmp)) : nothing;
	}

	template <class String, template <class T_, class = std::allocator<T_>> class Container = std::vector>
	maybe<Container<String>> ReadLine(std::wstring const& file_pass)
	{
		typename IfsSelector<String>::fstream ifs(file_pass);
		if (!ifs){
			std::wcout << L"file open error: " << file_pass << std::endl;
			return nothing;
		}
		return ReadLine<String, Container>(ifs);
	}

	template <class Num, template <class T_, class = std::allocator<T_>> class Container = std::vector>
	maybe<Container<Num>> ReadNum(std::wstring const& file_pass, std::string delimiter = "")
	{
		Container<Num> tmp;
		ReadNum<Num>(tmp, file_pass, delimiter);
		return tmp.size() ? maybe<Container<Num>>(std::move(tmp)) : nothing;
	}
#endif

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