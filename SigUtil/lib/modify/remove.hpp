/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIGUTIL_REMOVE_HPP
#define SIGUTIL_REMOVE_HPP

#include "../sigutil.hpp"
#include "../helper/container_helper.hpp"

/// \file remove.hpp コンテナの要素削除に関する処理

namespace sig
{
/// コンテナの要素から重複した値を削除
/**
	\param container: 処理対象コンテナ（\ref sig_container ）

	\return std::map<削除値, その個数>

	\code
	array<std::string, 4> data1{ "a", "b", "c", "b" };
	std::vector<int> data2{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };
	std::unordered_multiset<int> data3{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };

	auto removed1 = remove_duplicates(data1);	// { "a":0, "b":1, "c":0 }
	data1;										// { "a", "b", "c" }
	auto removed2 = remove_duplicates(data2);	// { 0:1, 1:1, 3:2, 4:0, 5:0 }
	data2;										// { 1, 5, 3, 0, 4 }
	auto removed3 = remove_duplicates(data3);
	data3;										// { 1, 5, 3, 0, 4 } 順不同
	\endcode
*/
template <class C,
	class CR = typename impl::remove_const_reference<C>::type,
	class T = typename impl::container_traits<CR>::value_type
>
auto remove_duplicates(C& container)
{
	std::map<T, uint> removed;

	for (auto it = std::begin(container), end = std::end(container); it != end;){
		if (!removed.count(*it)){
			removed[*it] = 0;
			++it;
		}
		else{
			++removed[*it];
			it = container.erase(it);
			end = container.end();
		}
	}

	return removed;
}

#if SIG_ENABLE_BOOST
#define Sig_Eraser_ParamType1 typename boost::call_traits<typename impl::container_traits<C>::value_type>::param_type
#else
#define Sig_Eraser_ParamType1 typename std::common_type<typename impl::container_traits<C>::value_type>::type const&
#endif

/// コンテナから指定した値を1つ削除
/**
	\param container 処理対象コンテナ（\ref sig_container ）
	\param remove 削除したい値

	\return 削除値が存在したか(bool)

	\code
	array<std::string, 4> data1{ "a", "b", "c", "b" };
	std::vector<int> data2{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };
	std::unordered_multiset<int> data3{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };

	remove_one(data1, "b");		// true
	remove_one(data2, 3);		// true
	remove_one(data3, 10);		// false

	data1;		// { "a", "c", "b" }
	data2;		// { 1, 5, 3, 0, 4, 0, 1, 3 }
	data3;		// { 1, 5, 3, 3, 0, 4, 0, 1, 3 }
	\endcode
*/
template <class C>
bool remove_one(C& container, Sig_Eraser_ParamType1 remove)
{
	for (auto it = std::begin(container), end = std::end(container); it != end;){
		if (*it == remove){
			container.erase(it);
			return true;
		}
		else ++it;
	}

	return false;
}

/// コンテナから述語条件を満たす値を1つ削除
/**
	\param container 処理対象コンテナ（\ref sig_container ）
	\param remove_pred 削除するか判断を行う関数オブジェクト

	\return 削除値が存在したか(bool)

	\code
	std::list<int> data{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };

	remove_one_if(data, [](int v){ return v == 3; });
	data;		// { 1, 5, 3, 0, 4, 0, 1, 3 }
	\endcode
*/
template <class Pred, class C>
bool remove_one_if(C& container, Pred remove_pred)
{
	for (auto it = std::begin(container), end = std::end(container); it != end;){
		if (remove_pred(*it)){
			container.erase(it);
			return true;
		}
		else ++it;
	}

	return false;
}

/// コンテナから指定した値を全削除
/**
	\param container 処理対象コンテナ（\ref sig_container ）
	\param remove 削除したい値

	\return 削除値が存在したか(bool)

	\code
	array<std::string, 4> data1{ "a", "b", "c", "b" };
	std::vector<int> data2{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };
	std::unordered_multiset<int> data3{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };

	remove_all(data1, "b");	// true
	remove_all(data2, 3);	// true
	remove_all(data3, 10);	// false

	data1;		// { "a", "c" }
	data2;		// { 1, 5, 0, 4, 0, 1 }
	data3;		// { 1, 5, 3, 3, 0, 4, 0, 1, 3 }
	\endcode
*/
template <class C>
bool remove_all(C& container, Sig_Eraser_ParamType1 remove)
{
	uint presize = container.size();

	if (!container.empty()) erase(container, remove);

	return presize != container.size();
}


/// コンテナから述語条件を満たす値を全削除
/**
	\param container 処理対象コンテナ（\ref sig_container ）
	\param remove_pred 削除するか判断を行う関数オブジェクト
	
	\return 削除値が存在したか

	\code
	std::list<int> data{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };

	remove_all_if(data, [](int v){ return v == 3; });
	data;		// { 1, 5, 0, 4, 0, 1 }
	\endcode
*/
template <class Pred, class C>
bool remove_all_if(C& container, Pred remove_pred)
{
	uint presize = container.size();

	if (!container.empty()) erase_if(container, remove_pred);

	return presize != container.size();
}

}
#endif
