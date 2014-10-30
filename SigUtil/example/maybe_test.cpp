#include "maybe_test.h"

void MaybeTest()
{
	auto m = sig::Just<int>(1);

	auto n = sig::Nothing(0);	// optional有効時には引数はダミー．無効時には引数の値がそのまま返される
	auto nn = sig::Maybe<int>(n);

	auto f = [&](int v){ return sig::Just(v * 1.5); };

#if SIG_ENABLE_BOOST && SIG_USE_OPTIONAL
	assert(m);
	assert(!n);
	assert(!nn);

	assert(sig::isJust(m));
	assert(!sig::isJust(n));
	assert(!sig::isJust(nn));

	assert(!sig::isNothing(m));
	assert(sig::isNothing(n));
	assert(sig::isNothing(nn));


	const auto cm = sig::Just<int>(2);

	assert(sig::equal(sig::fromJust(m), 1));
	assert(sig::equal(sig::fromJust(cm), 2));
	assert(sig::equal(sig::fromJust(sig::Just<int>(3)), 3));

	auto m1 = sig::Just<int>(4);
	auto& rm1 = sig::fromJust(m1);	// int&
	rm1 = 5;
	assert(sig::equal(sig::fromJust(m1), 5));

	auto& m2 = sig::fromJust(cm);	// const int&
	auto m3 = sig::fromJust(cm);	// int


	assert(sig::equal(*m, 1));
	assert(sig::equal(*cm, 2));
	assert(sig::equal(*sig::Just<int>(3), 3));

	assert(sig::equal(sig::fromMaybe(2, m), 1));
	assert(sig::equal(sig::fromMaybe(2, n), 2));
	assert(sig::equal(sig::fromMaybe(2, nn), 2));


	std::vector<int> vec{ 1, 2, 3 };

	auto vecd = sig::mapMaybe([](int v){ return v != 2 ? sig::Just<double>(v *1.5) : sig::Nothing(0.0); }, vec);

	assert(sig::equal(vecd[0], 1.5));
	assert(sig::equal(vecd[1], 4.5));
	

	// bind chain
	auto mm = sig::Just(3);

	using sig::operator>>=;	// required
	auto mm1 = (mm >>= f) >>= [&](double v){ return sig::Just(std::to_string(v)); };	// Haskell Style

	auto tmm = *mm1;
	assert(sig::equal(*mm1, std::to_string(4.5)));

	using sig::operator<<=;	// required
	auto mm2 = [&](double v){ return sig::Just(std::to_string(v)); } <<= f <<= mm;	// this equals above mm1

	assert(sig::equal(*mm2, std::to_string(4.5)));

	auto mm3 = [&](double v){ return sig::Just(std::to_string(v)); } <<= [](double v){ return sig::Nothing<double>(); } <<= f <<= mm;	// "nothing" pattern

	assert(sig::isNothing(mm3));

	// assign
	auto mmm = sig::Just<int>(3);
	auto nnn = sig::Nothing<int>();
	
	using sig::operator<<=;	// required
	mmm <<= 2;
	nnn <<= 2;

	assert(sig::equal(*mmm, 2));
	assert(sig::equal(*nnn, 2));
#else
	//assert(m);
	//assert(!n);
	//assert(!nn);

	assert(sig::isJust(m));
	assert(!sig::isJust(n));
	assert(!sig::isJust(nn));

	assert(!sig::isNothing(m));
	assert(sig::isNothing(n));
	assert(sig::isNothing(nn));


	const auto cm = sig::Just<int>(2);

	assert(sig::equal(sig::fromJust(m), 1));
	assert(sig::equal(sig::fromJust(cm), 2));
	assert(sig::equal(sig::fromJust(sig::Just<int>(3)), 3));

	auto m1 = sig::Just<int>(4);
	auto& rm1 = sig::fromJust(m1);	// int&
	rm1 = 5;
	assert(sig::equal(sig::fromJust(m1), 5));

	auto& m2 = sig::fromJust(cm);	// const int&
	auto m3 = sig::fromJust(cm);	// int


	using sig::operator*;	// required

	assert(sig::equal(*m, 1));
	assert(sig::equal(*cm, 2));
	assert(sig::equal(*sig::Just<int>(3), 3));

	//assert(sig::equal(sig::fromMaybe(2, m), 1));
	//assert(sig::equal(sig::fromMaybe(2, n), 2));
	//assert(sig::equal(sig::fromMaybe(2, nn), 2));


	std::vector<int> vec{ 1, 2, 3 };

	//auto vecd = sig::mapMaybe([](int v){ return v != 2 ? sig::Just<double>(v *1.5) : sig::Nothing(0); }, vec);

	//assert(sig::equal(vecd[0], 1.5));
	//assert(sig::equal(vecd[1], 4.5));


	//using sig::operator>>=;	// required
	//auto mm1 = (m >>= f) >>= [&](double v){ return sig::Just<bool>(v < 10); };	// Haskell Style

	//assert(*mm1);

	//using sig::operator<<=;	// required
	//auto mm2 = [&](double v){ return sig::Just<bool>(v < 10); } <<= f <<= m;	// this requals above mm1

	//assert(*mm2);

	auto mmm = sig::Just<int>(3);

	using sig::operator<<=;	// required
	mmm <<= 2;

	assert(sig::equal(*mmm, 2));
#endif
}