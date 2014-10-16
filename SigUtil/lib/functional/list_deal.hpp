/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_LIST_DEAL_HPP
#define SIG_UTIL_LIST_DEAL_HPP

#include "../helper/helper.hpp"
#include "../helper/container_helper.hpp"

/// \file list_deal.hpp リスト(コンテナ)操作関数

namespace sig
{
/// 値を複製したコンテナを返す
// uint -> a -> [a]
template <class T, class C = std::vector<T>>
C replicate(uint n, T const& value)
{
	C result;
	for (uint i = 0; i<n; ++i) impl::container_traits<C>::add_element(result, value);
	return result;
}

// [a] -> [a]
// コンテナの要素を逆転させたコンテナを返す
template <class C>
C reverse(C const& list)
{
	C result = list;
	std::reverse(std::begin(result), std::end(result));
	return result;
}

// [a] -> [a] -> [a]
// コンテナの結合
template <class C>
C merge(C const& list1, C const& list2)
{
	auto result = list1;
	impl::container_traits<C>::concat(result, list2);
	return result;
}

// [a] -> [a] -> [a]
// コンテナの結合 (tamplateパラメータCで結合後のコンテナの種類を指定)
template <class C, class C1, class C2>
C merge(C1 const& list1, C2 const& list2)
{
	C result;
	for (auto v : list1) impl::container_traits<C>::add_element(result, v);
	for (auto v : list2) impl::container_traits<C>::add_element(result, v);
	return result;
}

// uint -> [a] -> [a]
// コンテナの先頭からn個を取り出したコンテナを返す (順序の無いコンテナでは実装依存)
template <class C>
C take(uint n, C const& list)
{
	C result;
	uint i = 0;
	for (auto it = std::begin(list); i < n; ++i, ++it) impl::container_traits<C>::add_element(result, *it);
	return result;
}

// uint -> [a] -> [a]
// コンテナの先頭からn個を削除したコンテナを返す (順序の無いコンテナでは実装依存)
template <class C>
C drop(uint n, C const& list)
{
	C result;
	uint i = 0;
	auto it = std::begin(list), end = std::end(list);
	for (; i < n && it != end; ++i, ++it) ;
	for (; it != end; ++i, ++it) impl::container_traits<C>::add_element(result, *it);
	return result;
}

}
#endif
