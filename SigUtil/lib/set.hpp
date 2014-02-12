/*
The MIT License(MIT)

Copyright(c) 2014 Akihiro Nishimura

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files(the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and / or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef __SIG_UTIL_SET__
#define __SIG_UTIL_SET__

#include "sigutil.hpp"
#include "tool.hpp"

/* コンテナの要素に対するよく行う操作 */

namespace sig
{

#if _MSC_VER > 1800
	template <class T, class D = void> struct RI{ static const bool value = false; };
	template <class T> struct RI<T, decltype(std::declval<T>()[0], void())>{ static const bool value = true; };

	template <class T, class D = void> struct CMPOP{ static const bool value = false; };
	template <class T> struct CMPOP<T, decltype(std::declval<T>() < std::declval<T>(), void())>{ static const bool value = true; };

	//至って普通なソートのラッパ
	template <class Container, typename std::enable_if<RI<Container>::value>::type*& = enabler>
	void Sort(Container& data){
		std::sort(std::begin(data), std::end(data));
	}
	template <class Container, typename std::enable_if<!RI<Container>::value && CMPOP<Container>::value>::type*& = enabler>
	void Sort(Container& data){
		data.sort();
	}
#endif

	//ソート前のindexを保持してソート
	template <class T>
	std::vector< std::tuple<uint, T> > SortWithIndex(std::vector<T> const& vec, bool const small_to_large)
	{
		std::vector< std::tuple<uint, T> > result(vec.size());

		for (uint i = 0; i < vec.size(); ++i){
			std::get<0>(result[i]) = i;
			std::get<1>(result[i]) = vec[i];
		}
		if (small_to_large) std::sort(result.begin(), result.end(), [](std::tuple<uint, T> const& a, std::tuple<uint, T> const& b){ return std::get<1>(a) < std::get<1>(b); });
		else std::sort(result.begin(), result.end(), [](std::tuple<uint, T> const& a, std::tuple<uint, T> const& b){ return std::get<1>(b) < std::get<1>(a); });

		return std::move(result);
	}

	//コンテナの要素をシャッフル
	template <class T, template < class T_, class = std::allocator<T_>> class Container>
	void Shuffle(Container<T>& data)
	{
		static SimpleRandom<double> myrand(0.0, 1.0, false);
		std::random_shuffle(data.begin(), data.end(), [&](uint max)->uint{ return myrand() * max; });
	}

	//2つのコンテナの要素を対応させながらソート
	template < class T1, class T2, template < class T_, class = std::allocator<T_>> class Container1, template < class T_, class = std::allocator<T_>> class Container2>
	void Shuffle(Container1<T1>& data1, Container2<T2>& data2)
	{
		uint size = std::min(data1.size(), data2.size());
		auto rnum = RandomUniqueNumbers(size, 0, size - 1, false);
		auto copy1 = std::move(data1);
		auto copy2 = std::move(data2);

		data1.resize(copy1.size());
		data2.resize(copy2.size());
		for (uint i = 0; i<size; ++i){
			data1[rnum[i]] = std::move(copy1[i]);
			data2[rnum[i]] = std::move(copy2[i]);
		}
	}


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

	//コンテナへの代入演算 ([a], [b], (a -> b -> a))
	template < class T1, class T2, template < class T_, class = std::allocator<T_>> class Container>
	void CompoundAssignment(Container<T1>& list1, Container<T2> const& list2, std::function<typename std::common_type<T1>::type(typename std::common_type<T1>::type, typename std::common_type<T2>::type)> const& op)
	{
		const uint length = list1.size() < list2.size() ? list1.size() : list2.size();

		for (uint i = 0; i < length; ++i) list1[i] = op(list1[i], list2[i]);
	}

	//コンテナへの代入演算 ([a], b, (a -> b -> a))
	template < class T1, class T2, template < class T_, class = std::allocator<T_>> class Container>
	void CompoundAssignment(Container<T1>& list1, T2 const& v, std::function<typename std::common_type<T1>::type(typename std::common_type<T1>::type, typename std::common_type<T2>::type)> const& op)
	{
		for (uint i = 0, length = list1.size(); i < length; ++i) list1[i] = op(list1[i], v);
	}


	//生成関数を通して値を生成する
	//args -> generator: 生成関数.引数はループindex
	template < class T, template < class T_, class = std::allocator<T_>> class Container = std::vector>
	Container<T> Generate(std::function<T(int)> const& generator, uint count)
	{
		Container<T> tmp;
		tmp.reserve(count);
		for (uint i = 0; i < count; ++i) tmp.push_back(generator(i));
		return std::move(tmp);
	}


	//vector, list の積集合を求める(要素数は1個). [動作要件：T::operator==()]
	template <class T, template<class T_, class = std::allocator<T_>> class Container>
	Container<T> SetIntersection(Container<T> const& src1, Container<T> const& src2)
	{
		Container<T> result;

		for (T const& e1 : src1){
			for (T const& e2 : src2){
				if (e1 == e2 && [&result, &e2]()->bool{
					for (T const& r : result){
						if (r == e2) return false;
					}
					return true;
				}()){
					result.push_back(e2);
				}
			}
		}
		return move(result);
	}

	//unordered_set の積集合を求める.[動作要件：T::operator==()]
	template <class T>
	std::unordered_set<T> SetIntersection(std::unordered_set<T> const& src1, std::unordered_set<T> const& src2)
	{
		std::unordered_set<T> result;

		for (T const& e1 : src1){
			for (T const& e2 : src2){
				if (e1 == e2) result.insert(e2);
			}
		}
		return move(result);
	}

	//unordered_map の積集合を求める(bool key ? キーで比較【第1引数の要素を取得】 : 両方一致). [動作要件：K::operator==(), V::operator==()]
	template <class K, class V>
	std::unordered_map<K, V> SetIntersection(std::unordered_map<K, V> const& src, std::unordered_map<K, V> const& other, bool const key)
	{
		std::unordered_map<K, V> result;

		for (auto const& e : src){
			for (auto const& o : other){
				if (key && e.first == o.first) result.insert(e);
				else if (e == o) result.insert(e);
			}
		}
		return move(result);
	}


	//vector, list の差集合を求める(要素数は1個). [動作要件：T::operator==()]
	template <class T, template<class T_, class = std::allocator<T_>> class Container>
	Container<T> SetDifference(Container<T> const& src1, Container<T> const& src2)
	{
		Container<T> result, sum(src1);
		sum.insert(sum.end(), src2.begin(), src2.end());

		auto intersection = SetIntersection(src1, src2);

		for (T const& s : sum){
			if ([&intersection, &s]()->bool{
				for (T const& i : intersection){
					if (s == i) return false;
				}
				return true;
			}() && [&result, &s]()->bool{
				for (T const& r : result){
					if (s == r) return false;
				}
				return true;
			}()){
				result.push_back(s);
			}
		}
		return move(result);
	}

	//unordered_set の差集合を求める.[動作要件：T::operator==()]
	template <class T>
	std::unordered_set<T> SetDifference(std::unordered_set<T> const& src1, std::unordered_set<T> const& src2)
	{
		std::unordered_set<T> result, sum(src1);
		sum.insert(src2.begin(), src2.end());

		auto intersection = SetIntersection(src1, src2);

		for (T const& s : sum){
			if ([&intersection, &s]()->bool{
				for (T const& i : intersection){
					if (s == i) return false;
				}
				return true;
			}()){
				result.insert(s);
			}
		}
		return move(result);
	}

	//unordered_map の差集合を求める(bool key ? キーで比較 : 両方一致). [動作要件：K::operator==(), V::operator==()]
	template <class K, class V>
	std::unordered_map<K, V> SetDifference(std::unordered_map<K, V> const& src1, std::unordered_map<K, V> const& src2, bool const key)
	{
		std::unordered_map<K, V> result, sum(src1);
		sum.insert(src2.begin(), src2.end());

		auto intersection = SetIntersection(src1, src2, key);

		for (auto const& s : sum){
			if ([&intersection, &s, key]()->bool{
				for (auto const& i : intersection){
					if (key && s.first == i.first) return false;
					else if (!key && s == i) return false;
				}
				return true;
			}()){
				result.insert(s);
			}
		}
		return move(result);
	}


}

#endif