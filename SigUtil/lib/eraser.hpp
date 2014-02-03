#ifndef __SIG_UTIL_ERASER__
#define __SIG_UTIL_ERASER__

#include "sigutil.hpp"


/* 削除・フィルタ関連 */

namespace sig{

	//コンテナの要素から重複したものを削除
	//data: コンテナ
	//need_removes: 削除した要素を戻り値で受け取るか
	//is_sorted: コンテナがソート済みか 
	//return -> 削除要素
/*	template <class Container, typename = decltype(std::declval<Container>() < std::declval<Container>(), void())>
	inline Container<T> RemoveDuplicates(Container<T>& list, bool need_removes, bool is_sorted = false)
	{
		if (!is_sorted) ;

		auto end = std::unique(std::begin(data), std::end(data));
		auto removes = need_removes ? Container(end, std::end(data)) : Container();
		data.erase(end, std::end(data));

		return std::move(removes);
	}
	template <class Container>
	Container RemoveDuplicates(Container& data, bool need_removes, bool is_sorted = false)
	{
		if (!is_sorted) std::sort(std::begin(data), std::end(data));

		auto end = std::unique(std::begin(data), std::end(data));
		auto removes = need_removes ? Container(end, std::end(data)) : Container();
		data.erase(end, std::end(data));

		return std::move(removes);
	}*/

#if SIG_ENABLE_BOOST
#define Sig_Eraser_ParamType1 typename boost::call_traits<T>::param_type
#else
#define Sig_Eraser_ParamType1 typename std::common_type<T>::type const&
#endif

	//コンテナから指定要素を1つ削除
	//args -> list: コンテナ, remove: 削除要素
	//return -> 削除要素が存在したか
	template <class T, template<class T_, class = std::allocator<T_>> class Container >
	inline bool RemoveOne(Container<T>& list, Sig_Eraser_ParamType1 remove)
	{
		for (auto it = list.begin(), end = list.end(); it != end;){
			if (*it == remove){
				list.erase(it);
				return true;
			}
			else ++it;
		}
		return false;
	}

	//コンテナから述語条件を満たす要素を1つ削除
	//args -> list: コンテナ, remove_pred: 削除判別関数
	//return -> 削除要素が存在したか
	template <class Pred, class T, template<class T_, class = std::allocator<T_>> class Container >
	inline bool RemoveOneIf(Container<T>& list, Pred remove_pred)
	{
		for (auto it = list.begin(), end = list.end(); it != end;){
			if (remove_pred(*it)){
				list.erase(it);
				return true;
			}
			else ++it;
		}
		return false;
	}

	//コンテナから指定要素を全削除
	//args -> list: コンテナ, remove: 削除要素
	//return -> 削除要素が存在したか
	template < class T, template < class T_, class = std::allocator<T_>> class Container >
	inline bool RemoveAll(Container<T>& list, Sig_Eraser_ParamType1 remove)
	{
		uint presize = list.size();
		if (!list.empty()) list.erase(std::remove(list.begin(), list.end(), remove), list.end());
		return presize != list.size();
	}

	//コンテナから述語条件を満たす要素を全削除
	//args -> list: コンテナ, remove_pred: 削除判別関数
	//return -> 削除要素が存在したか
	template <class Pred, class T, template<class T_, class = std::allocator<T_>> class Container >
	inline bool RemoveAllIf(Container<T>& list, Pred remove_pred)
	{
		uint presize = list.size();
		if (!list.empty()) list.erase(std::remove_if(list.begin(), list.end(), remove_pred), list.end());
		return presize != list.size();
	}

}

#endif