/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIGUTIL_DEBUG_HPP
#define SIGUTIL_DEBUG_HPP

#include <assert.h>
#include "../lib/helper/helper_modules.hpp"
#include "../lib/helper/container_helper.hpp"

/* デバッグ用ツール */

namespace sig
{
#undef min

	const FilepassString raw_pass = SIG_TO_FPSTR("../../SigUtil/example/test_file/");


	struct DebugEqual{
		template <class T1, class T2, typename std::enable_if<!(std::is_floating_point<T1>::value) && !(std::is_floating_point<T2>::value)>::type *& = enabler>
		void operator()(T1 v1, T2 v2) const{
			using T = typename std::common_type<T1, T2>::type;
			assert(static_cast<T>(v1) == static_cast<T>(v2));
		}

		template <class T1, class T2, typename std::enable_if<std::is_floating_point<T1>::value || std::is_floating_point<T2>::value>::type *& = enabler>
		void operator()(T1 v1, T2 v2) const{
			//std::cout << "v1:" << v1 << ", v2:" << v2 << std::endl;
			assert(equal(v1, v2));
		}
	};

	const DebugEqual debug_equal;

	template <class F, class C, class... Cs>
	auto assert_foreach(F const& func, C const& test, Cs const&... origs)
	{
		C calc;
		const uint length = min(origs.size()...);
		iterative_make(length, calc, func, std::begin(origs)...);
		assert(test.size() == calc.size()); 

		auto it1 = std::begin(test), end1 = std::end(test);
		auto it2 = std::begin(calc), end2 = std::end(calc);

		for (; it1!=end1 && it2!=end2; ++it1, ++it2){
			debug_equal(*it1, *it2);
		}
	}

	class TestInt
	{
		std::vector<int> v;		// use only first element
		bool emp;
	public:

		TestInt() : v(), emp(true){}
		TestInt(int i) : v(1, i), emp(false){}
		TestInt(TestInt const& s) : v(s.v), emp(false){}
		TestInt(TestInt&& s) : v(std::move(s.v)), emp(false){ s.emp = true; }

		TestInt& operator=(TestInt const& s){ v = s.v; emp = false; return *this; }
		TestInt& operator=(TestInt&& s){ v = std::move(s.v); emp = false; s.emp = true; return *this; }

		TestInt& operator+=(TestInt const& s){ v[0] += s.v[0]; emp = false; return *this; }
		TestInt& operator+=(TestInt&& s){ auto tmp = std::move(s.v); v[0] += tmp[0]; emp = false; s.emp = true; return *this; }

		bool operator==(TestInt s) const{ return s.v == v; }

		bool empty() const{ return emp; }

		int value() const{ return emp ? -1 : v[0]; }
	};
};

#endif
