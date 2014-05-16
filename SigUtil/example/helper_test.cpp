#include "helper_test.h"
#include "../lib/functional.hpp"

void TestHelperModules()
{
	/* コンパイル時 兼 実行時 */

	//可変長and
	static_assert(sig::And(true, true), "");
	static_assert(!sig::And(true, false), "");
	static_assert(!sig::And(false, true), "");
	static_assert(!sig::And(false, false), "");
	static_assert(sig::And(true, true, true, true), "");
	static_assert(!sig::And(false, true, false, true, false), "");

	//可変長or
	static_assert(sig::Or(true, true), "");
	static_assert(sig::Or(true, false), "");
	static_assert(sig::Or(false, true), "");
	static_assert(!sig::Or(false, false), "");
	static_assert(!sig::Or(false, false, false), "");
#if !SIG_MSVC_ENV
	static_assert(sig::Or(false, true, false, true, false), "");
#endif

	//xor
	static_assert(!sig::Xor(true, true), "");
	static_assert(sig::Xor(true, false), "");
	static_assert(sig::Xor(false, true), "");
	static_assert(!sig::Xor(false, false), "");

	//!xor
	static_assert(sig::Consistency(true, true), "");
	static_assert(!sig::Consistency(true, false), "");
	static_assert(!sig::Consistency(false, true), "");
	static_assert(sig::Consistency(false, false), "");

	//大小比較
	static_assert(!sig::Greater(0.5, 1), "");
	static_assert(sig::Less(0.5, 1), "");

#if !SIG_MSVC_ENV
	//最小値/最大値
	static_assert(sig::Min(1, -1, 0, 3, 2) == -1, "");
	static_assert(sig::Max(1, -1, 0, 3, 2) == 3, "");
#endif

	/* 実行時 */

	//boost::optional or pointer (SIG_ENABLE_BOOST で切り替え)
	const sig::Just<int>::type maybe_true(1);
	const sig::Just<int>::type maybe_false(sig::Nothing(0));

#if !SIG_MSVC_ENV
	assert(sig::And(maybe_true, maybe_true));
	assert(sig::And(maybe_true, true));
	assert(!sig::And(maybe_true, maybe_false));
	assert(!sig::And(maybe_true, false));
	assert(!sig::And(maybe_false, maybe_true));
	assert(!sig::And(maybe_false, true));
	assert(!sig::And(maybe_false, maybe_false));
	assert(!sig::And(maybe_false, false));

	assert(sig::Or(maybe_true, maybe_true));
	assert(sig::Or(maybe_true, true));
	assert(sig::Or(maybe_true, maybe_false));
	assert(sig::Or(maybe_true, false));
	assert(sig::Or(maybe_false, maybe_true));
	assert(sig::Or(maybe_false, true));
	assert(!sig::Or(maybe_false, maybe_false));
	assert(!sig::Or(maybe_false, false));

	assert(!sig::Xor(maybe_true, maybe_true));
	assert(!sig::Xor(maybe_true, true));
	assert(sig::Xor(maybe_true, maybe_false));
	assert(sig::Xor(maybe_true, false));
	assert(sig::Xor(maybe_false, maybe_true));
	assert(sig::Xor(maybe_false, true));
	assert(!sig::Xor(maybe_false, maybe_false));
	assert(!sig::Xor(maybe_false, false));

	assert(sig::Consistency(maybe_true, maybe_true));
	assert(sig::Consistency(maybe_true, true));
	assert(!sig::Consistency(maybe_true, maybe_false));
	assert(!sig::Consistency(maybe_true, false));
	assert(!sig::Consistency(maybe_false, maybe_true));
	assert(!sig::Consistency(maybe_false, true));
	assert(sig::Consistency(maybe_false, maybe_false));
	assert(sig::Consistency(maybe_false, false));
#endif

	//min,max
	std::vector<int> v{ 1, 2, 3 };
	assert(sig::Min(1, -1, 0, (int)v.size(), 2) == -1);
	assert(sig::Max(1, -1, 0, (int)v.size(), 2) == v.size());

	//generic |a - b|
	assert(sig::abs_delta(1, 3) == 2);
	assert(sig::abs_delta(-2, -1) == 1);
	assert(sig::abs_delta(3.0, 1.5) == 1.5);
	assert(sig::abs_delta(3, 1.5) == 1.5);

	//generic a == b
	int ct1 = 0;
	for (double f = 0.0; !sig::equal(f, 1.0); f += 0.1){
		if (++ct1 >= 1000) break;
	}
	assert(ct1 == 10);

	int ct2 = 0;
	for (double f = 0.0; f != 1.0; f += 0.1){
		if (++ct2 >= 1000) break;
	}
	assert(ct2 == 1000);

	//generic a ≒ b (|a - b| < ε)
	assert(sig::equal_tolerant(0.001, 0.002, 0.001));
	assert(!sig::equal_tolerant(0.001, 0.003, 0.001));

	//value range check for 'cr'
	int cr = 5;
	assert(sig::check_range(cr, 0, 10));
	assert(!sig::check_range(cr, 0, 3));
	assert(cr == 5);

	//value range check and modify for 'cr'
	int mr = 5;
	assert(sig::modify_range(mr, 0, 10));
	assert(mr == 5);
	assert(!sig::modify_range(mr, 0, 3));
	assert(mr == 3);

	//copy elements (into another container)
	std::vector<int> orig{1,4,3,2,1};
	auto cpy_list = sig::copy<std::list<int>>(orig);
	auto cpy_set = sig::copy<std::set<int>>(orig);

	sig::zipWith([](int v1, int v2){ assert(v1 == v2); return 0; }, cpy_list, orig);
	sig::zipWith([](int v1, int v2){ assert(v1 == v2); return 0; }, cpy_set, sig::array<int,4>{1,2,3,4});
}