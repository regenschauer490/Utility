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
// for variadic parameter
template <class... Cs
#if !SIG_MSVC_ENV || !(SIG_MSVC_VER <= 140)
	, typename std::enable_if< And(impl::container_traits<Cs>::exist...) >::type*& = enabler
#endif
	>
auto zip(Cs&&... lists)
{
	return variadicZipWith(
		[&](typename impl::actual_element<Cs>::type... vs){
			return std::make_tuple(std::forward<typename impl::actual_element<Cs>::type>(vs)...);
		},
		std::forward<Cs>(lists)...
	);
}


/// タプルのコンテナから、指定したindexのコンテナ(templateパラメータで指定)を取り出す
/**
	[(a, b, ...)] -> [a0]
*/
template <uint Index, class CT>
auto unzip(CT&& c_tuple)
{
	using T = typename std::tuple_element<Index, typename impl::container_traits<typename impl::remove_const_reference<CT>::type>::value_type>::type;
	using R = std::vector<T>;
	using AT = typename impl::actual_element<CT>::type;

	R result;

	for (auto&& t : std::forward<CT>(c_tuple)){
		impl::container_traits<R>::add_element(result, std::get<Index>(std::forward<AT>(t)));
	}
	return result;
}

namespace impl
{
template<class CT, uint I = 0,
	class CTR = typename impl::remove_const_reference<CT>::type,
	typename std::enable_if<I + 1 == std::tuple_size<typename impl::container_traits<CTR>::value_type>::value, void>::type*& = enabler>
auto unzipImpl_(CT&& c_tuple)
{
	return std::make_tuple(unzip<I>(std::forward<CT>(c_tuple)));
}
template<class CT, uint I = 0,
	class CTR = typename impl::remove_const_reference<CT>::type,
	typename std::enable_if<I + 1 < std::tuple_size<typename impl::container_traits<CTR>::value_type>::value, void>::type*& = enabler>
auto unzipImpl_(CT&& c_tuple)
{
	return std::tuple_cat(std::make_tuple(unzip<I>(std::forward<CT>(c_tuple))), impl::unzipImpl_<CT, I + 1>(std::forward<CT>(c_tuple)));
}
}	// impl

/// タプルのコンテナから、コンテナのタプルを作る
/**
	[(a, b, ...)] -> ([a], [b], ...)
*/
template <class CT>
auto unzip(CT&& c_tuple)
{
	return impl::unzipImpl_(std::forward<CT>(c_tuple));
}


#if SIG_GCC_GT4_9_0 || SIG_CLANG_GT_3_5 || !(SIG_MSVC_VER <= 140)
namespace impl
{
	template <class TC, size_t... Is>
	auto zipImpl_(TC&& t_lists, std::index_sequence<Is...>)
	{
		return zip(std::get<Is>(std::forward<TC>(t_lists))...);
	}
}	// impl

// ([a], [b], ...) -> [(a, b, ...)]
// コンテナのタプルから、タプルのコンテナを作る
// for tuple
template <class... Cs, class Indices = std::make_index_sequence<sizeof...(Cs)>
auto zip(std::tuple<Cs...>&& t_lists)
{
	return impl::zipImpl_(std::forward<Cs...>(t_lists), Indices());
}
#endif

}
#endif