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
		return HigherOrderFunction(func, container);
	}


	//(a -> b -> c) -> [a] -> [b] -> [c]
	//2引数高階関数
	template <class F, class C1, class C2>
	auto ZipWith(F const& func, C1 const& container1, C2 const& container2)
	{
		return HigherOrderFunction(func, container1, container2);
	}

	//n引数高階関数
	template <class F, class C1, class... Cs>
	auto HigherOrderFunction(F const& func, C1 const& container1, Cs const&... containers)
	{
		using OutputType = typename container_traits<C1>::template rebind<decltype(eval(
			func,
			std::declval<typename container_traits<C1>::value_type>(),
			std::declval<typename container_traits<Cs>::value_type>()...
		))>;

		OutputType result;
		const uint length = Min(container1.size(), containers.size()...);
		HigherOrderFunctionImpl(length, result, func, std::begin(container1), std::begin(containers)...);

		return std::move(result);
	}

	template <class OutputType, class F, class... Its>
	void HigherOrderFunctionImpl(uint loop, OutputType& result, F const& func, Its... iterators)
	{
		for (uint i = 0; i < loop; ++i, IncrementIterator(iterators...)){
			container_traits<OutputType>::add_element(result, eval(func, DereferenceIterator(iterators)...));
		}
	}


	//[a] -> [b] -> [(a, b)]
	//ペアを作る
	template <class... Cs>
	auto Zip(Cs const&... containers)
	{
		return HigherOrderFunction([](typename container_traits<Cs>::value_type const&...  vs){
			return std::make_tuple(vs...); 
		}, containers...);
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