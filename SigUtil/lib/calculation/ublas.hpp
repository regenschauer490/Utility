/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIGUTIL_BLAS_HPP
#define SIGUTIL_BLAS_HPP

#include "../helper/maybe.hpp"
#include "../helper/container_helper.hpp"
#include "../helper/helper_modules.hpp"

#ifdef SIG_USE_BOOST

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/triangular.hpp>

/// \file ublas.hpp boost::numeric::ublasを利用したユーテリティ関数群

namespace sig
{

template <class T>
using matrix_u = boost::numeric::ublas::matrix<T>;
/*
template <class T, class F = boost::numeric::ublas::row_major, class AR = boost::numeric::ublas::unbounded_array<T>>
class matrix_u : public boost::numeric::ublas::matrix<T, F, AR>
{
	using Base = boost::numeric::ublas::matrix<T, F, AR>;

public:
	matrix_u(uint row, uint col, T const& val) : Base(row, col, val){}
	matrix_u(uint row, uint col) : matrix_u(row, col, 0){}
	matrix_u(uint row) : Base(row, 0){}
	matrix_u() : Base(0){}

	template<class AE>
	matrix_u(matrix_expression<AE> const& src) : boost::numeric::ublas::matrix<AE, F, AR<>>(src){}
	template<class AE>
	matrix_u(matrix_expression<AE>&& src) : boost::numeric::ublas::matrix<AE, F, AR>(std::move(src)){}
};
*/

template <class T>
using vector_u = boost::numeric::ublas::vector<T>;
/*
template <class T, class AR = boost::numeric::ublas::unbounded_array<T>>
class vector_u : public boost::numeric::ublas::vector<T, AR>
{
	using Base = boost::numeric::ublas::vector<T, AR>;

	uint tail_;
public:
	vector_u() : Base(), tail_(0){}
	vector_u(uint size) : Base(size), tail_(0){}
	vector_u(uint size, array_type const& data) : Base(size, data), tail_(data.size()){}
	vector_u(array_type const& data) : Base(data), tail_(data.size()){}
	vector_u(uint size, value_type const& init) : Base(size, init), tail_(size){}
	template <class AE>
	vector_u(vector_expression<AE> const& ae) : Base(ae), tail_(ae.size()){}

	vector_u(vector_u const& v) : Base(v), tail_(v.size()){}
	vector_u(vector_u&& v) : Base(std::move(v)), tail_(v.size()){}

	vector_u& operator=(vector_u const& v){ return Base::operator=(v); }
	template<class C>
	vector_u& operator=(vector_container<C> const& v){ return Base::operator=(v); }
	template<class AE>
	vector_u& operator=(vector_expression<AE> const& ae){ return Base::operator=(ae); }

	void push_back(ParamType<T> v){
		assert(tail_ < Base::size());

		Base::insert_element(tail_, v); 
		++tail_;
	}
};

/// ublas::vectorとvector_uのcontainer_traitsを通した振る舞いを定義（使用可能な機能に多少の制限あり）
template<template<class, class> class C, class T, template<class, class> class AR, class A>
struct impl::sequence_container_traits<C<T, AR<T, A>>>
{
	static const bool exist = true;

	using value_type = T;

	template<class U>
	using rebind = vector_u<U, AR<U, typename A::template rebind<U>::other>>;


	static vector_u<T, AR<T,A>> make(size_t n){ return vector_u<T, AR<T,A>>(n); }	// default implementation

	static void add_element(vector_u<T, AR<T,A>>& c, T const& t)
	{
		c.push_back(t);
	}
};

template<class... Args>
struct impl::container_traits<boost::numeric::ublas::vector<Args...>> : public sequence_container_traits<boost::numeric::ublas::vector<Args...>>
{};

template<class... Args>
struct impl::container_traits<vector_u<Args...>> : public sequence_container_traits<vector_u<Args...>>
{};
*/


/// ublas::vector<T> から STLのvectorへ変換
template <class V, class R = std::vector<typename V::value_type>>
auto from_vector_ublas(V const& vec) ->R
{
	R dest = impl::container_traits<R>::make(vec.size());

	for (uint i = 0; i < vec.size(); ++i){
		impl::container_traits<R>::add_element(dest, vec(i));
	}

	return dest;
}

/// ublas::matrix<T> の指定行を STLのvectorへ変換
template <class M, class R = std::vector<typename M::value_type>>
auto from_matrix_ublas(M const& mat, uint row) ->R
{
	R dest = impl::container_traits<R>::make(mat.size2());

	for (uint i = 0; i < mat.size2(); ++i){
		impl::container_traits<R>::add_element(dest, mat(row, i));
	}

	return dest;
}

/// ublas::matrix<T> から STLのvectorの2次元配列へ変換
template <class M, class R = std::vector<std::vector<typename M::value_type>>>
auto from_matrix_ublas(M const& mat) ->R
{
	using C = typename impl::container_traits<R>::value_type;

	R dest = impl::container_traits<R>::make(mat.size1());

	for (uint i=0; i < mat.size1(); ++i){
		C row = impl::container_traits<C>::make(mat.size2());

		for (uint j = 0; j < mat.size2(); ++j){
			impl::container_traits<C>::add_element(row, mat(i, j));
		}
		impl::container_traits<R>::add_element(dest, std::move(row));
	}

	return dest;
}

/// STLのvector から ublas::vector<T> へ変換
template <class C, class T = typename impl::container_traits<C>::value_type>
auto to_vector_ublas(C const& vec) ->vector_u<T>
{	
	vector_u<T> dest(vec.size());

	for (uint i=0; i<vec.size(); ++i){
		dest(i) = vec[i];
	}

	return dest;
}

/// STLのvectorの2次元配列 から ublas::matrix<T> へ変換
template <class CC, class T = typename impl::container_traits<typename impl::container_traits<CC>::value_type>::value_type>
auto to_matrix_ublas(CC const& mat) ->matrix_u<T>
{
	using C = typename impl::container_traits<CC>::value_type;

	const uint size1 = mat.size();
	const uint size2 = mat.begin()->size();
	matrix_u<T> dest(size1, size2);

	for (uint i = 0; i < size1; ++i){
		for (uint j = 0; j < size2; ++j){
			dest(i, j) = mat[i][j];
		}
	}

	return dest;
}


/// 逆行列を求める
/**
	LU分解を用いて逆行列を計算する．\n
	boost.optional有効時には値がラップされて返される

	\param mat 逆行列を求める行列．この行列自体に変更を加えても良い場合はmoveで渡す

	\return 逆行列
*/
template <class T,
	class RT = typename std::conditional<std::is_integral<T>::value, double, T>::type
>
auto invert_matrix(matrix_u<T>&& mat)
	->Maybe<matrix_u<RT>>
{
	using namespace boost::numeric::ublas;

	permutation_matrix<> pm(mat.size1());

	if (lu_factorize(mat, pm) != 0) return Nothing(matrix_u<RT>());

	matrix<RT> result = identity_matrix<RT>(mat.size1());

	lu_substitute(mat, pm, result);

	return Just<matrix_u<RT>>(std::move(result));
}

template <class T,
	class RT = typename std::conditional<std::is_integral<T>::value, double, T>::type
>
auto invert_matrix(matrix_u<T> const& mat)
	->Maybe<matrix_u<RT>>
{
	matrix_u<RT> tmp(mat);

	return invert_matrix(std::move(tmp));
}


/// 連立方程式を解く
template <class T,
	class RT = typename std::conditional<std::is_integral<T>::value, double, T>::type
>
auto matrix_vector_solve(matrix_u<T>&& mat, vector_u<T>&& vec)
	->Maybe<vector_u<RT>>
{
	using namespace boost::numeric::ublas;

	permutation_matrix<> pm(mat.size1());

	if (lu_factorize(mat, pm) != 0) return Nothing(vector_u<RT>());

	lu_substitute(mat, pm, vec);

	return Just<vector_u<RT>>(std::move(vec));
}

template <class T,
	class RT = typename std::conditional<std::is_integral<T>::value, double, T>::type
>
auto matrix_vector_solve(matrix_u<T> const& mat, vector_u<T> const& vec)
	->Maybe<vector_u<RT>>
{
	matrix_u<RT> tmp_m(mat);
	vector_u<RT> tmp_v(vec);
	
	return matrix_vector_solve(std::move(tmp_m), std::move(tmp_v));
}

template <class T,
	class RT = typename std::conditional<std::is_integral<T>::value, double, T>::type
>
auto matrix_vector_solve(matrix_u<T> const& mat, vector_u<T>&& vec)
	->Maybe<vector_u<RT>>
{
	matrix_u<RT> tmp_m(mat);

	return matrix_vector_solve(std::move(tmp_m), std::move(vec));
}

template <class T,
	class RT = typename std::conditional<std::is_integral<T>::value, double, T>::type
>
auto matrix_vector_solve(matrix_u<T>&& mat, vector_u<T> const& vec)
->Maybe<vector_u<RT>>
{
	vector_u<RT> tmp_v(vec);

	return matrix_vector_solve(std::move(mat), std::move(tmp_v));
}


/// ベクトルの全要素に対して、値を返す関数を適用し、その結果のベクトルを返す
template <class F, class V>
auto map_v(F&& func, V&& vec)
{
	using namespace boost::numeric::ublas;
	using RT = decltype(impl::eval(std::forward<F>(func), std::forward<V>(vec)(0)));

	vector_u<RT> result(vec.size());

	for (uint i = 0, size = vec.size(); i < size; ++i){
		result(i) = std::forward<F>(func)(std::forward<V>(vec)(i));
	}

	return result;
}

template <class F, class M>
auto map_m(F&& func, M&& mat)
{
	using namespace boost::numeric::ublas;
	using RT = decltype(impl::eval(std::forward<F>(func), std::forward<M>(mat)(0,0)));

	const uint size1 = mat.size1();
	const uint size2 = mat.size2();

	matrix_u<RT> result(size1, size2);

	for (uint i = 0; i < size1; ++i){
		for (uint j = 0; j < size2; ++j){
			result(i, j) = std::forward<F>(func)(std::forward<M>(mat)(i, j));
		}
	}

	return result;
}

/// ベクトルの全要素に対して、関数を適用する
template <class F, class V, class... Vs>
void for_each_v(F&& func, V& vec, Vs&&... vecs)
{
	const uint length = sig::min(vec.size(), vecs.size()...);
	iterative_assign(length, std::forward<F>(func), impl::begin(vec), impl::begin(std::forward<Vs>(vecs))...);
}

/// 行列の全要素に対して、代入演算を行う関数を適用する
template <class F, class M>
void for_each_m(F&& func, M& mat)
{
	const uint size1 = mat.size1();
	const uint size2 = mat.size2();

	for (uint i = 0; i < size1; ++i){
		for (uint j = 0; j < size2; ++j){
			std::forward<F>(func)(mat(i, j));
		}
	}
}

/// 行列の対角要素の対して、代入演算を行う関数を適用する
template <class F, class M>
void for_diagonal(F&& func, M& mat)
{
	using namespace boost::numeric::ublas;

	const uint size = min(mat.size1(), mat.size2());
	matrix_vector_range<M> r(mat, range(0, size), range(0, size));

	for (auto it = r.begin(), end = r.end(); it != end; ++it){
		std::forward<F>(func)(*it);
	}
}

template <class V,
	typename std::enable_if<std::is_floating_point<typename V::value_type>::value>::type*& = enabler
>
bool normalize_dist_v(V& data)
{
	double sum = std::accumulate(impl::begin(data), impl::end(data), 0.0);
	for (auto& e : data) e /= sum;

	return true;
}

template <class V>
auto normalize_dist_v(V const& data, int dummy = 0)
{
	auto result = data;
	normalize_dist_v(result);

	return result;
}

}
#endif

#endif
