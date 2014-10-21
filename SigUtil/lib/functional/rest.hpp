/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_REST_HPP
#define SIG_UTIL_REST_HPP

#include "../helper/helper.hpp"
#include "../helper/container_helper.hpp"

/// \file restl.hpp その他 functional関数

namespace sig
{
// 等差数列
// st:初項, d:公差, n:項数
template <class T1, class T2, class C = std::vector<typename std::common_type<T1, T2>::type>>
C seq(T1 st, T2 d, uint n)
{
	using R = typename std::common_type<T1, T2>::type;
	C result;
	for (uint i = 0; i<n; ++i) impl::container_traits<C>::add_element(result, static_cast<R>(st) + i * static_cast<R>(d));
	return result;
}

#if SIG_GCC_GT4_8_0 || SIG_CLANG_GT_3_4 || !(SIG_MSVC_VER <= 120)
//(a -> a -> bool) -> [a] -> [a]
//比較関数を指定してソート
template <class F, class C, typename std::enable_if<has_random_access_op<C>::value, void>::type*& = enabler>
auto sort(F const& binary_op, C const& data){
	C result = data;
	std::sort(std::begin(result), std::end(result), binary_op);
	return result;
}
template <class F, class C, typename std::enable_if<!has_random_access_op<C>::value, void>::type*& = enabler>
auto sort(F const& binary_op, C const& data){
	C result = data;
	//result.sort(binary_op);
	return result;
}
#endif

}
#endif
