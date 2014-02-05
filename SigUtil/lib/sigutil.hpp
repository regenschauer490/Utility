#ifndef __SIG_UTIL_H__
#define __SIG_UTIL_H__

#define SIG_ENABLE_BOOST 1

#ifdef _WIN32
#define SIG_WINDOWS_ENV 1
#elif _WIN64
#define SIG_WINDOWS_ENV 1
#else
#define SIG_WINDOWS_ENV 0
#endif

#include <assert.h>
#include <string>
#include <vector>

#include <stdio.h>
#include <iostream>
#include <locale>
#include <fstream>
#include <iomanip>
#include <string>
#include <memory>
#include <random>
#include <functional>
#include <algorithm>
#include <array>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <numeric>
#include <regex>
#include <utility>

#include "container_traits_plus.hpp"

#if SIG_ENABLE_BOOST

#include <boost/optional.hpp>
#include <boost/format.hpp>
#include <boost/call_traits.hpp>
#include <boost/range.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/serialization/serialization.hpp>

#endif

namespace sig{
#undef max
#undef min

	/* typedef */
	typedef unsigned long uint;
	typedef std::shared_ptr< std::string > StrPtr;
	typedef std::shared_ptr< std::string const > C_StrPtr;
	typedef std::shared_ptr< std::wstring > WStrPtr;
	typedef std::shared_ptr< std::wstring const > C_WStrPtr;
	
#if SIG_ENABLE_BOOST
	template <typename T>
	using maybe = boost::optional<T>;

	auto const nothing = boost::none;
#endif


/* ヘルパ関数・ヘルパクラス */
	struct NullType{};
	
	extern void* enabler;

	//maybeの有効・無効に関係なく記述するためのもの
#if SIG_ENABLE_BOOST
	template <class T> struct Just{ typedef maybe<T> type; };
	template <class T> auto Nothing(T const& default_value)-> decltype(nothing){ return nothing; }
#else
	template <class T> struct Just{ typedef T type; };
	template <class T> T Nothing(T&& default_value){ return std::forward<T>(default_value); }
#endif

	template <class T> struct NullDummy{ typedef T type; };
	template <> struct NullDummy<void>{ typedef NullType type; };

	/*
#if _MSC_VER > 1800
	//container_traitsの拡張
	template<class C>
	struct map_associative_container_traits;

	template<template<class , class, class, class> class C, class K, class T, template<class> class O, class A>
	struct map_associative_container_traits<C<K, T, O<K>, A>>
	{
		using key_type = K;

		using value_type = T;

		using pair_type = std::pair<const K, T>;

		static void add_element(C<K, T, O<K>, A>& c, const pair_type& t)
		{
			c.insert(t);
		}

		static void concat(C<K, T, O<K>, A>& lhs, const C<K, T, O<K>, A>& rhs)
		{
			lhs.insert(rhs.begin(), rhs.end());
		}

		template<class U>
		using rebind = C<K, U, O<K>, typename A::template rebind<U>::other>;
	};

	template<class... Args>
	struct container_traits<std::map<Args...>> : public map_associative_container_traits<std::map<Args...>>
	{};

	template<class... Args>
	struct container_traits<std::multimap<Args...>> : public map_associative_container_traits<std::multimap<Args...>>
	{};
#endif
*/
	//template <class Container, class Sfinae = void> struct ContainerConstructor{ typedef Container type; };
	//template <class Container> struct ContainerConstructor<Container, typename std::enable_if<std::is_array<Container>::value>::type>{ typedef std::array<std::remove_extent<Container>, std::rank<Container>::value> type; };
	
	//template <class T, class D = void> struct HasBegin : std::true_type{};
	//template <class T> struct HasBegin<T, decltype(std::declval<T>().begin())> : std::false_type{};

	//template <class T> constexpr auto HasBegin(int) ->decltype(std::declval<T>().begin()){ return true; }
	//template <class T> constexpr bool HasBegin(...){ return false; }

	//template <typename T> constexpr auto has_reserve_method(int) -> decltype(std::declval<T>().reserve(0), bool()) { return true; }
	//template <typename T> constexpr bool has_reserve_method(...) { return false; }

	/*template <typename T> auto Reserve(T& t, size_t n) -> typename std::enable_if<has_reserve_method<T>(0), void>::type {
		std::cout << "true" << std::endl;
		t.reserve(n);
	}

	template <typename T> auto Reserve(T& t, size_t n) -> typename std::enable_if<!has_reserve_method<T>(0), void>::type {
		std::cout << "false" << std::endl;
	}*/
	
	/*template <typename T, typename std::enable_if<has_reserve_method<T>(0)>::type *& = enabler> void Reserve(T& t, size_t n){
		std::cout << "true" << std::endl;
		t.reserve(n);
	}

	template <typename T, typename std::enable_if<!has_reserve_method<T>(0)>::type *& = enabler> void Reserve(T& t, size_t n) {
		std::cout << "false" << std::endl;
	}*/

	template <class T> auto Reserve(T& t, size_t n) ->decltype(t.reserve(n), void()){ t.reserve(n); std::cout << "true" << std::endl; }
	template <class T> void Reserve(T& t, size_t n){ std::cout << "false" << std::endl; }


/* 便利関数 */

	//xor
	inline bool BoolXor(bool A, bool B){ return (A && !B) || (!A && B); }

	//AとBの真偽一致でtrueを返す (⇔ !xor)
	inline bool BoolConsistency(bool A, bool B){ return (A && B) || (!A && !B); }

	//浮動小数点型にも使える等値比較関数
	template <class T1, class T2>
	bool Equal(T1 v1, T2 v2)
	{
		const auto dmin = std::numeric_limits<std::common_type<T1, T2>::type>::min();

		return std::abs(v1 - v2) < dmin;
	}

	//小数点以下の桁数取得 (ex: v=1.2300000 -> 2)
	inline uint Precision(double v)
	{
		uint keta = 0;
		double dv = v - int(v);
		
		while (true){
			if (Equal(dv, 0) || keta >= 15) break;
			v *= 10;
			dv = v - int(v);
			++keta;
		}

		return keta;
	}

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
		auto rnum = RandomUniqueNumbers(size, 0, size-1, false);
		auto copy1 = std::move(data1);
		auto copy2 = std::move(data2);

		data1.resize(copy1.size());
		data2.resize(copy2.size());
		for (uint i=0; i<size; ++i){
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

/* 便利アイテム */

#if SIG_ENABLE_BOOST
	//パーセント型
	class Percent
	{
		int _percent;

	public:
		explicit Percent(int percent) : _percent(percent){}

		int GetPercent() const{ return _percent; }
		double GetDouble() const{ return _percent * 0.01; }

		static Percent const& Unit(){ static const Percent unit(100); return unit; }

		Percent operator=(Percent src){ _percent = src._percent; return *this; }
		Percent operator=(int src){ _percent = src; return *this; }
		
		bool operator==(Percent obj) const{ return _percent == obj._percent; }

		bool operator!=(Percent obj) const{ return _percent != obj._percent; }

	private:
		friend class boost::serialization::access;

		template <class Archive>
		void serialize(Archive& ar, unsigned int version)
		{
			ar & _percent;
		}

		template <class Archive>
		friend void save_construct_data(Archive & ar, Percent const* p, unsigned int version){};

		template <class Archive>
		friend void load_construct_data(Archive & ar, Percent* p, unsigned int version){
			::new(p) Percent(0);
		}
	};

#endif


/* 修正・補正・追加・削除 */

	//範囲チェックと自動修正
	template <class T, class U>
	inline bool ModifyRange(T& val, U const& min, U const& max)
	{
		if(val<min){ val = min; return false; }
		if(val>max){ val = max; return false; }
		return true;
	}

	template <class T, class U>
	inline bool CheckRange(T const& val, U const& min, U const& max)
	{
		if(val<min){ return false; }
		if(val>max){ return false; }
		return true;
	}


/* 集合操作 */
	namespace Set{

		//vector, list の積集合を求める(要素数は1個). [動作要件：T::operator==()]
		template <class T, template<class T_, class = std::allocator<T_>> class Container>
		Container<T> SetIntersection(Container<T> const& src1, Container<T> const& src2)
		{
			Container<T> result;

			for(T const& e1 : src1){
				for(T const& e2 : src2){
					if(e1 == e2 && [&result, &e2]()->bool{
						for(T const& r : result){
							if(r == e2) return false;
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

			for(T const& e1 : src1){
				for(T const& e2 : src2){
					if(e1 == e2) result.insert(e2);
				}
			}
			return move(result);
		}

		//unordered_map の積集合を求める(bool key ? キーで比較【第1引数の要素を取得】 : 両方一致). [動作要件：K::operator==(), V::operator==()]
		template <class K, class V>
		std::unordered_map<K,V> SetIntersection(std::unordered_map<K,V> const& src, std::unordered_map<K,V> const& other, bool const key)
		{
			std::unordered_map<K,V> result;

			for(auto const& e : src){
				for(auto const& o : other){
					if(key && e.first == o.first) result.insert(e);
					else if(e == o) result.insert(e);
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

			for(T const& s : sum){
				if([&intersection, &s]()->bool{
					for(T const& i : intersection){
						if(s == i) return false;
					}
					return true;
				}() && [&result, &s]()->bool{
					for(T const& r : result){
						if(s == r) return false;
					}
					return true;
				}()	){
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

			for(T const& s : sum){
				if([&intersection, &s]()->bool{
					for(T const& i : intersection){
						if(s == i) return false;
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
		std::unordered_map<K,V> SetDifference(std::unordered_map<K,V> const& src1, std::unordered_map<K,V> const& src2, bool const key)
		{
			std::unordered_map<K,V> result, sum(src1);
			sum.insert(src2.begin(), src2.end());

			auto intersection = SetIntersection(src1, src2, key);

			for(auto const& s : sum){
				if([&intersection, &s, key]()->bool{
					for(auto const& i : intersection){
						if(key && s.first == i.first) return false;
						else if(!key && s == i) return false;
					}
					return true;
				}()){
					result.insert(s);
				}
			}
			return move(result);
		}

	}

/*	template < template<class T, class = std::allocator<T>> class Container >
	inline void Print(Container<std::string> const& container, char const* const delimiter = "\n")
	{
		std::copy(container.begin(), container.end(), std::ostream_iterator<std::string>(std::cout, delimiter));
	}

	template < template<class T, class = std::allocator<T>> class Container >
	inline void Print(Container<std::wstring> const& container, wchar_t const* const delimiter = L"\n")
	{
		std::copy(container.begin(), container.end(), std::ostream_iterator<std::wstring>(std::wcout, delimiter));
	}
*/

	inline void Print(std::string const& text, char const* const delimiter = "\n")
	{
		std::cout << text << delimiter;
	}

	inline void Print(std::wstring const& text, wchar_t const* const delimiter = L"\n")
	{
		std::wcout << text << delimiter;
	}

	template < class T, template < class T_, class = std::allocator<T_>> class Container, typename std::enable_if<!std::is_same<T, std::wstring>::value>::type*& = enabler>
	inline void Print(Container<T> const& container, char const* const delimiter = "\n")
	{
		std::copy(container.begin(), container.end(), std::ostream_iterator<T>(std::cout, delimiter));
	}

	template<template<class ...> class Container>
	inline void Print(Container<std::wstring> const& container, wchar_t const* const delimiter = L"\n")
	{
		std::copy(container.begin(), container.end(), std::ostream_iterator<std::wstring>(std::wcout, delimiter));
	}


}

#endif UTILITY