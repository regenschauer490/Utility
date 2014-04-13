#include "helper_test.h"

void TestHelperModules()
{
	/* コンパイル時 兼 実行時 */

	//可変長and/or
	static_assert(sig::And(true, true), "");
	static_assert(!sig::And(false, true), "");
	static_assert(sig::Or(true, false), "");
	static_assert(!sig::Or(false, false), "");

#ifndef SIG_MSVC_LT1800
	static_assert(!sig::And(true, true, true, false, true), "");
	static_assert(sig::Or(true, false, true, false), "");
#endif

	//大小比較
	static_assert(!sig::Greater(0.5, 1), "");
	static_assert(sig::Less(0.5, 1), "");

	//最小値/最大値
	std::vector<int> v{ 1, 2, 3 };
#ifndef SIG_MSVC_LT1800
	static_assert(sig::Min(-1, 0, 1.5, 2) == -1, "");
	static_assert(sig::Max(-1, 0, 1.5, 2) == 2, "");
	assert(sig::Min(-1.5, 0, v.size()) == -1.5);
	assert(sig::Max(-1.5, 0, v.size()) == v.size());
#else
	assert(sig::Min(-1, 0, static_cast<int>(v.size())) == -1);
	assert(sig::Max(-1, 0, static_cast<int>(v.size())) == v.size());
#endif


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
	assert(sig::Equal(1, 1.1 - 0.1));
	assert(sig::Equal(0.3333333333333333, 1.0 / 3));

	int ct = 0;
	for (double f = 0.0; !sig::Equal(f, 1.0); f += 0.1){
		if (++ct >= 1000) break;
	}
	assert(ct == 10);

	//generic a ≒ b (|a - b| < ε)
	assert(sig::TolerantEqual(0.33, 1.0 / 3, 0.01));
	assert(!sig::TolerantEqual(0.33, 1.0 / 3, 0.001));

	//
	assert(sig::Precision(0.5) == 1);
	assert(sig::Precision(1.23000) == 3);
	assert(sig::Precision(123.4560) == 6);
}