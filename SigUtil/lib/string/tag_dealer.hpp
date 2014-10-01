/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_TAGDEALER_HPP
#define SIG_UTIL_TAGDEALER_HPP

#include "../functional.hpp"

namespace sig
{
namespace impl
{
	template <class S> struct space{};
	template <> struct space<std::string>{ std::string operator()() const{ return " "; } };
	template <> struct space<std::wstring>{ std::wstring operator()() const{ return L" "; } };
}

/// HTML風にタグをエンコード・デコードする
/**
例： <TAG>text<TAG>

\tparam S 扱う文字列型
*/
template <class S>
class TagDealer
{
	const S tel_;
	const S ter_;

public:
	/**
		左右それぞれの囲み文字を指定(ex. left = "<", right= ">")
	*/
	TagDealer(S tag_encloser_left, S tag_encloser_right) : tel_(tag_encloser_left), ter_(tag_encloser_right){};

	/**
		タグ付きの文字列にエンコード

		\param src 本文
		\param tag 囲み文字の内側のタグ文字
		\return タグ付き文字列
	*/
	S encode(S const& src, S const& tag) const{
		auto tag_str = tel_ + tag + ter_;
		return tag_str + src + tag_str;
	}

	/**
		指定したタグを探してデコード

		\param src タグ付きの文字列
		\param tag 囲み文字の内側のタグ文字
		\return 指定タグに囲まれた本文の文字列
	*/
	auto decode(S const& src, S const& tag) ->Just<S> const{
		auto tag_str = tel_ + tag + ter_;
		auto parse = split(impl::space<S>()() + src, tag_str);
		return parse.size() < 2 ? Nothing(S()) : Just<S>(parse[1]);
	}

	template < template < class T_, class Allocator = std::allocator<T_>> class Container >
	S encode(Container<S> const& src, Container<S> const& tag) const;

	template < template < class T_, class Allocator = std::allocator<T_>> class Container >
	auto decode(S const& src, Container<S> const& tag)->Just<Container<S>> const;

};

template <class S>
template < template < class T_, class Allocator = std::allocator<T_>> class Container >
S TagDealer<S>::encode(Container<S> const& src, Container<S> const& tag) const
{
	auto calc = zipWith([&](typename Container<S>::value_type s, typename Container<S>::value_type t){ return encode(s, t); }, src, tag);
	return std::accumulate(calc.begin(), calc.end(), S());
}

template <class S>
template < template < class T_, class Allocator = std::allocator<T_>> class Container >
auto TagDealer<S>::decode(S const& src, Container<S> const& tag) ->Just<Container<S>> const
{
	Container<S> result;
	for (auto const& e : tag){
		if (auto d = decode(src, e)) result.push_back(*d);
	}
	return result.empty() ? Nothing(S()) : Just<Container<S>>(std::move(result));
}

}

#endif