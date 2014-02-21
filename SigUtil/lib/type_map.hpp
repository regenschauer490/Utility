/*
Copyright(c) 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef _SIG_UTILUTIL_TYPEMAP_HPP
#define _SIG_UTILUTIL_TYPEMAP_HPP

#include "sigutil.hpp"

namespace sig{

	//out/in type to stream (mainly filestream)
	template <class FILE_STRING> struct FStreamSelector{};
	template<> struct FStreamSelector<std::string>{
		typedef std::ofstream ofstream;
		typedef std::ifstream ifstream;
		typedef std::ostreambuf_iterator<char> ostreambuf_iterator;
		typedef std::istreambuf_iterator<char> istreambuf_iterator;
	};
	template<> struct FStreamSelector<std::wstring>{
		typedef std::wofstream ofstream;
		typedef std::wifstream ifstream;
		typedef std::ostreambuf_iterator<wchar_t> ostreambuf_iterator;
		typedef std::istreambuf_iterator<wchar_t> istreambuf_iterator;
	};

	//out/in type to stringstream
	template <class T> struct SStreamSelector{
		typedef std::ostringstream ostringstream;
		typedef std::istringstream istringstream;
	};
	template<> struct SStreamSelector<std::wstring>{
		typedef std::wostringstream ostringstream;
		typedef std::istringstream istringstream;
	};
	template<> struct SStreamSelector<wchar_t const*>{
		typedef std::wostringstream ostringstream;
		typedef std::istringstream istringstream;
	};

	//string to Num
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

	//identify whether T is string-related type and convert to STL string type
	template <class T>
	struct StringId{};
	template <>
	struct StringId<std::string>{ typedef std::string type; };
	template <>
	struct StringId<char*>{ typedef std::string type; };
	template <>
	struct StringId<char const*>{ typedef std::string type; };
	template <>
	struct StringId<std::wstring>{ typedef std::wstring type; };
	template <>
	struct StringId<wchar_t*>{ typedef std::wstring type; };
	template <>
	struct StringId<wchar_t const*>{ typedef std::wstring type; };

	//preset to use StringId 
	template <class T>
	using TString = typename StringId<typename std::decay<T>::type>::type;

	//string type to associated in regex type
	template <class T>
	struct Str2RegexSelector{};
	template <>
	struct Str2RegexSelector<std::string>{
		typedef SIG_Regex regex;
		typedef SIG_SMatch smatch;
	};
	template <>
	struct Str2RegexSelector<std::wstring>{
		typedef SIG_WRegex regex;
		typedef SIG_WSMatch smatch;
	};
}
#endif