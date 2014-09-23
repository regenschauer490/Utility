/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_BLAS_HPP
#define SIG_UTIL_BLAS_HPP

#include "sigutil.hpp"
#include "traits/container_traits.hpp"

#ifdef SIG_ENABLE_BOOST

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/triangular.hpp>

/* boost::numeric::ublasを利用したユーテリティ関数群 */

namespace sig
{
template <class T>
using vector_u = boost::numeric::ublas::vector<T>;

template <class T>
using matrix_u = boost::numeric::ublas::matrix<T>;


template <class V, class R = std::vector<V::value_type>>
auto from_vector_ublas(V const& vec)
{
	R dest;

	for (uint i = 0; i < vec.size(); ++i){
		container_traits<R>::add_element(dest, vec(i));
	}

	return dest;
}

template <class M, class R = std::vector<M::value_type>>
auto from_vector_ublas(M const& mat, uint row)
{
	R dest;

	for (uint i = 0; i < mat.size2(); ++i){
		container_traits<R>::add_element(dest, mat(row, i));
	}

	return dest;
}

template <class C, class T = typename container_traits<C>::value_type>
auto to_vector_ublas(C const& vec)
{	
	vector_u<T> dest(vec.size());

	sig::for_each([&](uint i, T val){
		dest(i) = val;
	}
	, 0, vec);

	return dest;
}

template <class M, class R = std::vector<std::vector<M::value_type>>>
auto from_matrix_ublas(M const& mat)
{
	using C = typename container_traits<R>::value_type;
	R dest;

	for (uint i=0; i < mat.size1(); ++i){
		C row;
		for (uint j = 0; j < mat.size2(); ++j){
			container_traits<C>::add_element(dest, mat(i, j));
		}
		container_traits<R>::add_element(dest, std::move(row));
	}

	return dest;
}

template <class CC, class T = typename container_traits<container_traits<CC>::value_type>::value_type>
auto to_matrix_ublas(CC const& mat)
{
	using C = typename container_traits<CC>::value_type
	uint size1 = mat.size();
	uint size2 = std::begin(mat)->size();
	matrix_u<T> dest(size1, size2);

	sig::for_each([&](uint i, C const& row){
		sig::for_each([&](uint j, T val){
			dest(i, j) = val;
		}
		, 0, row)
	}
	, 0, mat);

	return dest;
}


template <class T>
auto invert_matrix(matrix_u<T> const& mat) ->maybe<matrix_u<T>>
{
	using namespace boost::numeric::ublas;
	matrix<T> tmp(mat);
	permutation_matrix<> pm(tmp.size1());

	if (lu_factorize(tmp, pm) != 0) return sig::Nothing(matrix_u<T>());

	matrix<T> result = identity_matrix<T>(tmp.size1());

	lu_substitute(tmp, pm, result);

	return sig::Just<matrix_u<T>>(std::move(result));
}

}
#endif

#endif