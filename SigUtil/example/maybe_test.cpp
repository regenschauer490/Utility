#include "maybe_test.h"

using namespace sig;

void MaybeTest()
{
	auto m = Just<int>(1);

	auto n = Nothing(0);	// optional有効時には引数はダミー．無効時には引数の値がそのまま返される
	auto nn = Maybe<int>(n);

	auto f = [&](int v){ return Just(v * 1.5); };

#if SIG_ENABLE_BOOST && SIG_USE_OPTIONAL
	assert(m);
	assert(!n);
	assert(!nn);

	assert(isJust(m));
	assert(!isJust(n));
	assert(!isJust(nn));

	assert(!isNothing(m));
	assert(isNothing(n));
	assert(isNothing(nn));


	const auto cm = Just<int>(2);

	assert(equal(fromJust(m), 1));
	assert(equal(fromJust(cm), 2));
	assert(equal(fromJust(Just<int>(3)), 3));

	auto m1 = Just<int>(4);
	auto& rm1 = fromJust(m1);	// int&
	rm1 = 5;
	assert(equal(fromJust(m1), 5));

	auto& m2 = fromJust(cm);	// const int&
	auto m3 = fromJust(cm);	// int


	assert(equal(*m, 1));
	assert(equal(*cm, 2));
	assert(equal(*Just<int>(3), 3));

	assert(equal(fromMaybe(2, m), 1));
	assert(equal(fromMaybe(2, n), 2));
	assert(equal(fromMaybe(2, nn), 2));


	std::vector<int> vec{ 1, 2, 3 };

	auto vecd = mapMaybe([](int v){ return v != 2 ? Just<double>(v *1.5) : Nothing(0.0); }, vec);

	assert(equal(vecd[0], 1.5));
	assert(equal(vecd[1], 4.5));
	

	// bind chain
	auto mm = Just(3);

	using sig::operator>>=;	// required
	auto mm1 = (mm >>= f) >>= [&](double v){ return Just(std::to_string(v)); };	// Haskell Style

	auto tmm = *mm1;
	assert(equal(*mm1, std::to_string(4.5)));

	using sig::operator<<=;	// required
	auto mm2 = [&](double v){ return Just(std::to_string(v)); } <<= f <<= mm;	// this equals above mm1

	assert(equal(*mm2, std::to_string(4.5)));

	auto mm3 = [&](double v){ return Just(std::to_string(v)); } <<= [](double v){ return Nothing<double>(); } <<= f <<= mm;	// "nothing" pattern

	assert(isNothing(mm3));

	// assign
	auto mmm = Just<int>(3);
	auto nnn = Nothing<int>();
	
	using sig::operator<<=;	// required
	mmm <<= 2;
	nnn <<= 2;

	assert(equal(*mmm, 2));
	assert(equal(*nnn, 2));
#else
	//assert(m);
	//assert(!n);
	//assert(!nn);

	assert(isJust(m));
	assert(!isJust(n));
	assert(!isJust(nn));

	assert(!isNothing(m));
	assert(isNothing(n));
	assert(isNothing(nn));


	const auto cm = Just<int>(2);

	assert(equal(fromJust(m), 1));
	assert(equal(fromJust(cm), 2));
	assert(equal(fromJust(Just<int>(3)), 3));

	auto m1 = Just<int>(4);
	auto& rm1 = fromJust(m1);	// int&
	rm1 = 5;
	assert(equal(fromJust(m1), 5));

	auto& m2 = fromJust(cm);	// const int&
	auto m3 = fromJust(cm);	// int


	using sig::operator*;	// required

	assert(equal(*m, 1));
	assert(equal(*cm, 2));
	assert(equal(*Just<int>(3), 3));

	//assert(equal(fromMaybe(2, m), 1));
	//assert(equal(fromMaybe(2, n), 2));
	//assert(equal(fromMaybe(2, nn), 2));


	std::vector<int> vec{ 1, 2, 3 };

	//auto vecd = mapMaybe([](int v){ return v != 2 ? Just<double>(v *1.5) : Nothing(0); }, vec);

	//assert(equal(vecd[0], 1.5));
	//assert(equal(vecd[1], 4.5));


	//using operator>>=;	// required
	//auto mm1 = (m >>= f) >>= [&](double v){ return Just<bool>(v < 10); };	// Haskell Style

	//assert(*mm1);

	//using operator<<=;	// required
	//auto mm2 = [&](double v){ return Just<bool>(v < 10); } <<= f <<= m;	// this requals above mm1

	//assert(*mm2);

	auto mmm = Just<int>(3);

	using sig::operator<<=;	// required
	mmm <<= 2;

	assert(equal(*mmm, 2));
#endif
}