#ifndef __SIG_UTIL_FUNCTIONAL__
#define __SIG_UTIL_FUNCTIONAL__

#include "sigutil.hpp"

/* 関数型プログラミング サポート */

//注：渡すコンテナが巨大な場合にはコピーのコストに留意

namespace sig
{
	//(a -> b) -> [a] -> [b]
	//1引数高階関数
	template <class F, class C>
	auto Map(F const& func, C const& container)
	{
		using OutputType = typename container_traits<C>::template rebind<decltype(eval(func, std::declval<typename container_traits<C>::value_type>()))>;

		OutputType result;
		for (auto const& v : container) container_traits<OutputType>::add_element(result, eval(func, v));
		return std::move(result);
	}


	//(a -> b -> c) -> [a] -> [b] -> [c]
	//2引数高階関数
	template <class F, class C1, class C2>
	auto ZipWith(F const& func, C1 const& container1, C2 const& container2)
	{
		using OutputType = typename container_traits<C1>::template rebind<decltype(eval(
			func,
			std::declval<typename container_traits<C1>::value_type>(),
			std::declval<typename container_traits<C2>::value_type>()
		))>;

		OutputType result;
		const uint length = std::min(container1.size(), container2.size());
		auto it1 = std::begin(container1), end1 = std::end(container1);
		auto it2 = std::begin(container2), end2 = std::end(container2);
		for (uint i = 0; i < length; ++i, ++it1, ++it2){
			container_traits<OutputType>::add_element(result, eval(func, *it1, *it2));
		}

		return std::move(result);
	}
/*
	//[a] -> [b] -> (a -> b -> void) -> void
	//戻り値の型がvoidの場合
	template <class R, class A, class B, template < class T, class = std::allocator<T >> class Container, typename std::enable_if<std::is_same<R, void>::value>::type*& = enabler>
	void ZipWith(Container<A> const& list1, Container<B> const& list2, std::function<void(typename std::common_type<A>::type, typename std::common_type<B>::type)> const& func)
	{
		const uint length = list1.size() < list2.size() ? list1.size() : list2.size();
		uint i = 0;
		for (auto it1 = std::begin(list1), it2 = std::begin(list2), end1 = std::end(list1), end2 = std::end(list2); i < length; ++i, ++it1, ++it2) func(*it1, *it2);
	}
*/

	//[a] -> [b] -> [(a, b)]
	//
	template <class  C1, class C2>
	auto Zip(C1 const& container1, C2 const& container2)
	{
		using R1 = typename container_traits<C1>::value_type;
		using R2 = typename container_traits<C2>::value_type;
		return ZipWith([](R1 const& v1, R2 const& v2){
			return std::make_tuple(v1, v2); 
		}, container1, container2);
	}

	//uint -> a -> [a]
	//値を複製したコンテナを返す
	template <class T, class C = std::vector<T>>
	C Fill(uint n, T const& value){
		C result;
		for (uint i = 0; i<n; ++i) container_traits<C>::add_element(result, value);
		return std::move(result);
	}

	//[a] -> [a]
	//コンテナの要素を逆転させたコンテナを返す
	template <class C>
	C Reverse(C const& container)
	{
		C result = container;
		std::reverse(std::begin(result), std::end(result));
		return std::move(result);
	}

	//[a] -> [a] -> [a]
	//コンテナの結合
	template <class C>
	C Merge(C const& container1, C const& container2)
	{
		auto result = container1;
		container_traits<C>::concat(result, container2);
		return std::move(result);
	}

	//[a] -> [b] -> [c]
	//コンテナの結合
	template <class C, class C1, class C2>
	C Merge(C1 const& container1, C2 const& container2)
	{
		C result;
		for (auto v : container1) container_traits<C>::add_element(result, v);
		for (auto v : container2) container_traits<C>::add_element(result, v);
		return std::move(result);
	}

	//uint -> [a] -> [a]
	//コンテナの先頭からn個を取り出したコンテナを返す (順序の無いコンテナでは実装依存)
	template <class C>
	C Take(uint n, C const& container)
	{
		C result;
		uint i = 0;
		for (auto it = std::begin(container); i < n; ++i, ++it) container_traits<C>::add_element(result, *it);
		return std::move(result);
	}

	//uint -> [a] -> [a]
	//コンテナの先頭からn個を削除したコンテナを返す (順序の無いコンテナでは実装依存)
	template <class C>
	C Drop(uint n, C const& container)
	{
		C result;
		uint i = 0;
		auto it = std::begin(container), end = std::end(container);
		for (; i < n && it != end; ++i, ++it) ;
		for (; it != end; ++i, ++it) container_traits<C>::add_element(result, *it);
		return std::move(result);
	}

}

#endif