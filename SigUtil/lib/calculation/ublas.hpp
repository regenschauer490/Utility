/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_BLAS_HPP
#define SIG_UTIL_BLAS_HPP

#include "../sigutil.hpp"
#include "../helper/container_traits.hpp"
#include "../helper/maybe.hpp"

#ifdef SIG_ENABLE_BOOST

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/triangular.hpp>

/// \file ublas.hpp boost::numeric::ublasを利用したユーテリティ関数群

namespace sig
{
template <class T>
using vector_u = boost::numeric::ublas::vector<T>;

template <class T>
using matrix_u = boost::numeric::ublas::matrix<T>;

/// ublas::vector<T> から STLのvectorへ変換
template <class V, class R = std::vector<V::value_type>>
auto from_vector_ublas(V const& vec) ->R
{
	R dest;

	for (uint i = 0; i < vec.size(); ++i){
		impl::container_traits<R>::add_element(dest, vec(i));
	}

	return dest;
}

/// ublas::matrix<T> の指定行を STLのvectorへ変換
template <class M, class R = std::vector<M::value_type>>
auto from_vector_ublas(M const& mat, uint row) ->R
{
	R dest;

	for (uint i = 0; i < mat.size2(); ++i){
		impl::container_traits<R>::add_element(dest, mat(row, i));
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

/// ublas::matrix<T> から STLのvectorの2次元配列へ変換
template <class M, class R = std::vector<std::vector<typename M::value_type>>>
auto from_matrix_ublas(M const& mat) ->R
{
	using C = typename impl::container_traits<R>::value_type;
	using T = typename M::value_type;
	R dest;

	for (uint i=0; i < mat.size1(); ++i){
		C row;
		for (uint j = 0; j < mat.size2(); ++j){
			impl::container_traits<C>::add_element(row, mat(i, j));
		}
		impl::container_traits<R>::add_element(dest, std::move(row));
	}

	return dest;
}

/// STLのvectorの2次元配列 から ublas::matrix<T> へ変換
template <class CC, class T = typename impl::container_traits<impl::container_traits<CC>::value_type>::value_type>
auto to_matrix_ublas(CC const& mat) ->matrix_u<T>
{
	using C = typename impl::container_traits<CC>::value_type;
	uint size1 = mat.size();
	uint size2 = std::begin(mat)->size();
	matrix_u<T> dest(size1, size2);

	for (uint i=0; i<mat.size(); ++i){
		for (uint j=0; j<mat[i].size(); ++j){
			dest(i, j) = mat[i][j];
		}
	}

	return dest;
}

/// 逆行列を求める
/**
	LU分解を用いて逆行列を計算する．\n
	boost.optional有効時には値がラップされて返される

	\return 逆行列
*/
template <class T>
auto invert_matrix(matrix_u<T> const& mat)
	->Maybe<matrix_u<T>>
{
	using namespace boost::numeric::ublas;
	matrix<T> tmp(mat);
	permutation_matrix<> pm(tmp.size1());

	if (lu_factorize(tmp, pm) != 0) return Nothing(matrix_u<T>());

	matrix<T> result = identity_matrix<T>(tmp.size1());

	lu_substitute(tmp, pm, result);

	return Just<matrix_u<T>>(std::move(result));
}

}
#endif

#endif