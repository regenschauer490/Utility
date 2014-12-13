/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_MAHALANOBIS_DISTANCE_HPP
#define SIG_UTIL_MAHALANOBIS_DISTANCE_HPP

#include "comparable_check.hpp"
#include "../calculation/ublas.hpp"
#include "../calculation/binary_operation.hpp"

namespace sig
{
namespace impl
{
	template <class T>
	double mahalanobis_impl(
		matrix_u<T> const& cov,
		vector_u<T> const& v)
	{
		using boost::numeric;
		return std::sqrt(ublas::inner_prod(v, ublas::prod(*invert_matrix(cov), v)));
	}
}

struct MahalanobisDistance
{
	template <class T1, class T2>
	double operator()(vector_u<T1> const& vec1, vector_u<T2> const& vec2) const
	{
		//assert(is_comparable(vec1, vec2, impl::NumericVectorTag()));

		return impl::mahalanobis_impl(covariance(vec1, vec2), minus(vec1, vec2));
	}
};

}
#endif