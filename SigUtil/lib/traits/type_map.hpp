/*
Copyright(c) 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef _SIG_UTIL_TYPEMAP_HPP
#define _SIG_UTIL_TYPEMAP_HPP

#include "../sigutil.hpp"

namespace sig
{
namespace impl
{
// out/in type to stream (mainly filestream)
template <class FILE_STRING>
struct FStreamSelector{
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
template<> struct FStreamSelector<wchar_t const*>{
	typedef std::wofstream ofstream;
	typedef std::wifstream ifstream;
	typedef std::ostreambuf_iterator<wchar_t> ostreambuf_iterator;
	typedef std::istreambuf_iterator<wchar_t> istreambuf_iterator;
};

// out/in type to stringstream
template <class T> struct SStreamSelector{
	typedef std::ostringstream ostringstream;
	typedef std::istringstream istringstream;
	typedef std::string string;
};
template<> struct SStreamSelector<std::wstring>{
	typedef std::wostringstream ostringstream;
	typedef std::wistringstream istringstream;
	typedef std::wstring string;
};
template<> struct SStreamSelector<wchar_t const*>{
	typedef std::wostringstream ostringstream;
	typedef std::wistringstream istringstream;
	typedef std::wstring string;
};

// string to Num
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

// convert string-related type T into STL string type
template <class T>
struct StringId{ typedef T type; static const bool value = false; };
template <>
struct StringId<std::string>{ typedef std::string type; static const bool value = true; };
template <>
struct StringId<char*>{ typedef std::string type; static const bool value = true; };
template <>
struct StringId<char const*>{ typedef std::string type; static const bool value = true; };
template <>
struct StringId<std::wstring>{ typedef std::wstring type; static const bool value = true; };
template <>
struct StringId<wchar_t*>{ typedef std::wstring type; static const bool value = true; };
template <>
struct StringId<wchar_t const*>{ typedef std::wstring type; static const bool value = true; };

// preset to use StringId 
template <class T>
using TString = typename StringId<typename std::decay<T>::type>::type;

// combine std::conditional and std::same
template <class T1, class T2, class TrueT, class FalseT>
struct SameIf{
	using type = typename std::conditional<std::is_same<T1, T2>::value, TrueT, FalseT>::type;
};
}	// impl
}
#endif
