#ifndef __SIG_UTIL_FUNCTIONAL__
#define __SIG_UTIL_FUNCTIONAL__

#include "sigutil.hpp"

/* 関数型プログラミング サポート */

namespace sig
{
	//(a -> b) -> [a] -> [b]
	//1引数高階関数
	template <class C, class F>
	auto Map(F const& func, C const& container)
	{
		using OutputType = typename container_traits<C>::template rebind<decltype(eval(func, std::declval<typename container_traits<C>::value_type>()))>;

		OutputType result;
		for (auto const& v : container) container_traits<OutputType>::add_element(result, eval(func, v));
		return std::move(result);
	}


	//(a -> b -> c) -> [a] -> [b] -> [c]
	//2引数高階関数
	template <class C1, class C2, class F>
	auto ZipWith(F const& func, C1 const& container1, C2 const& container2)
	{
		using OutputType = typename container_traits<C1>::template rebind<decltype(eval(
			func,
			std::declval<typename container_traits<C1>::value_type>(),
			std::declval<typename container_traits<C2>::value_type>()
		))>;

		const uint length = container1.size() < container2.size() ? container1.size() : container2.size();
		OutputType result;
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

	//uint -> a -> [a]
	//値を複製したコンテナを返す
	template <class T, class C = std::vector<T>>
	C Fill(uint n, T const& value){
		C result;
		for (uint i = 0; i<n; ++i) container_traits<C>::add_element(result, value);
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
}

#endif