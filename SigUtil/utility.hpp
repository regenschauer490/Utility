#ifndef UTILITY_H
#define UTILITY_H

/* Last Update : 2014 1 1 */

#define ENABLE_BOOST 1

#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <locale>
#include <fstream>
#include <sstream>
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
#include <chrono>
#include <numeric>
#include <regex>
#include <utility>

#if ENABLE_BOOST

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
	
#if ENABLE_BOOST

	template <typename T>
	using maybe = boost::optional<T>;

	auto const nothing = boost::none;

#endif


/* ヘルパ関数・ヘルパクラス */
	struct NullType{};
	
	extern void* enabler;

	template <class Container, class Sfinae = void> struct ContainerConstructor{ typedef Container type; };
	template <class Container> struct ContainerConstructor<Container, typename std::enable_if<std::is_array<Container>::value>::type>{ typedef std::array<std::remove_extent<Container>, std::rank<Container>::value> type; };
	
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
}

/* 関数型プログラミング */

	//[a] -> (a -> r) -> [r]
	//戻り値の型Rは明示的に指定が必要
/*	template <class Out, class In, class Func, typename std::enable_if<HasBegin<Out>(0)>::type*& = enabler>
	Out Map(In const& list, Func func)
	{
		Out result;
		//Reserve(result, list.size());
		std::transform(std::begin(list), std::end(list), std::inserter(result, std::begin(result)), func);
		return std::move(result);
	}

	template <class Out, class In, class Func, typename std::enable_if<!HasBegin<Out>(0)>::type*& = enabler>
	std::vector<Out> Map(In const& list, Func func)
	{
		std::vector<Out> result;
		//Reserve(result, list.size());
		std::transform(std::begin(list), std::end(list), std::inserter(result, std::begin(result)), func);
		return std::move(result);
	}
*/

	template <class R, class A, template <class T, class = std::allocator<T>> class Container>
	Container<R> Map(Container<A> const& list, std::function<typename std::common_type<R>::type(typename std::common_type<A>::type)> const& func)
	{
		Container<R> result;
		Reserve(result, list.size());
		std::transform(list.begin(), list.end(), std::back_inserter(result), func);
		return std::move(result);
	}
	
	template <class R, class A, std::size_t N, template <class T, size_t N> class Array>
	Array<R, N> Map(Array<A, N> const& list, std::function<typename std::common_type<R>::type(typename std::common_type<A>::type)> const& func)
	{
		Array<R, N> result;
		std::transform(list.begin(), list.end(), result.begin(), func);
		return result;
	}

	template <class R, class A, std::size_t N>
	std::array<R, N> Map(A const(&list)[N], std::function<typename std::common_type<R>::type(typename std::common_type<A>::type)> const& func)
	{
		std::array<R, N> result;
		std::transform(list, list + N, result.begin(), func);
		return result;
	}

	//[a] -> [b] -> (a -> b -> r) -> [r]
	//戻り値の型Rは、明示的に指定する必要あり
	template < class R, class A, class B, template < class T, class = std::allocator<T>> class Container>
	Container<R> ZipWith(Container<A> const& list1, Container<B> const& list2, std::function<typename std::common_type<R>::type(typename std::common_type<A>::type, typename std::common_type<B>::type)> const& func)
	{
		const uint length = list1.size() < list2.size() ? list1.size() : list2.size();
		Container<R> result;

		uint i = 0;
		for (auto it1 = list1.begin(), it2 = list2.begin(), end1 = list1.end(), end2 = list2.end(); i < length; ++i, ++it1, ++it2) result.push_back(func(*it1, *it2));

		return std::move(result);
	}

#if ENABLE_BOOST
	//[a] -> b -> (a -> b -> r) -> [r]
	//戻り値の型Rは、明示的に指定する必要あり
	template < class R, class A, class B, template < class T, class = std::allocator<T >> class Container>
		Container<R> ZipWith(Container<A> const& list1, typename boost::call_traits<B>::param_type val, std::function<typename std::common_type<R>::type(typename std::common_type<A>::type, typename std::common_type<B>::type)> const& func)
	{
		Container<R> result;

		uint i = 0;
		for (auto it1 = list1.begin(), end1 = list1.end(); i < list1.size(); ++i, ++it1) result.push_back(func(*it1, val));

		return std::move(result);
	}
#endif

	//[a] -> b -> (a -> common<a,b> -> common<a,b>) -> common<a,b>
	//std::accumulateとは違い、初期値の型BではなくAとBが暗黙的に変換される型に集約
	template < class A, class B, template < class T, class = std::allocator<T>> class Container>
		typename std::common_type<A, B>::type Accumulate(Container<A> const& list, B init, std::function<typename std::common_type<A>::type(typename std::common_type<A>::type, typename std::common_type<A, B>::type)> const& func){
			typename std::common_type<A, B>::type result = init;
			for(auto const& e : list)  result = func(e, result);
			return result;
		}


/* 乱数 */

	//初期化時に指定した範囲の一様分布乱数を発生させるクラス
	//デフォルト: 乱数生成器 -> メルセンヌツイスター
	template <class NumType, class Engine = std::mt19937>
	class SimpleRandom {
		Engine _engine;		//乱数生成アルゴリズム 
		typename std::conditional <
			std::is_integral<NumType>::value,
			std::uniform_int_distribution<int>,
			std::uniform_real_distribution<double>
		> ::type _dist;		//確率分布

	public:
		SimpleRandom(NumType min, NumType max, bool debug) : _engine(
			[debug](){
				std::random_device rnd;
				std::vector<uint> v(10);
				if (debug) std::fill(v.begin(), v.end(), 0);
				else std::generate(v.begin(), v.end(), std::ref(rnd));

				return Engine(std::seed_seq(v.begin(), v.end()));
		}()
			),
			_dist(min, max){}

		NumType operator()(){
			return _dist(_engine);
		}
	};


	//重複の無い整数乱数を生成
	template < template < class T, class = std::allocator<T>> class Container = std::vector >
	Container<int> RandomUniqueNumbers(std::size_t n, int min, int max, bool debug) {
		std::unordered_set<int> match;
		Container<int> result;
		static SimpleRandom<int> Rand(0, max - min, debug);

		int r;
		for (int i = 0; i < n; ++i){
			do{
				r = min + Rand();
			} while (match.find(r) != match.end());

			match.insert(r);
			result.push_back(r);
		}

		return std::move(result);
	}


/* 便利関数 */

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

#if ENABLE_BOOST

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

	//値を指定個複製する
	template < class T, template < class T_, class = std::allocator<T_>> class Container = std::vector>
		Container<T> Fill(T const& value, uint count)
	{
		Container<T> tmp;
		tmp.reserve(count);
		for (uint i = 0; i < count; ++i) tmp.push_back(value);
		return std::move(tmp);
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

#if ENABLE_BOOST
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

	//タイムウォッチ
	class TimeWatch{
		typedef std::chrono::system_clock::time_point TIME;
		TIME st, ed;
	
		void Init(){
			ed = std::chrono::system_clock::now();
			st = std::chrono::system_clock::now();
		}

	public:
		TimeWatch(){ Init(); }

		void ReStart(){ Init(); }

		void Stop(){
			ed = std::chrono::system_clock::now();
		}
		
		//template<class TimeUnit = std::chrono::milliseconds> VC++2012 未対応
		template<class TimeUnit>
		long long GetTime(){
			return std::chrono::duration_cast<TimeUnit>(ed - st).count();
		}
	};
		
	//ヒストグラム
	//template <要素の型, 度数>
	template <class T, size_t BIN_NUM>
	class Histgram{
		T const _min;
		T const _max;
		double const _delta;
		std::array<uint, BIN_NUM + 2> _count;	//[0]: x < min, [BIN_NUM-1]: max <= x
		uint _num;

	public:
		//要素の範囲を指定
		Histgram(T min, T max) : _min(min), _max(max), _delta(((double) max - min) / BIN_NUM), _num(0){
			assert(_delta > 0);
			for (auto& ct : _count) ct = 0;
		}

		//要素をbinに振り分けてカウント
		void Count(T value){
			for (uint i = 0; i < BIN_NUM + 1; ++i){
				if (value < _delta*i + _min){
					++_num;
					++_count[i];
					return;
				}
			}
			++_count[BIN_NUM + 1];
		}

		template < template < class TT, class = std::allocator<TT >> class Container>
			void Count(Container<T> const& values){
				for (auto e : values) Count(e);
			}

			//bin外の要素が存在したか
			bool IsOverRange() const{ return _count[0] || _count[BIN_NUM + 1]; }

			//double GetAverage() const{ return std::accumulate(_count.begin(), _count.end(), 0, [](T total, T next){ return total + next; }) / static_cast<double>(_num); }

			//頻度を取得
			std::array<uint, BIN_NUM> GetCount() const{
				std::array<uint, BIN_NUM> tmp;
				for (uint i = 0; i < BIN_NUM; ++i) tmp[i] = _count[i + 1];
				return std::move(tmp);
			}

#if ENABLE_BOOST
			//bin番目(0 ～ BIN_NUM-1)の頻度を取得
			//return -> tuple<頻度, 範囲最小値(以上), 範囲最大値(未満)>
			maybe<std::tuple<uint, int, int>> GetCount(uint bin) const{ return bin < BIN_NUM ? maybe < std::tuple < uint, int, int >> (std::make_tuple(_count[bin + 1], _delta*bin + _min, _delta*(bin + 1) + _min)) : nothing; }
#else
			std::tuple<uint,int,int> GetCount(uint bin) const{ return bin < BIN_NUM ? std::make_tuple(_count[bin+1], _delta*bin+_min, _delta*(bin+1)+_min) : throw std::out_of_range("Histgram::Get, bin=" + std::to_string(bin)); }
#endif

			void Print() const{
				auto IsPlus = [](double v){ return v < 0 ? false : true; };

				auto IntDigit = [](double v){ return log10(v) + 1; };

				auto Space = [](int num){
					std::string space;
					for (int i = 0; i < num; ++i) space.append(" ");
					return std::move(space);
				};

				int const rketa = IntDigit(_max);
				int const disp_precision = rketa > 2 ? 0 : 2-rketa;
				int const keta = std::max(rketa, std::min((int) Precision(_delta), disp_precision) + 2);
				int const ctketa = log10(*std::max_element(_count.begin(), _count.end())) + 1;
				T const dbar = _num < 100 ? 1.0 : _num*0.01;

				/*
				std::string offset1, offset2;
				if (keta < 3) offset1.append(2 - keta, ' ');
				else offset2.append(keta - 3, ' ');*/

				std::cout << "\n-- Histgram --\n";
				for (int i = 0; i < BIN_NUM + 2; ++i){
					auto low = _delta*(i - 1) + _min;
					auto high = _delta*i + _min;

					if (i == 0) std::cout << std::fixed << std::setprecision(disp_precision) << "\n[-∞" << Space(keta-2) << "," << std::setw(keta + 1) << high << ")" << "：" << std::setw(ctketa) << _count[i] << " ";
					else if (i == BIN_NUM + 1) std::cout << std::fixed << std::setprecision(disp_precision) << "\n[" << std::setw(keta + 1) << low << ",+∞" << Space(keta - 2) << ")" << "：" << std::setw(ctketa) << _count[i] << " ";
					else std::cout << std::fixed << std::setprecision(disp_precision) << "\n[" << std::setw(keta+1) << low << "," << std::setw(keta+1) << high << ")" << "：" << std::setw(ctketa) << _count[i] << " ";
					
					for (int j = 1; dbar*j <= _count[i]; ++j) printf("|");
				}
				std::cout << std::resetiosflags(std::ios_base::floatfield) << "\n\n";
			}
	};

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

	// 削除関連の関数群
	namespace Eraser{

		#define Sig_Eraser_RemoveDuplicates_Impl(Container) \
			auto end = std::unique(list.begin(), list.end());\
			auto removes = need_removes ? Container(end, list.end()) : Container();\
			list.erase(end, list.end())\


		//コンテナの要素から重複したものを削除
		//args -> list: コンテナ, need_removes: 削除した要素を戻り値で受け取るか, is_sorted: コンテナがソート済みか 
		//return -> 削除要素
		template <class T, template<class T_, class = std::allocator<T_>> class Container>
		inline Container<T> RemoveDuplicates(Container<T>& list, bool need_removes, bool is_sorted = false)
		{
			if (!is_sorted) std::sort(list.begin(), list.end());

			Sig_Eraser_RemoveDuplicates_Impl(Container<T>);

			return std::move(removes);
		}
		template < class T>
		inline std::list<T> RemoveDuplicates(std::list<T>& list, bool need_removes, bool is_sorted = false)
		{
			if (!is_sorted) list.sort();

			Sig_Eraser_RemoveDuplicates_Impl(std::list<T>);

			return std::move(removes);
		}
	
		#if ENABLE_BOOST
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
			for(auto it =list.begin(), end = list.end(); it != end;){
				if(*it == remove){
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
			for(auto it =list.begin(), end = list.end(); it != end;){
				if(remove_pred(*it)){
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
			if(!list.empty()) list.erase( std::remove_if(list.begin(), list.end(), remove_pred), list.end());
			return presize != list.size();
		}
	}

/* 文字列処理 */
	namespace String{

	#if ENABLE_BOOST

		//HTML風にタグをエンコード・デコードする
		//例：　<TAG>text<TAG>
		template <class String>
		class TagText
		{
			const String _tel;
			const String _ter;

		public:
			TagText(String tag_encloser_left, String tag_encloser_right) : _tel(tag_encloser_left), _ter(tag_encloser_right){};

			String Encode(String const& src, String const& tag) const{
				auto tag_str = _tel + tag + _ter;
				return tag_str + src + tag_str;
			}

			template < template < class T, class Allocator = std::allocator<T>> class Container = std::vector >
			String Encode(Container<String> const& src, Container<String> const& tag);

			maybe<String> Decode(String const& src, String const& tag) const{
				auto tag_str = _tel + tag + _ter;
				auto parse = Split(src, tag_str, false);
				return parse.empty() ? nothing : maybe<String>(parse[1]);
			}

			template < template < class T, class Allocator = std::allocator<T >> class Container = std::vector >
			maybe<Container<String>> Decode(String const& src, Container<String> const& tag);
		};

		template <class String>
		template < template < class T, class Allocator = std::allocator<T >> class Container = std::vector >
		String TagText<String>::Encode(Container<String> const& src, Container<String> const& tag)
		{
			String result;
			auto size = std::min( src.size(), tag.size());
			for (uint i = 0; i < size; ++i){
				result += Encode(src[i], tag[i]);
			}
			return result;
		}

		template <class String>
		template < template < class T, class Allocator = std::allocator<T >> class Container = std::vector >
		maybe<Container<String>> TagText<String>::Decode(String const& src, Container<String> const& tag)
		{
			Container<String> result;
			for (auto const& e : tag){
				if(auto d = Decode(src, e)) result.push_back(*d);
			}
			return result.empty() ? nothing : maybe<Container<String>>(std::move(result));
		}
	
	#endif

		//文字列をある文字を目印に分割する
		template < class String, template <class T_, class = std::allocator<T_>> class Container = std::vector >
		Container<String> Split(String src, typename std::common_type<String>::type const& delim, bool ignore_blank = true)
		{
			Container<String> result;
			int const mag = delim.size();
			int cut_at;

			while( (cut_at = src.find(delim)) != src.npos ){
				 if(!ignore_blank || cut_at > 0) result.push_back(src.substr(0, cut_at));
				 src = src.substr(cut_at + mag);
			}
			if(!ignore_blank || src.length() > 0){
				 result.push_back(src);
			}

			return std::move(result);
		}

		template < template <class T_, class = std::allocator<T_>> class Container = std::vector >
		Container<std::string> Split(char const* src, char const* delim)
		{
			return Split<std::string, Container>(std::string(src), delim);
		}

		template < template < class T_, class = std::allocator<T_>> class Container = std::vector >
		Container<std::wstring> Split(wchar_t const* src, wchar_t const* delim)
		{
			return Split<std::wstring, Container>(std::wstring(src), delim);
		}

	/*
	#if ENABLE_BOOST

		//コンテナに格納された全文字列を結合して1つの文字列に(delimiterで区切り指定)
		template < class T, template < class T, class = std::allocator<T >> class Container >
		inline std::string CatStr(Container<T> const& container, std::string delimiter = "")
		{
			std::string tmp;

			for (auto const& src : container){
				tmp += (boost::format("%1%%2%") % src % delimiter).str();
			}
			return std::move(tmp);
		}
	#endif
	*/
	
		//コンテナに格納された全文字列を結合して1つの文字列に(delimiterで区切り指定)
		template < class T, template < class T_, class = std::allocator<T_>> class Container >
		inline std::string CatStr(Container<T> const& container, std::string delimiter = "")
		{
			std::ostringstream ostream;

			for (uint i = 0; i < container.size()-1; ++i){
				ostream << container[i] << delimiter;
			}
			ostream << container[container.size() - 1];
			return ostream.str();
		}

		template < class T, template < class T_, class = std::allocator<T_>> class Container >
		inline std::wstring CatWStr(Container<T> const& container, std::wstring delimiter = L"")
		{
			std::wostringstream ostream;

			for (auto const& src : container){
				ostream << src << delimiter;
			}
			return ostream.str();
		}

		template <class STRING>
		struct Map2Regex{
			typedef void type;
		};
		template <>
		struct Map2Regex<std::string>{
			typedef std::regex type;
		};
		template <>
		struct Map2Regex<std::wstring>{
			typedef std::wregex type;
		};
	
		template <class STRING>
		struct Map2Smatch{
			typedef void type;
		};
		template <>
		struct Map2Smatch<std::string>{
			typedef std::smatch type;
		};
		template <>
		struct Map2Smatch<std::wstring>{
			typedef std::wsmatch type;
		};


		//expressionに含まれる文字に関して、正規表現の特殊文字をエスケープする
		template <class String>
		String RegexEscaper(String expression)
		{
			static const std::wregex escape_reg(L"([(){}\\[\\]|^?$.+*\\\\])");
			return std::regex_replace(expression, escape_reg, L"\\$1");
		}

		template <class String>
		typename Map2Regex<String>::type RegexMaker(String const& expression)
		{
			return typename Map2Regex<String>::type(RegexEscaper(expression));
		}

	#if ENABLE_BOOST

		//std::regex_search のラッパ関数。
		//return -> maybe ? [マッチした箇所の順番][マッチ内の参照の順番. 0は全文, 1以降は参照箇所] : nothing
		//例：
		//src = "test tes1 tes2"
		//expression = std::regex("tes(\\d)")
		//return -> [[tes, 1], [tes, 2]]
		template < class String, template<class T, class = std::allocator<T>> class Container = std::vector >
		maybe< Container< Container<String>>> RegexSearch(String src, typename Map2Regex<String>::type expression)
		{
			Container<Container<String>> d;
			maybe<Container<Container<String>>> result(d);
			typename Map2Smatch<String>::type match;

			while( std::regex_search(src, match, expression) ){
				result->push_back(Container<String>());
				for(auto const& m : match) (*result)[result->size()-1].push_back(m);
				src = match.suffix().str();
			}

			return result->empty() ? nothing : std::move(result);
		}

		//expressionに含まれる文字に関して、正規表現の特殊文字をエスケープしてから処理（推奨）
		template < class String, template < class T, class = std::allocator<T >> class Container = std::vector >
		maybe< Container< Container<String>>> RegexSearch(String src, String expression)
		{		
			return RegexSearch(src, RegexMaker(expression));
		}

	#endif

		//UTF-16 to Shift-JIS
		inline std::string WSTRtoSTR(const std::wstring &src)
		{
			size_t mbs_size = src.length() * MB_CUR_MAX + 1;
			if(mbs_size < 2 || src == L"\0") return std::string();
			char *mbs = new char[mbs_size];
			size_t num;

			wcstombs_s(&num, mbs, mbs_size, src.c_str(), src.length() * MB_CUR_MAX + 1);
			std::string dest(mbs);
			delete [] mbs;

			return std::move(dest);
		}

		inline std::vector<std::string> WSTRtoSTR(std::vector<std::wstring> const& strvec)
		{
			std::vector<std::string> result;
			for(auto const& str : strvec) result.push_back( WSTRtoSTR(str) );
			return std::move(result);
		}

		//Shift-JIS to UTF-16
		inline std::wstring STRtoWSTR(const std::string &src)
		{
			size_t wcs_size = src.length() + 1;
			if(wcs_size < 2|| src == "\0") return std::wstring();
			//std::cout << src << std::endl;
			wchar_t *wcs = new wchar_t[wcs_size];
			size_t num;

			mbstowcs_s(&num, wcs, wcs_size, src.c_str(), src.length() + 1);
			std::wstring dest(wcs);
			delete [] wcs;

			return std::move(dest);
		}

		inline std::vector<std::wstring> STRtoWSTR(std::vector<std::string> const& strvec)
		{
			std::vector<std::wstring> result;
			for(auto const& str : strvec) result.push_back( STRtoWSTR(str) );
			return std::move(result);
		}
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

/* 入出力 */
	namespace File{

		//ディレクトリ・ファイルパスの末尾に'/'or'\\'があるかチェックし、付けるか外すかどうか指定
		inline std::wstring DirpassTailModify(std::wstring const& directory_pass, bool const has_slash)
		{
			if(directory_pass.empty()) return directory_pass;

			auto tail = directory_pass.back();

			if(has_slash){
				//付ける場合
				if(tail == '/' || tail == '\\') return directory_pass;
				else return (directory_pass + L"/");
			}
			else{
				if(tail != '/' && tail != '\\') return directory_pass;
				else{
					auto tmp = directory_pass;
					tmp.pop_back();
					return tmp;
				}
			}

		};


		//指定ディレクトリにあるファイル名を取得
		//args -> empty_dest: 空のコンテナ, directry_pass: 読み込み先のフォルダパス
		//読み込み失敗: throw(std::invalid_argument)
		inline void GetFileNames(std::vector<std::wstring>& empty_dest, std::wstring const& directory_pass) throw(std::invalid_argument)
		{
			WIN32_FIND_DATA fd;
			auto pass = DirpassTailModify(directory_pass, true) + L"*.*";
			auto hFind = FindFirstFile(pass.c_str(), &fd);

			if (hFind == INVALID_HANDLE_VALUE){
				throw std::invalid_argument("error: invalid directory_pass");
			}
			else{
				do{
					//フォルダは無視
					if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !(fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
					{
						empty_dest.push_back(std::wstring(fd.cFileName));
					}
				} while (FindNextFile(hFind, &fd));

				FindClose(hFind);
			}
		}

#if ENABLE_BOOST

		//指定ディレクトリにあるファイル名を取得
		//args -> directry_pass: 読み込み先のフォルダパス
		//読み込み失敗: return -> maybe == nothing
		inline maybe<std::vector<std::wstring>> GetFileNames(std::wstring const& directory_pass)
		{
			std::vector<std::wstring> tmp;
			try{
				GetFileNames(tmp, directory_pass);
			}
			catch (...){
				return nothing;
			}
			return tmp;
		}

#endif

		//指定ディレクトリにあるファイル名をフルパスで取得
		//args -> empty_dest: 空のコンテナ, directry_pass: 読み込み先のフォルダパス 
		//読み込み失敗: throw(std::invalid_argument)
		inline void GetFilePasses(std::vector<std::wstring>& empty_dest, std::wstring const& directory_pass) throw(std::invalid_argument)
		{
			auto pass = DirpassTailModify(directory_pass, true);
			std::vector<std::wstring> names;
			GetFileNames(names, pass);

			try{
				for (auto const& e : names) empty_dest.push_back(pass + e);
			}
			catch (...){
				throw std::invalid_argument("error: invalid directory_pass");
			}
		}

#if ENABLE_BOOST

		//指定ディレクトリにあるファイル名をフルパスで取得
		//args -> directry_pass: 読み込み先のフォルダパス
		//読み込み失敗: throw(std::invalid_argument)
		inline maybe<std::vector<std::wstring>> GetFilePasses(std::wstring const& directory_pass)
		{
			std::vector<std::wstring> tmp;
			try{
				GetFilePasses(tmp, directory_pass);
			}
			catch (...){
				return nothing;
			}
			return tmp;
		}

#endif

		//for type map
		template <class FILE_STRING> struct OFS_SELECTION{};
		template<> struct OFS_SELECTION<char const*>{
			typedef std::ofstream fstream;
			typedef std::ostreambuf_iterator<char> fstreambuf_iter;
		};
		template<> struct OFS_SELECTION<std::string>{
			typedef std::ofstream fstream;
			typedef std::ostreambuf_iterator<char> fstreambuf_iter;
		};
		template<> struct OFS_SELECTION<wchar_t const*>{
			typedef std::wofstream fstream;
			typedef std::ostreambuf_iterator<wchar_t> fstreambuf_iter;
		};
		template<> struct OFS_SELECTION<std::wstring>{
			typedef std::wofstream fstream;
			typedef std::ostreambuf_iterator<wchar_t> fstreambuf_iter;
		};

		template <class FILE_STRING> struct IFS_SELECTION{};
		template<> struct IFS_SELECTION<std::string>{
			typedef std::ifstream fstream;
			typedef std::istreambuf_iterator<char> fstreambuf_iter;
		};
		template<> struct IFS_SELECTION<std::wstring>{
			typedef std::wifstream fstream;
			typedef std::istreambuf_iterator<wchar_t> fstreambuf_iter;
		};

		template <class NUM> struct S2NUM_SELECTION{};
		template <> struct S2NUM_SELECTION<int>{
			 int operator()(std::string s){ return std::stoi(s); }
		};
		template <> struct S2NUM_SELECTION<long>{
			long operator()(std::string s){ return std::stol(s); }
		};
		template <> struct S2NUM_SELECTION<long long>{
			long long operator()(std::string s){ return std::stoll(s); }
		};
		template <> struct S2NUM_SELECTION<unsigned int>{
			unsigned int operator()(std::string s){ return std::stoul(s); }
		};
		template <> struct S2NUM_SELECTION<unsigned long>{
			unsigned long operator()(std::string s){ return std::stoul(s); }
		};
		template <> struct S2NUM_SELECTION<unsigned long long>{
			unsigned long long operator()(std::string s){ return std::stoull(s); }
		};
		template <> struct S2NUM_SELECTION<float>{
			float operator()(std::string s){ return std::stof(s); }
		};
		template <> struct S2NUM_SELECTION<double>{
			double operator()(std::string s){ return std::stod(s); }
		};
		
		enum class WriteMode{ overwrite, append };


		inline void RemakeFile(std::wstring const& file_pass)
		{
			std::ofstream ofs(file_pass);
			ofs << "";
		}

		//-- Save Text

		template <class String>
		inline void SaveLine(String const& src, typename OFS_SELECTION<String>::fstream& ofs)
		{
			ofs << src << std::endl;
		}

		template <class String>
		inline void SaveLine(std::vector<String> const& src, typename OFS_SELECTION<String>::fstream& ofs)
		{
			typename OFS_SELECTION<String>::fstreambuf_iter streambuf_iter(ofs);
			for (auto const& str : src){
				std::copy(str.begin(), str.end(), streambuf_iter);
				streambuf_iter = '\n';
			}
		}

		//ファイルへ1行ずつ保存
		//args -> src: 保存対象, file_pass: 保存先のディレクトリとファイル名（フルパス）, open_mode: 上書き(overwrite) or 追記(append)
		template <class String>
		inline void SaveLine(String src, std::wstring const& file_pass, WriteMode mode)
		{
			static bool first = true;
			if (first){
				std::locale::global(std::locale(""));
				first = false;
			}

			std::ios::open_mode const open_mode = mode == WriteMode::overwrite ? std::ios::out : std::ios::out | std::ios::app;
			typename OFS_SELECTION<typename std::decay<String>::type>::fstream ofs(file_pass, open_mode);
			SaveLine(src, ofs);
		}
		template <class String>
		inline void SaveLine(std::vector<String> const& src, std::wstring const& file_pass, WriteMode mode)
		{
			static bool first = true;
			if (first){
				std::locale::global(std::locale(""));
				first = false;
			}

			std::ios::open_mode const open_mode = mode == WriteMode::overwrite ? std::ios::out : std::ios::out | std::ios::app;
			typename OFS_SELECTION<String>::fstream ofs(file_pass, open_mode);
			SaveLine(src, ofs);
		}

		template <class Num>
		inline void SaveLineNum(std::vector<Num> const& src, std::wstring const& file_pass, WriteMode mode, std::string delimiter = "\n")
		{
			SaveLine(String::CatStr(src, delimiter), file_pass, mode);
		}


		//-- Read Text

		template <class R>
		inline void ReadLine(std::vector<R>& empty_dest, typename IFS_SELECTION<R>::fstream& ifs, std::function< R(typename std::conditional<std::is_same<typename IFS_SELECTION<R>::fstream, std::ifstream>::value, std::string, std::wstring>::type)> const& conv = nullptr)
		{
			typename std::conditional<std::is_same<typename IFS_SELECTION<R>::fstream, std::ifstream>::value, std::string, std::wstring>::type line;

			while (ifs && getline(ifs, line)){
				conv ? empty_dest.push_back(conv(std::move(line))) : empty_dest.push_back(std::move(line));
			}
		}
		
		template <class R>
		inline void ReadLine(std::vector<R>& empty_dest, std::wstring const& file_pass, std::function< R(typename std::conditional<std::is_same<typename IFS_SELECTION<R>::fstream, std::ifstream>::value, std::string, std::wstring>::type)> const& conv = nullptr)
		{
			typename IFS_SELECTION<R>::fstream ifs(file_pass);
			if (!ifs){
				wprintf(L"file open error: %s \n", file_pass);
				return;
			}
			ReadLine(empty_dest, ifs);
		}

		template <class Num>
		inline void ReadLineNum(std::vector<Num>& empty_dest, std::wstring const& file_pass)
		{
			typename IFS_SELECTION<std::string>::fstream ifs(file_pass);
			S2NUM_SELECTION<Num> conv;
			std::string line;
			if (!ifs){
				wprintf(L"file open error: %s \n", file_pass);
				return;
			}
			while (ifs && getline(ifs, line))
				empty_dest.push_back(conv(std::move(line)));
		}

#if ENABLE_BOOST

		//読み込み失敗: return -> maybe == nothing
		template <class String>
		inline maybe<std::vector<String>> ReadLine(typename IFS_SELECTION<String>::fstream& ifs)
		{
			typedef std::vector<String> R;
			R tmp;
			ReadLine(tmp, ifs);
			return tmp.size() ? maybe<R>(std::move(tmp)) : nothing;
		}

		template <class String>
		inline maybe<std::vector<String>> ReadLine(std::wstring const& file_pass)
		{
			typename IFS_SELECTION<String>::fstream ifs(file_pass);
			if (!ifs){
				std::wcout << L"file open error: " << file_pass << std::endl;
				return nothing;
			}
			return ReadLine<String>(ifs);
		}

		template <class Num>
		inline maybe<std::vector<Num>> ReadLineNum(std::wstring const& file_pass)
		{
			std::vector<Num> tmp;
			ReadLineNum<Num>(tmp, file_pass);
			return tmp.size() ? maybe<std::vector<Num>>(std::move(tmp)) : nothing;
		}

#endif

		//csvで保存
		template <class Num>
		inline void SaveCSV(std::vector<std::vector<Num>> const& data, std::vector<std::string> const& row_names, std::vector<std::string> const& col_names, std::wstring const& out_fullpass)
		{
			std::ofstream ofs(out_fullpass);

			//first row: field name
			ofs << ",";
			for (uint i = 1; i < data[0].size() + 1; ++i){
				auto name = i-1 < col_names.size() ? col_names[i - 1] : "";
				ofs << i << ". " << name << ",";
			}
			ofs << "\n";

			//first col: field name
			for (uint j = 0; j < data.size(); ++j){
				auto name = j < row_names.size() ? row_names[j] : "";
				ofs << j+1 << ". " << name << ",";

				for (auto e : data[j]){
					ofs << e << ",";
				}
				ofs << "\n";
			}
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