/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_SORT_HPP
#define SIG_UTIL_SORT_HPP

#include "../helper/type_traits.hpp"
#include "../functional/zip.hpp"
#include "../functional/rest.hpp"


/// \file sort.hpp ソートに関連する処理

namespace sig
{
/// 標準ソート関数のラッパ
/**
	\param container ソート対象のコンテナ（\ref sig_container ）
	\param binary_op 大小比較を行う関数オブジェクト

	\code
	array<double, 3> data1{ 3.3, 1.1, -2.2 };	// sig::array
	std::vector<int> data2{ 1, 5, 3, -2 };
	std::list<int> data3{ 1, 5, 3, -2 };

	sort(data1, std::less<double>());
	data1;			// { -2.2, 1.1, 3.3 }

	sort(data2, std::less<int>());
	data2;			// { -2, 1, 3, 5 }

	sort(data3, std::greater<int>());
	data3;			// { 5, 3, 1, -2 }
	\endcode
*/
// for sequence container which has random access iterator
template <class F, class C,
	class CR = typename impl::remove_const_reference<C>::type,
	class T = typename impl::sequence_container_traits<CR>::value_type,
	typename std::enable_if<impl::has_random_access_iter<CR>::value>::type*& = enabler
>
auto sort(
	C& container,
	F&& binary_op
	) ->decltype(impl::eval(std::forward<F>(binary_op), std::declval<T>(), std::declval<T>()), void())
{
	std::sort(std::begin(container), std::end(container), std::forward<F>(binary_op));
}

// for static container which has random access iterator
template <class F, class C,
	class CR = typename impl::remove_const_reference<C>::type,
	class T = typename impl::static_container_traits<CR>::value_type,
	class D = void,
	typename std::enable_if<impl::has_random_access_iter<CR>::value>::type*& = enabler
>
auto sort(
	C& container,
	F&& binary_op
	) ->decltype(impl::eval(std::forward<F>(binary_op), std::declval<T>(), std::declval<T>()), void())
{
	std::sort(std::begin(container), std::end(container), std::forward<F>(binary_op));
}

// for sequence container which doesn't have random access iterator
template <class F, class C,
	class CR = typename impl::remove_const_reference<C>::type,
	typename std::enable_if<!impl::has_random_access_iter<CR>::value>::type*& = enabler,
	class T = typename impl::sequence_container_traits<CR>::value_type
>
auto sort(
	C& container,
	F&& binary_op
	) ->decltype(impl::eval(std::forward<F>(binary_op), std::declval<T>(), std::declval<T>()), void())
{
	container.sort(std::forward<F>(binary_op));
}
	

/// ソート前の位置を保持してソート
/**
	ex: [30, 50, -10, 0] -> ([-10, 0, 30, 50], [2, 3, 0, 1])

	\param container ソート対象のコンテナ（\ref sig_container ）
	\param binary_op 大小比較を行う関数オブジェクト

	\return std::tuple<(ソート後のコンテナ), (ソート前のindexを記録したコンテナ)>

	\code
	const std::vector<int> cdata1{ 30, 50, -10, 0 };

	auto swi1 = sort_with_index(cdata1, std::less<int>());

	auto sorted1 = std::get<0>(swi1);			// { -10, 0, 30, 50 }
	auto original_index1 = std::get<1>(swi1);	// { 2, 3, 0, 1 }
	\endcode

	\code
	class TestInt{
		std::vector<int> v;
		bool emp;
	public:
		TestInt() : v(), emp(true){}
		TestInt(int i) : v(1, i), emp(false){}
		TestInt(TestInt const& s) : v(s.v), emp(false){}
		TestInt(TestInt&& s) : v(std::move(s.v)), emp(false){ s.emp = true; }

		TestInt& operator=(TestInt const& s){ v = s.v; emp = false; return *this; }
		TestInt& operator=(TestInt&& s){ v = std::move(s.v); emp = false; s.emp = true; return *this; }

		bool empty() const{ return emp; }
		int value() const{ return emp ? -1 : v[0]; }
	};
	
	array<TestInt, 4> data2{ 30, 50, -10, 0 };

	auto swi2 = sort_with_index(std::move(data2), [](TestInt const& l, TestInt const& r){ return l.value() > r.value(); });

	auto sorted1 = std::get<0>(swi2);			// { 50, 30, 0, -10 }
	auto original_index1 = std::get<1>(swi2);	// { 1, 0, 3, 2 }
	assert(data2[0].empty() && data2[8].empty());	// moved
	\endcode
*/
template <class C, class F,
	class T = typename impl::container_traits<typename impl::remove_const_reference<C>::type>::value_type
>
auto sort_with_index(
	C&& container,
	F&& binary_op)
{
	using Tp = std::tuple<T, uint>;

	auto result = zip(std::forward<C>(container), seqn(0u, 1u, container.size()));

	sort(result, [&](Tp const& l, Tp const& r){ return std::forward<F>(binary_op)(std::get<0>(l), std::get<0>(r)); });

	return unzip(std::move(result));
}

}
#endif
