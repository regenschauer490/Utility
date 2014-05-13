#include "helper_test.h"
#include "../lib/functional.hpp"

void TestHelperModules()
{
	/* コンパイル時 兼 実行時 */

	//可変長and/or
	static_assert(sig::and(true, true), "");
	static_assert(!sig::and(false, true), "");
	static_assert(sig::or(true, false), "");
	static_assert(!sig::or(false, false), "");

	//大小比較
	static_assert(!sig::greater(0.5, 1), "");
	static_assert(sig::less(0.5, 1), "");

	//最小値/最大値
	std::vector<int> v{ 1, 2, 3 };
	assert(sig::min(-1, 0, static_cast<int>(v.size())) == -1);
	assert(sig::max(-1, 0, static_cast<int>(v.size())) == v.size());


	/* 実行時 */

	//boost::optional or pointer (SIG_ENABLE_BOOST で切り替え)
	const sig::Just<int>::type maybe_true(1);
	const sig::Just<int>::type maybe_false(sig::Nothing(0));

	//xor
	assert(sig::xor(true, false));
	assert(sig::xor(false, true));
	assert(!sig::xor(true, true));
	assert(!sig::xor(false, false));
	assert(sig::xor(maybe_true, false));
	assert(sig::xor(maybe_false, true));

	//!xor
	assert(!sig::consistency(true, false));
	assert(!sig::consistency(false, true));
	assert(sig::consistency(true, true));
	assert(sig::consistency(false, false));
	assert(!sig::consistency(maybe_true, false));
	assert(!sig::consistency(maybe_false, true));

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