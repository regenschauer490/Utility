#ifndef SIG_UTIL_SET_HPP
#define SIG_UTIL_SET_HPP

#include "sigutil.hpp"
#include "tool.hpp"

namespace sig
{

#if SIG_ENABLE_BOOST

	//条件に最適な値とそのIndexを探す.　comp(比較対象値, 暫定最小値)
	template < class T, class CMP, template < class T, class = std::allocator<T>> class Container >
	maybe<std::tuple<T, uint>> SearchIndex(Container<T> const& src, CMP comp)
	{
		if (src.empty()) return nothing;

		T val = src[0];
		uint index = 0;

		for (uint i = 0, size = src.size(); i < size; ++i){
			if (comp(src[i], val)){
				val = src[i];
				index = i;
			}
		}

		return std::make_tuple(val, index);
	}

#endif


	inline void Print(std::string const& text, char const* const delimiter = "\n")
	{
		std::cout << text << delimiter;
	}

	inline void Print(std::wstring const& text, wchar_t const* const delimiter = L"\n")
	{
		std::wcout << text << delimiter;
	}

	template < class T, template <class...> class Container, typename std::enable_if<!std::is_same<T, std::wstring>::value>::type*& = enabler>
	inline void Print(Container<T> const& container, char const* const delimiter = "\n")
	{
		std::copy(container.begin(), container.end(), std::ostream_iterator<T>(std::cout, delimiter));
	}

	template<template<class...> class Container>
	inline void Print(Container<std::wstring> const& container, wchar_t const* const delimiter = L"\n")
	{
		std::copy(container.begin(), container.end(), std::ostream_iterator<std::wstring>(std::wcout, delimiter));
	}


template <class CC1, class CC2>
auto GaleShapley(CC1 const& men_vec, CC2 const& women_vec)
{
	assert(men_vec.size() != women_vec.size());

	uint total = men_vec.size();
	std::vector<uint> keeping(total, total+1);	// 女性が 何位の男性と婚約しているか（初期値N+1）
	std::vector<std::tuple<bool, uint>> single_men(total, std::make_tuple(true, 0));	// 男性が tuple<独身であるか, 告白回数（希望相手女性の順位）>
	std::queue<uint> rest;	// 告発実行待ちの男性のキュー

	// 男性が女性に告白する。戻り値：婚約成功の可否
	auto propose = [&](const uint man, const uint woman) ->bool{
		if(keeping[woman] > total){
			// womanがまだ婚約していない場合
			keeping[woman] = man;
			std::get<0>(single_men[man]) = true;
			return true;
		}
		else{
			for(uint i=0; i<total; ++i){
				if(man == women_vec[woman][i]){
					if(i < keeping[woman]){
						// manがwomanの婚約相手よりも好感度が高い場合
						uint now_man = women_vec[woman][keeping[woman]];
						keeping[woman] = i;
						std::get<0>(single_men[man]) = true;
						// 現婚約者の婚約解消
						std::get<0>(single_men[now_man]) = false;
						rest.push(now_man);
						return true;
					}
					else{
						return false;
					}
				}
			}
		}
	};

	for(uint i=0; i<total; ++i) rest.push(i);

	while (!rest.empty()){
		const uint man = rest.front();
		const uint propose_woman = men_vec[man][std::get<1>(single_men[man])];
		++std::get<1>(single_men[man]);
		if (propose(man, propose_woman)) rest.pop();
	}
}
}

#endif