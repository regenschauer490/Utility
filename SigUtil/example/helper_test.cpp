#include "helper_test.h"
#include "../lib/functional.hpp"

void TestHelperModules()
{
	/* コンパイル時 兼 実行時 */

	//可変長and/or
	static_assert(sig::And(true, true), "");
	static_assert(!sig::And(false, true), "");
	static_assert(sig::Or(true, false), "");
	static_assert(!sig::Or(false, false), "");

	//大小比較
	static_assert(!sig::Greater(0.5, 1), "");
	static_assert(sig::Less(0.5, 1), "");

	//最小値/最大値
	std::vector<int> v{ 1, 2, 3 };
	assert(sig::Min(-1, 0, static_cast<int>(v.size())) == -1);
	assert(sig::Max(-1, 0, static_cast<int>(v.size())) == v.size());


	/* 実行時 */

	//boost::optional or pointer (SIG_ENABLE_BOOST で切り替え)
	const sig::Just<int>::type maybe_true(1);
	const sig::Just<int>::type maybe_false(sig::Nothing(0));

	//xor
	assert(sig::Xor(true, false));
	assert(sig::Xor(false, true));
	assert(!sig::Xor(true, true));
	assert(!sig::Xor(false, false));
	assert(sig::Xor(maybe_true, false));
	assert(sig::Xor(maybe_false, true));

	//!xor
	assert(!sig::Consistency(true, false));
	assert(!sig::Consistency(false, true));
	assert(sig::Consistency(true, true));
	assert(sig::Consistency(false, false));
	assert(!sig::Consistency(maybe_true, false));
	assert(!sig::Consistency(maybe_false, true));

	//generic |a - b|
	assert(sig::DeltaAbs(1, 3) == 2);
	assert(sig::DeltaAbs(-2, -1) == 1);
	assert(sig::DeltaAbs(3.0, 1.5) == 1.5);
	assert(sig::DeltaAbs(3, 1.5) == 1.5);

	//generic a == b
	int ct1 = 0;
	for (double f = 0.0; !sig::Equal(f, 1.0); f += 0.1){
		if (++ct1 >= 1000) break;
	}
	assert(ct1 == 10);

	int ct2 = 0;
	for (double f = 0.0; f != 1.0; f += 0.1){
		if (++ct2 >= 1000) break;
	}
	assert(ct2 == 1000);

	//generic a ≒ b (|a - b| < ε)
	assert(sig::TolerantEqual(0.001, 0.002, 0.001));
	assert(!sig::TolerantEqual(0.001, 0.003, 0.001));

	//value range check for 'cr'
	int cr = 5;
	assert(sig::CheckRange(cr, 0, 10));
	assert(!sig::CheckRange(cr, 0, 3));
	assert(cr == 5);

	//value range check and modify for 'cr'
	int mr = 5;
	assert(sig::ModifyRange(mr, 0, 10));
	assert(mr == 5);
	assert(!sig::ModifyRange(mr, 0, 3));
	assert(mr == 3);

	//copy elements
	std::vector<int> orig{1,4,3,2,1};
	auto cpy_list = sig::Copy<std::list<int>>(orig);
	auto cpy_set = sig::Copy<std::set<int>>(orig);

	sig::zipWith([](int v1, int v2){ assert(v1 == v2); return 0; }, cpy_list, orig);
	sig::zipWith([](int v1, int v2){ assert(v1 == v2); return 0; }, cpy_set, sig::Array<int,4>{1,2,3,4});
}