#include "helper_test.h"
#include "debug.hpp"
#include "../lib/calculation/statistics_util.hpp"
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
	static_assert(!sig::And(true, false, false, true, false), "");


	//可変長or
	static_assert(sig::Or(true, true), "");
	static_assert(sig::Or(true, false), "");
	static_assert(sig::Or(false, true), "");
	static_assert(!sig::Or(false, false), "");
	static_assert(!sig::Or(false, false, false), "");
	static_assert(sig::Or(false, true, false, true, false), "");


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
	static_assert(!sig::greater(0.5, 1), "");
	static_assert(sig::less(0.5, 1), "");

#if !SIG_MSVC_GT120
	//最小値/最大値
	static_assert(sig::min(1, -1, 0, 3, 2) == -1, "");
	static_assert(sig::max(1, -1, 0, 3, 2) == 3, "");
#endif

	/* 実行時 */

	//boost::optional or pointer (SIG_ENABLE_BOOST で切り替え)
	const auto maybe_true = sig::Just(1);
	const auto maybe_false = sig::Nothing(0);

#if !SIG_MSVC_GT120
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
	assert(sig::min(1, -1, 0, (int)v.size(), 2) == -1);
	assert(sig::max(1, -1, 0, (int)v.size(), 2) == (int)v.size());

	//check NaN
	double zero = 0;
	double nan = std::numeric_limits<double>::quiet_NaN();
	assert(sig::is_number(0));
	assert(!sig::is_number(nan));
	assert(!sig::is_number(zero/zero));
	assert(!sig::is_number(std::sqrt(-1)));

	//check Inf
	double inf = std::numeric_limits<double>::infinity();
	double inf2 = sig::product(sig::replicate<double>(1000, 10));	// 10^1000
	assert(sig::is_finite_number(0));
	assert(!sig::is_finite_number(inf));
	assert(!sig::is_finite_number(inf2));

	//generic |a - b|
	static_assert(sig::abs_delta(1, 3) == 2, "");
	static_assert(sig::abs_delta(-2, -1) == 1, "");
	static_assert(sig::abs_delta(3.0, 1.5) == 1.5, "");
	static_assert(sig::abs_delta(3, 1.5) == 1.5, "");

	//generic a == b
	assert(sig::equal(1, 1));
	assert(sig::equal(1, 1u));
	assert(sig::equal(1u, 1));
	assert(sig::equal(1u, 1u));
	assert(sig::equal(1, 1.0));
	assert(sig::equal(1.0, 1));
	assert(sig::equal('a', 'a'));
	assert(sig::equal("tes", "tes"));
	assert(sig::equal(L"tes", L"tes"));
	assert(sig::equal("tes", std::string("tes")));
	assert(sig::equal(std::string("tes"), "tes"));
	assert(sig::equal(L"tes", std::wstring(L"tes")));
	assert(sig::equal(std::wstring(L"tes"), L"tes"));

	//誤差の蓄積にある程度対処できる(許容範囲の設定は関数定義)
	int ct1 = 0;
	for (double f = 0.0; !sig::equal(f, 1); f += 0.1){
		if (++ct1 >= 1000) break;
	}
	assert(ct1 == 10);

	int ct2 = 0;
	for (double f = 0.0; f != 1; f += 0.01){
		if (++ct2 == 1000) break;
	}
	assert(ct2 == 1000);

	int ct3 = 0;
	for (double f = 0.0; !sig::equal(f, 1); f += 0.01, ++ct3){
		if (ct3 >= 100) assert(false);
	}

	for(int i=0; i<10000; ++i){
		if(!sig::equal(1.1*i, i*1 + i*(1.0/10))){
			std::cout << "equal error:" << i << std::endl;
		}
	}

	//許容範囲を指定　a ≒ b (|a - b| < ε)
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

	//greater, less test
	static_assert(sig::greater(1, 0), "");
	static_assert(sig::less(0, 1), "");

	//function object test
	assert(sig::Identity()(1) == 1, "");
	assert(sig::Increment()(1) == 2);
	assert(sig::Decrement()(1) == 0);

	//copy elements (into another container)
	std::vector<int> orig{1,4,3,2,1};
	auto cpy_list = sig::copy<std::list<int>>(orig);
	auto cpy_set = sig::copy<std::set<int>>(orig);

	sig::assert_foreach(sig::Identity(), cpy_list, orig);
	sig::assert_foreach(sig::Identity(), cpy_set, sig::array<int, 4>{1, 2, 3, 4});
}


struct TestA{};
struct TestB{};

TestA func1(){ return TestA{}; }
TestA func2(int){ return TestA{}; }

struct FO
{
	TestA a_;
	TestB b_;

	TestA operator()(){ return a_; }
	TestB operator()() const{ return b_; }

	TestA mf(){ return a_; }
	TestB mf() const{ return b_; }
};

void TestEval()
{
	static_assert(std::is_same<decltype(sig::impl::eval(func1)), TestA>::value, "");
	static_assert(std::is_same<decltype(sig::impl::eval(func2, 0)), TestA>::value, "");

	FO fo;
	const FO cfo;
	static_assert(std::is_same<decltype(sig::impl::eval(fo)), TestA>::value, "");
	static_assert(std::is_same<decltype(sig::impl::eval(cfo)), TestB>::value, "");
	static_assert(std::is_same<decltype(sig::impl::eval(static_cast<TestA(FO::*)()>(&FO::mf), fo)), TestA>::value, "");
	static_assert(std::is_same<decltype(sig::impl::eval(static_cast<TestB(FO::*)() const>(&FO::mf), cfo)), TestB>::value, "");
	static_assert(std::is_same<decltype(sig::impl::eval(&FO::a_, fo)), TestA&>::value, "");
	static_assert(std::is_same<decltype(sig::impl::eval(&FO::b_, cfo)), TestB const&>::value, "");
}