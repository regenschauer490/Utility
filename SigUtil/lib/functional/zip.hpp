/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_ZIP_HPP
#define SIG_UTIL_ZIP_HPP

#include "high_order.hpp"

/// \file zip.hpp zip関数, unzip関数

namespace sig
{
/// 複数のコンテナから、タプルのコンテナを作る 
/**
	[a] -> [b] -> ... -> [(a, b, ...)] \n
	listsのいずれかの末尾に到達するまでタプルが作られる

	\param lists... データが格納された複数のコンテナ [a],[b],...（\ref sig_container ）

	\return 処理結果のコンテナ [(a, b, ...)]（コンテナはlistsの[a]と同じ種類）

	\code
	const array<int, 4> data1{ 1, 2, 3, 4 };		// sig::array
	const std::vector<int> data2{ 1, -3, 5, 2, 10 };
	const std::list<double> data3{ 1.1, -2.2, 3.3 };
	const std::set<std::string> data4{ "a", "aa", "aaa" };

	auto zipped = zip(data1, data2, data3, data4);	// array< std::tuple<int, int, double, std::string>, 4>

	uint size = zipped.size();	// 3
	auto e1 = zipped[1];		// std::tuple<int, int, double, std::string>{ 2, -3, -2.2, "aa" }
	\endcode
*/
// for variadic parameter, const lvalue reference
template <class... Cs
#if !SIG_MSVC_ENV || !(MSC_VER < 1900)
	, typename std::enable_if< And(impl::container_traits<Cs>::exist...) >::type*& = enabler
#endif
	>
auto zip(Cs const&... lists)
{
	return variadicZipWith([](typename impl::container_traits<Cs>::value_type const&... vs){
		return std::make_tuple(vs...);
	}, lists...);
}

#if !SIG_MSVC_ENV || !(_MSC_VER < 1900)

/**
	
*/
// for variadic parameter, rvalue reference
template <class C1, class... Cs,
	typename std::enable_if<!std::is_lvalue_reference<C1>::value && !And(std::is_lvalue_reference<Cs>::value...)>::type*& = enabler,
	typename std::enable_if< And(impl::container_traits<Cs>::exist...) >::type*& = enabler
>
auto zip(C1&& list1, Cs&&... lists)
{
	using RT = typename impl::container_traits<C1>::template rebind<
		std::tuple<typename impl::container_traits<C1>::value_type, typename impl::container_traits<Cs>::value_type...>
	>;

	const uint length = min(list1.size(), lists.size()...);
	RT result;
	iterative_make(length, result, [](typename impl::container_traits<C1>::value_type&& v1, typename impl::container_traits<Cs>::value_type&&... vs){
		return std::make_tuple(std::move(v1), std::move(vs)...);
	}, std::make_move_iterator(std::begin(list1)), std::make_move_iterator(std::begin(lists))...);

	return result;
}
#endif


/// タプルのコンテナから、指定したindexのコンテナ(templateパラメータで指定)を取り出す
/**
	[(a, b, ...)] -> [a0]
*/
template <size_t Index, class CT>
auto unzip(CT const& c_tuple)
{
	using T = typename std::tuple_element<Index, typename impl::container_traits<CT>::value_type>::type;
	using C = std::vector<T>;
	C result;

	for (auto const& e : c_tuple){
		impl::container_traits<C>::add_element(result, std::get<Index>(e));
	}
	return result;
}

template <uint Index, class CT, typename std::enable_if<!std::is_lvalue_reference<CT>::value>::type*& = enabler>
auto unzip(CT&& c_tuple)
{
	using T = typename std::tuple_element<Index, typename impl::container_traits<CT>::value_type>::type;
	using C = std::vector<T>;
	C result;

	for (auto& t : c_tuple){
		impl::container_traits<C>::add_element(result, std::get<Index>(std::move(t)));
	}
	return result;
}

namespace impl
{
template<class CT, size_t I = 0, typename std::enable_if<I + 1 == std::tuple_size<typename impl::container_traits<CT>::value_type>::value, void>::type*& = enabler>
auto unzipImpl_(CT const& c_tuple)
{
	return std::make_tuple(unzip<I>(c_tuple));
}
template<class CT, size_t I = 0, typename std::enable_if<I + 1 < std::tuple_size<typename impl::container_traits<CT>::value_type>::value, void>::type*& = enabler>
auto unzipImpl_(CT const& c_tuple)
{
	return std::tuple_cat(std::make_tuple(unzip<I>(c_tuple)), impl::unzipImpl_<CT, I + 1>(c_tuple));
}

template<class CT, uint I = 0, typename std::enable_if<I + 1 == std::tuple_size<typename impl::container_traits<CT>::value_type>::value, void>::type*& = enabler>
auto unzipImpl_(CT&& c_tuple)
{
	return std::make_tuple(unzip<I>(std::forward<CT>(c_tuple)));
}
template<class CT, uint I = 0, typename std::enable_if<I + 1 < std::tuple_size<typename impl::container_traits<CT>::value_type>::value, void>::type*& = enabler>
auto unzipImpl_(CT&& c_tuple)
{
	return std::tuple_cat(std::make_tuple(unzip<I>(std::forward<CT>(c_tuple))), impl::unzipImpl_<CT, I + 1>(std::forward<CT>(c_tuple)));
}
}	// impl

// [(a, b, ...)] -> ([a], [b], ...)
// タプルのコンテナから、コンテナのタプルを作る
template <class CT>
auto unzip(CT const& c_tuple)
{
	return impl::unzipImpl_(c_tuple);
}

template <class CT, typename std::enable_if<!std::is_lvalue_reference<CT>::value>::type*& = enabler>
auto unzip(CT&& c_tuple)
{
	return impl::unzipImpl_(std::forward<CT>(c_tuple));
}


#if SIG_GCC_GT4_9_0 || SIG_CLANG_GT_3_5 || !(_MSC_VER < 1900)
namespace impl
{
	template <class TC, size_t... I>
	auto zipImpl_(TC const& t_lists, std::index_sequence<I...>)
	{
		return zip(std::get<I>(t_lists)...);
	}
	template <class TC, size_t... I>
	auto zipImpl_(TC&& t_lists, std::index_sequence<I...>)
	{
		return zip(std::get<I>(std::forward<TC>(t_lists))...);
	}
}	// impl

// ([a], [b], ...) -> [(a, b, ...)]
// コンテナのタプルから、タプルのコンテナを作る
// for tuple, const lvalue reference
template <class... Cs, class Indices = std::make_index_sequence<sizeof...(Cs)>
	auto zip(std::tuple<Cs...> const& t_lists)
	{
		return impl::zipImpl_(t_lists, Indices());
	}

	// for tuple, rvalue reference
	template <class... Cs, class Indices = std::make_index_sequence<sizeof...(Cs)>
		auto zip(std::tuple<Cs...>&& t_lists)
		{
			return impl::zipImpl_(std::move(t_lists), Indices());
		}
#endif

}
#endif
