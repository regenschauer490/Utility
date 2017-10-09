/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIGUTIL_ZIP_HPP
#define SIGUTIL_ZIP_HPP

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

	auto z = zip(data1, data2, data3, data4);	// array< std::tuple<int, int, double, std::string>, 4>

	uint size = z.size();	// 3
	auto e1 = z[1];		// std::tuple<int, int, double, std::string>{ 2, -3, -2.2, "aa" }
	\endcode
*/
template <class... Cs
#if (SIG_MSVC_ENV && !(SIG_MSVC_VER <= 140)) || (SIG_GCC_ENV) || (SIG_CLANG_ENV)
#endif
	>
auto zip(Cs&&... lists)
{
	static_assert(
		And(impl::container_traits<
			typename impl::remove_const_reference<Cs>::type
		>::exist...),
		"Cs<T>...: unmatch types T."
	);

	return variadicZipWith(
#if SIG_ENABLE_MOVEITERATOR
		[](typename impl::forward_element<Cs>::type... vs){
			return std::make_tuple(std::forward<typename impl::forward_element<Cs>::type>(vs)...);
		},
#else
		[](typename impl::container_traits<typename impl::remove_const_reference<Cs>::type>::value_type... vs){
			return std::make_tuple(vs...);
		},
#endif
		std::forward<Cs>(lists)...
	);
}


/// タプルのコンテナから、指定したindexのコンテナを取り出す
/**
	[(a, b, ...)] -> [a0]

	\tparam Index 取り出すインデックス

	\param list_tuple データが格納されたタプルのコンテナ [(a, b, ...)]（\ref sig_container ）

	\return 結果のコンテナ（コンテナはc_tupleと同じ種類）

	\code
	const array<std::tuple<int, std::string>, 3> data{	// sig::array
		std::make_tuple(1, "a"),
		std::make_tuple(2, "b"),
		std::make_tuple(3, "c")
	};

	auto uz1 = unzip<0>(data);	// array<int, 3>{ 1, 2, 3 }
	auto uz2 = unzip<1>(data);	// array<std::string, 3>{ "a", "b", "c" }
	\endcode
*/
template <uint Index, class CT,
	class T = typename std::tuple_element<Index, typename impl::container_traits<typename impl::remove_const_reference<CT>::type>::value_type>::type,
	class R = std::vector<T>
>
auto unzip(CT&& list_tuple) ->R
{
	using ET = typename impl::forward_element<CT>::type;

	R result = impl::container_traits<R>::make(list_tuple.size());

	for (auto&& t : std::forward<CT>(list_tuple)){
		impl::container_traits<R>::add_element(result, std::get<Index>(std::forward<ET>(t)));
	}
	return result;
}

namespace impl
{
template<class CT, uint I = 0,
	class CTR = typename impl::remove_const_reference<CT>::type,
	typename std::enable_if<I + 1 == std::tuple_size<typename impl::container_traits<CTR>::value_type>::value, void>::type*& = enabler
>
auto unzipImpl_(CT&& c_tuple)
{
	return std::make_tuple(unzip<I>(std::forward<CT>(c_tuple)));
}
template<class CT, uint I = 0,
	class CTR = typename impl::remove_const_reference<CT>::type,
	typename std::enable_if<I + 1 < std::tuple_size<typename impl::container_traits<CTR>::value_type>::value, void>::type*& = enabler
>
auto unzipImpl_(CT&& c_tuple)
{
	return std::tuple_cat(std::make_tuple(unzip<I>(std::forward<CT>(c_tuple))), impl::unzipImpl_<CT, I + 1>(std::forward<CT>(c_tuple)));
}
}	// impl

/// タプルのコンテナから、コンテナのタプルを作る
/**
	[(a, b, ...)] -> ([a], [b], ...)

	\param c_tuple データが格納されたタプルのコンテナ [(a, b, ...)]（\ref sig_container ）

	\return 結果のコンテナを持つタプル（コンテナはc_tupleと同じ種類）

	\code
	const array<std::tuple<int, std::string>, 3> data{	// sig::array
		std::make_tuple(1, "a"),
		std::make_tuple(2, "b"),
		std::make_tuple(3, "c")
	};

	auto uz = unzip(data);	// std::tuple< array<int, 3>, array<std::string, 3> >

	auto e1 = std::get<0>(uz);		// array<int, 3>{ 1, 2, 3 }
	auto e2 = std::get<1>(uz);		// array<std::string, 3>{ "a", "b", "c" }
	\endcode
*/
template <class CT>
auto unzip(CT&& c_tuple)
{
	return impl::unzipImpl_(std::forward<CT>(c_tuple));
}


#if SIG_ENABLE_TUPLE_ZIP
namespace impl
{
	template <class TC, size_t... Is>
	auto zipImpl_(TC&& t_lists, std::index_sequence<Is...>)
	{
		return zip(std::get<Is>(std::forward<TC>(t_lists))...);
	}
}	// impl

/// コンテナのタプルから、タプルのコンテナを作る
/**
	([a], [b], ...) -> [(a, b, ...)] \n
	t_listsのいずれかのコンテナが末尾に到達するまでタプルが作られる

	\param t_lists... データが格納されたコンテナのタプル ([a], [b], ...)（\ref sig_container ）

	\return 処理結果のコンテナ [(a, b, ...)]（コンテナはt_listsの[a]と同じ種類）

	\code
	\endcode
*/
template <class... Cs>
auto tzip(std::tuple<Cs...> const& t_lists)
{
	using Indices = std::make_index_sequence<sizeof...(Cs)>;
	return impl::zipImpl_(t_lists, Indices());
}

template <class... Cs>
auto tzip(std::tuple<Cs...>&& t_lists)
{
	using Indices = std::make_index_sequence<sizeof...(Cs)>;
	return impl::zipImpl_(std::move(t_lists), Indices());
}
#endif

}
#endif
