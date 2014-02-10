#ifndef __SIG_UTIL_TYPEMAP__
#define __SIG_UTIL_TYPEMAP_

#include "sigutil.hpp"

namespace sig{

	template <class FILE_STRING> struct OfsSelector{};
	template<> struct OfsSelector<std::string>{
		typedef std::ofstream fstream;
		typedef std::ostreambuf_iterator<char> streambuf_iterator;
	};
	template<> struct OfsSelector<std::wstring>{
		typedef std::wofstream fstream;
		typedef std::ostreambuf_iterator<wchar_t> streambuf_iterator;
	};
	/*
	template <class FILE_STRING> struct IfsSelector{};
	template<> struct IfsSelector<std::string>{
		typedef std::ifstream fstream;
		typedef std::istreambuf_iterator<char> streambuf_iterator;
	};
	template<> struct IfsSelector<std::wstring>{
		typedef std::wifstream fstream;
		typedef std::istreambuf_iterator<wchar_t> streambuf_iterator;
	};*/

	template <class T> struct SStreamSelector{
		typedef std::ostringstream stringstream;
	};
	template<> struct SStreamSelector<std::wstring>{
		typedef std::wostringstream stringstream;
	};
	template<> struct SStreamSelector<wchar_t const*>{
		typedef std::wostringstream stringstream;
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

}
#endif