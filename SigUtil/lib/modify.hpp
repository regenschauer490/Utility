/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_ERASER_HPP
#define SIG_UTIL_ERASER_HPP

#include "sigutil.hpp"
#include "functional.hpp"
#include "tools.hpp"
#include <iterator>


/* ソート・削除など、コンテナに対する変更操作 */

namespace sig
{
#undef min
#undef max

// 標準ソート関数のラッパ (シーケンスコンテナのみ対応)
// container: ソートするコンテナ
// binary_op: 大小比較を行う関数オブジェクト(STLと同様)
#if SIG_MSVC_LT1800

template <class C, class F = std::less<typename sequence_container_traits<C>::value_type>, typename std::enable_if<has_random_access_op<C>::value>::type*& = enabler>
void sort(
	C& container,
	F const& binary_op = std::less<typename container_traits<C>::value_type>())
{
	std::sort(std::begin(container), std::end(container), binary_op);
}
// メンバ関数にsort()がある場合
template <class C, class F = std::less<typename sequence_container_traits<C>::value_type>, typename std::enable_if<!has_random_access_op<C>::value>::type*& = enabler>
void sort(
	C& container,
	F const& binary_op = std::less<typename container_traits<C>::value_type>())
{
	container.sort(binary_op);
}
#else

template <class T, class F>
void sort(
	std::vector<T>& container,
	F const& binary_op = std::less<T>())
{
	std::sort(std::begin(container), std::end(container), binary_op);
}

template <class C, class F = std::less<typename sequence_container_traits<C>::value_type>, typename std::enable_if<!has_random_access_op<C>::value>::type*& = enabler>
void sort(
	C& container,
	F const& binary_op = std::less<typename sequence_container_traits<C>::value_type>())
{
	container.sort(binary_op);
}
#endif
	
// ソート前の位置を保持してソート (シーケンスコンテナのみ対応)
// ex: [30, 50, -10, 0] -> ([-10, 0, 30, 50], [2, 3, 0, 1])
// container: ソート対象のデータ
// binary_op: 大小比較を行う関数オブジェクト(STLと同様)
// return -> std::tuple<(ソート後のコンテナ), (ソート前のindexを記録したコンテナ)>
template <class C, class F = std::less<typename sequence_container_traits<C>::value_type>>
auto sort_with_index(
	C const& container,
	F const& binary_op = std::less<typename sequence_container_traits<C>::value_type>())
{
	using Tp = std::tuple<typename sequence_container_traits<C>::value_type, uint>;
	auto result = zip(container, seq(0u, 1u, container.size()));

	sort(result, [&](Tp const& l, Tp const& r){ return binary_op(std::get<0>(l), std::get<0>(r)); });

	return unzip(std::move(result));
}

// コンテナの要素をシャッフル
template <class C>
void shuffle(C& container)
{
	static SimpleRandom<double> myrand(0.0, 1.0, false);
	std::random_shuffle(std::begin(container), std::end(container), [&](std::ptrdiff_t max){ return static_cast<std::ptrdiff_t>(myrand() * max); });
}

namespace impl{

template <class C>
void shuffle_impl(uint loop, C const& seq){}

template <class C, class It, class... Its>
void shuffle_impl(uint loop, C const& seq, It iter, Its... iters)
{
	auto ori_iter = iter;
	std::unordered_map<uint, typename It::value_type> map;

	for (uint i=0; i<loop; ++i, ++iter){
		map[seq[i]] = *iter;
	}
	for (uint i = 0; i < loop; ++i, ++ori_iter){
		*ori_iter = map[i];
	}

	shuffle_impl(loop, seq, iters...);
}
}	//impl

// 複数のコンテナの要素を対応させながらシャッフル
// ex: c1[1, 2, 3, 4], c2[1, 2, 3, 4] -> c1'[3, 1, 4, 2], c2'[3, 1, 4, 2]
template <class... Cs>
void shuffle(Cs&... containers)
{
	uint size = min(containers.size()...);
	auto rseq = make_unique_numbers(size, 0, size - 1, false);
		
	impl::shuffle_impl(size, rseq, std::begin(containers)...);
}


// コンテナの要素から重複したものを削除
// container: 処理対象コンテナ
// return -> 削除要素とその個数
template <class C>
auto remove_duplicates(C& container)
{
	using T = typename container_traits<C>::value_type;
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
#define Sig_Eraser_ParamType1 typename boost::call_traits<typename container_traits<C>::value_type>::param_type
#else
#define Sig_Eraser_ParamType1 typename std::common_type<typename container_traits<C>::value_type>::type const&
#endif

// コンテナから指定要素を1つ削除
// container: 処理対象コンテナ
// remove: 削除要素
// return -> 削除要素が存在したか
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

// コンテナから述語条件を満たす要素を1つ削除
// container: 処理対象コンテナ
// remove_pred: 削除判別を行う関数オブジェクト
// return -> 削除要素が存在したか
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

// コンテナから指定要素を全削除
// container: 処理対象コンテナ
// remove: 削除要素
// return -> 削除要素が存在したか
template <class C>
bool remove_all(C& container, Sig_Eraser_ParamType1 remove)
{
	uint presize = container.size();
	if (!container.empty()) erase(container, remove);
	return presize != container.size();
}


// コンテナから述語条件を満たす要素を全削除
// container: 処理対象コンテナ
// remove_pred: 削除判別を行う関数オブジェクト
// return -> 削除要素が存在したか
template <class Pred, class C>
bool remove_all_if(C& container, Pred remove_pred)
{
	uint presize = container.size();
	if (!container.empty()) erase_if(container, remove_pred);
	return presize != container.size();
}

}
#endif
