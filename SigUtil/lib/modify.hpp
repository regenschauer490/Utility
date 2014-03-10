/*
Copyright(c) 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_ERASER_HPP
#define SIG_UTIL_ERASER_HPP

#include "sigutil.hpp"
#include "functional.hpp"
#include "tool.hpp"
#include <iterator>

/* ソート・削除などのコンテナに対する変更操作 */

namespace sig
{

#ifndef SIG_MSVC_LT1800

	//標準ソート関数のラッパ (シーケンスコンテナのみ対応)
	//std::sort()が使える場合
	template <class C, class F = std::less<typename sequence_container_traits<C>::value_type>, typename std::enable_if<HasRandomIter<C>::value>::type*& = enabler>
	void Sort(C& data, F const& binary_op = std::less<typename container_traits<C>::value_type>())
	{
		std::sort(std::begin(data), std::end(data), binary_op);
	}
	//メンバ関数にsort()がある場合
	template <class C, class F = std::less<typename sequence_container_traits<C>::value_type>, typename std::enable_if<!HasRandomIter<C>::value>::type*& = enabler>
	void Sort(C& data, F const& binary_op = std::less<typename container_traits<C>::value_type>())
	{
		data.sort(binary_op);
	}
#else
	template <class T, class F = std::less<typename sequence_container_traits<C>::value_type>>
	void Sort(std::vector<T>& data, F const& binary_op = std::less<T>())
	{
		std::sort(std::begin(data), std::end(data), binary_op);
	}
	template <class C, class F = std::less<typename sequence_container_traits<C>::value_type>, typename std::enable_if<!HasRandomIter<C>::value>::type*& = enabler>
	void Sort(C& data, F const& binary_op = std::less<typename sequence_container_traits<C>::value_type>())
	{
		data.sort(binary_op);
	}
#endif
	
	//ソート前の位置を保持してソート (シーケンスコンテナのみ対応)
	//ex: [30, 50, -10, 0] -> ([-10, 0, 30, 50], [2, 3, 0, 1])
	template <class C, class F = std::less<typename sequence_container_traits<C>::value_type>>
	auto SortWithIndex(C const& container, F const& binary_op = std::less<typename sequence_container_traits<C>::value_type>())
	{
		using Tp = std::tuple<typename sequence_container_traits<C>::value_type, uint>;
		auto result = Zip(container, ArithSequence(0, 1, container.size()));

		Sort(result, [&](Tp const& l, Tp const& r){ return binary_op(std::get<0>(l), std::get<0>(r)); });

		return UnZip(std::move(result));
	}

	//コンテナの要素をシャッフル
	template <class C>
	void Shuffle(C& container)
	{
		static SimpleRandom<double> myrand(0.0, 1.0, false);
		std::random_shuffle(std::begin(container), std::end(container), [&](std::ptrdiff_t max){ return static_cast<std::ptrdiff_t>(myrand() * max); });
	}


	template <class C>
	void ShuffleImpl(uint loop, C const& seq){}

	template <class C, class It, class... Its>
	void ShuffleImpl(uint loop, C const& seq, It iter, Its... iters)
	{
		auto ori_iter = iter;
		std::unordered_map<uint, typename It::value_type> map;

		for (uint i=0; i<loop; ++i, ++iter){
			map[seq[i]] = *iter;
		}
		for (uint i = 0; i < loop; ++i, ++ori_iter){
			*ori_iter = map[i];
		}

		ShuffleImpl(loop, seq, iters...);
	}

	//複数のコンテナの要素を対応させながらシャッフル
	//ex: c1[1, 2, 3, 4], c2[1, 2, 3, 4] -> c1'[3, 1, 4, 2], c2'[3, 1, 4, 2]
	template <class... Cs>
	void Shuffle(Cs&... containers)
	{
		uint size = Min(containers.size()...);
		auto rseq = RandomUniqueNumbers(size, 0, size - 1, false);
		
		ShuffleImpl(size, rseq, std::begin(containers)...);
	}


	//コンテナの要素から重複したものを削除
	//container: 処理対象コンテナ
	//need_removes: 削除した要素を戻り値で受け取るか
	//return -> 削除要素
	template <class C>
	C RemoveDuplicates(C& container, bool need_removes)
	{
		using T = typename container_traits<C>::value_type;
		std::unordered_map<T, bool> rmv;
		C result, removed;

		for (auto it = std::begin(container), end = std::end(container); it != end;){
			if (!rmv.count(*it)){
				container_traits<C>::add_element(result, std::move(*it));
				rmv[*it] = true;
				++it;
				continue;
			}
			else if(need_removes){
				container_traits<C>::add_element(removed, std::move(*it));
			}
			++rmv[*it];
			it = container.erase(it);
			end = container.end();
		}

		container = std::move(result);
		return removed;
	}

#if SIG_ENABLE_BOOST
#define Sig_Eraser_ParamType1 typename boost::call_traits<typename container_traits<C>::value_type>::param_type
#else
#define Sig_Eraser_ParamType1 typename std::common_type<typename container_traits<C>::value_type>::type const&
#endif

	//コンテナから指定要素を1つ削除
	//container: コンテナ
	//remove: 削除要素
	//return -> 削除要素が存在したか
	template <class C>
	bool RemoveOne(C& container, Sig_Eraser_ParamType1 remove)
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

	//コンテナから述語条件を満たす要素を1つ削除
	//container: コンテナ
	//remove_pred: 削除判別関数
	//return -> 削除要素が存在したか
	template <class Pred, class C>
	bool RemoveOneIf(C& container, Pred remove_pred)
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

	//コンテナから指定要素を全削除
	//container: コンテナ
	//remove: 削除要素
	//return -> 削除要素が存在したか
	template <class C>
	bool RemoveAll(C& container, Sig_Eraser_ParamType1 remove)
	{
		uint presize = container.size();
		if (!container.empty()) Erase(container, remove);
		return presize != container.size();
	}


	//コンテナから述語条件を満たす要素を全削除
	//container: コンテナ
	//remove_pred: 削除判別関数
	//return -> 削除要素が存在したか
	template <class Pred, class C>
	bool RemoveAllIf(C& container, Pred remove_pred)
	{
		uint presize = container.size();
		if (!container.empty()) EraseIf(container, remove_pred);
		return presize != container.size();
	}
}

#endif