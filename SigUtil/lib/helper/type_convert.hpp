/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef _SIG_UTIL_TYPE_CONVERT_HPP
#define _SIG_UTIL_TYPE_CONVERT_HPP

#include "type_traits.hpp"

namespace sig
{
namespace impl
{

template <class T>
struct remove_const_reference
{
	using type = typename std::remove_const<typename std::remove_reference<T>::type>::type;
};

template <size_t N1, size_t N2>
struct plus_t
{
	static const size_t value = N1 + N2;
};


// string type to associated in regex type
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

// string to each number type
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
struct StringId{ using type = void; static const bool value = false; };
template <>
struct StringId<std::string>{ using type = std::string; static const bool value = true; };
template <>
struct StringId<char*>{ using type = std::string; static const bool value = true; };
template <>
struct StringId<char const*>{ using type = std::string; static const bool value = true; };
template <>
struct StringId<std::wstring>{ using type = std::wstring; static const bool value = true; };
template <>
struct StringId<wchar_t*>{ using type = std::wstring; static const bool value = true; };
template <>
struct StringId<wchar_t const*>{ using type = std::wstring; static const bool value = true; };

// 
template <class T>
using string_t = typename StringId<
	typename remove_const_reference<
		typename std::decay<T>::type
	>::type
>::type;

template <class T>
struct is_string
{
	static const bool value = StringId<
		typename remove_const_reference<
			typename std::decay<T>::type
		>::type
	>::value;
};

// combine std::conditional and std::same
template <class T1, class T2, class TrueT, class FalseT>
struct SameIf{
	using type = typename std::conditional<std::is_same<T1, T2>::value, TrueT, FalseT>::type;
};

}	// impl

}	// sig
#endif
