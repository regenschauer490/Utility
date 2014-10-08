#include "maybe_test.h"

void MaybeTest()
{
	auto m = sig::Just<int>(1);

	auto n = sig::Nothing(0);	// optional有効時には引数はダミー．無効時には引数の値がそのまま返される
	auto nn = sig::Maybe<int>(n);

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

	assert(sig::equal(sig::fromMaybe(2, m), 1));
	assert(sig::equal(sig::fromMaybe(2, n), 2));
	assert(sig::equal(sig::fromMaybe(2, nn), 2));

	std::vector<int> vec{ 1, 2, 3 };

	auto vecd = sig::mapMaybe([](int v){ return v != 2 ? sig::Just<double>(v *1.5) : sig::Nothing(0); }, vec);

	assert(sig::equal(vecd[0], 1.5));
	assert(sig::equal(vecd[1], 4.5));
#else
#endif
}