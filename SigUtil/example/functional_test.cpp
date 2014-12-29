#include "functional_test.h"
#include "debug.hpp"
#include "../lib/calculation.hpp"

using namespace sig;

void MapTest()
{
	const array<int, 3> data0{ 1, -3, 5 };
	const std::vector<int> data1{ 1, -3, 5 };
	const std::list<int> data2{ 1, -3, 5 };
	const std::multiset<int, std::greater<int>> data3{ 1, -3, 5 };
	const std::unordered_multiset<int> data4{ 1, -3, 5, -7 };

	auto r0 = map(increment_t(), data0);
	auto r1 = map([](int v){ return v * 2; }, data1);
	auto r3 = map([](int v){ return v / 4.0; }, data3);
	auto r4 = map([](int v){ return v < 0; }, data4);

	//test
	for(unsigned i=0; i<data0.size(); ++i){
		assert(data0[i] + 1 == r0[i]);
	}

	for(unsigned i=0; i<data1.size(); ++i){
		assert(data1[i] * 2 == r1[i]);
	}
#if !SIG_MSVC_ENV || !(SIG_MSVC_VER < 140)
	auto r2 = map([](int v){ return [v](int th){ return v < th; }; }, data2);
	{
	const int th = 2;
	auto it2 = data2.begin();
	auto itr2 = r2.begin();
	for(unsigned i=0; i<data2.size(); ++i, ++it2, ++itr2){
		assert(Consistency(*it2 < th, (*itr2)(th)));
	}
	}
#endif
	{
	auto it3 = data3.begin();
	auto itr3 = r3.begin();
	for(unsigned i=0; i<data3.size(); ++i, ++it3, ++itr3){
		assert(equal(*it3 / 4.0, *itr3));
	}
	}

	assert(std::accumulate(data4.begin(), data4.end(), 0, [](int sum, int v1){ return sum + static_cast<int>(v1 < 0); }) == std::accumulate(r4.begin(), r4.end(), 0));
}

void ZipWithTest()
{
	const std::vector<int> data1{ 1, -3, 5, 10, -8 };
	const std::deque<double> data2{ 1.1, 2.2, 3.3, 4.4 };
	const std::list<bool> data3{ false, true, false, true };
	const std::multiset<int> data4{ 1, -3, 5, 10 };
	const std::unordered_multiset<std::string> data5{ "a", "b", "c", "d", "e" };

	//-1, -6.6, -1, 44
	auto h123 = variadicZipWith([](int i, double d, bool b){ return b ? i * d : -1; }, data1, data2, data3);

	{
	auto it3 = data3.begin();
	for(unsigned i=0, end = min(data1.size(), data2.size(), data3.size()); i<end; ++i, ++it3){
		assert(equal(h123[i], *it3 ? data1[i] * data2[i] : -1));
	}
	}
	
	//-3.300000, b-3, 16.500000, d10
	auto h12345 = variadicZipWith([](int i1, double d, bool b, int i2, std::string s){
		return b ? s + std::to_string(i1) : std::to_string(i2 * d);
	}, data1, data2, data3, data4, data5);

	{
	auto it3 = data3.begin();
	auto it4 = data4.begin();
	auto it5 = data5.begin();
	for(unsigned i=0, end = min(data1.size(), data2.size(), data3.size(), data4.size(), data5.size()); i<end; ++i, ++it3, ++it4, ++it5){
		assert(h12345[i] == (*it3 ? *it5 + std::to_string(data1[i]) : std::to_string(*it4 * data2[i])));
	}
	}

#if SIG_ENABLE_MOVEITERATOR
	// move test
	const std::vector<std::vector<int>> ddata1{ std::vector<int>{1, 2}, std::vector<int>{3, 4} };
	std::vector<std::vector<int>> ddata2{ std::vector<int>{1, 2}, std::vector<int>{3, 4} };

	auto ht = zipWith([](std::vector<int> const& a, std::vector<int>&& b){ auto t = std::move(b); return a[0] + t[1]; }, ddata1, std::move(ddata2));
	assert(!ddata1[0].empty());
	assert(ddata2[1].empty());
#endif
}

void FunctionalTest()
{
	const array<int, 5> data0{ 1, 2, 3, 4, 5 };
	const std::vector<int> data1{ 1, -3, 5, 2, 10 };
	const std::vector<double> data1d{ 1.1, 2.2, 3.3 };
	const std::list<int> data2{ 1, -3, 5, 2 };
	const std::multiset<int, std::less<int>> data3{ 1, -3, 5, 2, 10 };
	const std::unordered_multiset<int> data4{ 1, -3, 5, 2, 10 };

	/// fold function
	{
		int fl1 = foldl(std::plus<int>(), 0, data0);
		double fl2 = foldl([](double sum, int v){ return sum + v * 0.5; }, 0, data2); //note:init value's type is int, but return's value is double because lambda-function returns double.

		assert(fl1 == sum(data0));
		assert(fl2 == std::accumulate(std::begin(data2), std::end(data2), 0.0, [](double sum, int v){ return sum + v * 0.5; }));

#if SIG_ENABLE_FOLDR

		int fl0 = foldl(std::minus<int>(), 1, array<int, 3>{ 2, 3, 4 }); //((1-2)-3)-4
		int fr0 = foldr(std::minus<int>(), 4, array<int, 3>{ 1, 2, 3 }); //1-(2-(3-4))

		assert(fl0 == ((1-2)-3)-4);
		assert(fr0 == 1-(2-(3-4)));
#endif
	}

	/// dotProduct
	{
		double dp = dotProduct(
			[](double sum, double a){ return sum + a; },
			[](int a, int b, int c, double d){ return std::pow(a*a + b*b + c*c + d*d, 0.25); },
			0,
			data0, data1, data2, array<double, 4>{ 1.1, 2.2, 3.3, 4.4 }
		);

		{
			double dp_test = 0;
			auto it0 = data0.begin();
			auto it1 = data1.begin();
			auto it2 = data2.begin();

			for (int i = 1, end = min(data0.size(), data1.size(), data2.size(), 4); i <= end; ++i, ++it0, ++it1, ++it2){
				dp_test += std::pow((*it0)*(*it0) + (*it1)*(*it1) + (*it2)*(*it2) + std::pow(i+0.1*i, 2), 0.25);
			}

			assert(equal(dp, dp_test));
		}
	}

	/// filter, partition
	{
		array<TestInt, 4> data0t{ 1, 2, 3, 0 };

		auto filt1 = filter([](int v){ return v%2; }, data0);
		auto filt2 = filter([](int i, int v){ return i < v; }, 1, data2);
		auto filt3 = filter([](TestInt const& v){ return v.value() < 2; }, std::move(data0t));

		auto part1 = partition([](int v){ return v % 2; }, data0);
		auto part2 = partition([](int v){ return v < 0; }, data2);

		for_each(DebugEqual(), filt1, std::vector<int>{1, 3, 5});
		for_each(DebugEqual(), filt2, std::vector<int>{5});
		for_each(DebugEqual(), filt3, std::vector<sig::TestInt>{1, 0});
		assert(data0t[0].empty() && data0t[3].empty() && (!data0t[1].empty()) && (!data0t[2].empty()));

		for_each(DebugEqual(), std::get<0>(part1), std::vector<int>{1, 3, 5});
		for_each(DebugEqual(), std::get<1>(part1), std::vector<int>{2, 4});
		for_each(DebugEqual(), std::get<0>(part2), std::vector<int>{-3});
		for_each(DebugEqual(), std::get<1>(part2), std::vector<int>{1, 5, 2});
	}

	/// zip, unzip (move and const& ver)
	{
#if SIG_ENABLE_MOVEITERATOR
		//move ver
		auto mdata1 = data1;
		auto mdata2 = data2;

		//make container of tuple (container-type equals data1's)
		auto zipped = zip(std::move(mdata1), std::move(mdata2));	//std::vector< std::tuple<int, int>>

		variadicZipWith([](int v1, int v2, std::tuple<int, int> t){
			assert(std::get<0>(t) == v1 && std::get<1>(t) == v2); return 0;
		}, data1, data2, zipped);

		//make tuple of container
		auto unzipped = unzip(std::move(zipped));		//std::tuple< std::vector<int>, std::vector<int>, std::vector<int>, std::vector<int>>

		for_each(DebugEqual(), data1, std::get<0>(unzipped));
		for_each(DebugEqual(), data2, std::get<1>(unzipped));

#if SIG_GCC_GT4_9_0 || SIG_CLANG_GT_3_5 || !(SIG_MSVC_VER <= 140)
		//make container of tuple, from tuple of container
		auto rezipped = zip(std::move(unzipped));		//std::vector< std::tuple<int, int>>

		variadicZipWith([](int v1, int v2, std::tuple<int, int> t){
			assert(std::get<0>(t) == v1 && std::get<1>(t) == v2); return 0;
		}, data1, data2, rezipped);
#endif
#endif
		//const ver
		const auto czipped = zip(data1, data2, data3, data4);	//std::vector< std::tuple<int, int, int, int>>

		variadicZipWith([](int v1, int v2, int v3, int v4, std::tuple<int, int, int, int> t){
			assert(std::get<0>(t) == v1 && std::get<1>(t) == v2 && std::get<2>(t) == v3 && std::get<3>(t) == v4); return 0;
		}, data1, data2, data3, data4, czipped);

		auto cunzipped = unzip(czipped);		//std::tuple< std::vector<int>, std::vector<int>, std::vector<int>, std::vector<int>>

		for_each(DebugEqual(), data1, std::get<0>(cunzipped));
		for_each(DebugEqual(), data2, std::get<1>(cunzipped));
		for_each(DebugEqual(), data3, std::get<2>(cunzipped));
		for_each(DebugEqual(), data4, std::get<3>(cunzipped));

#if SIG_ENABLE_TUPLE_ZIP
		auto rezipped = zip(cunzipped);		//std::vector< std::tuple<int, int, int, int>>

		variadicZipWith([](int v1, int v2, int v3, int v4, std::tuple<int, int, int, int> t){
			assert(std::get<0>(t) == v1 && std::get<1>(t) == v2 && std::get<2>(t) == v3 && std::get<3>(t) == v4); return 0;
		}, data1, data2, data3, data4, rezipped);
#endif
	}

	/// make container filled with same value
	{
		auto rep1 = replicate(5, 3.14);
		auto rep2 = replicate(3, std::string("replicate"));

		assert(rep1.size() == 5);
		assert(rep2.size() == 3);
		for (auto v : rep1) assert(equal(v, 3.14));
		for (auto v : rep2) assert(v == "replicate");
	}

	/// make arithmetic sequence
	{
		auto as1 = seqn(1, 2, 5);
		auto as2 = seqn(0, -1.1, 4);

		for_each(DebugEqual(), as1, array<int, 5>{ 1, 3, 5, 7, 9 });
		for_each(DebugEqual(), as2, array<double, 4>{ 0, -1.1, -2.2, -3.3 });
	}

	/// make container whose element reversed
	{
		auto rev0 = reverse(data0);
		auto rev1 = reverse(data1);
		auto rev2 = reverse(data2);

		for_each(DebugEqual(), rev0, array<int, 5>{ 5, 4, 3, 2, 1 });
		for_each(DebugEqual(), rev1, array<int, 5>{ 10, 2, 5, -3, 1 });
		for_each(DebugEqual(), rev2, array<int, 4>{ 2, 5, -3, 1 });
	}

	/// merge containers
	{
		auto mc1 = merge(data1, data1);	//same type

		auto mc2 = merge(data2, data3);
		auto mc3 = merge<std::vector<int>>(data2, data3);
		std::vector<double> mc4 = merge(data1, data1d);

		const auto size_d1 = data1.size();
		for (unsigned i = 0; i < 2; ++i){
			for (unsigned j = 0; j < size_d1; ++j) assert(mc1[i*size_d1 + j] == data1[j]);
		}

		unsigned total = 0;
		for (auto it = data2.begin(), end = data2.end(); it != end; ++it, ++total) assert(mc3[total] == *it);
		for (auto it = data3.begin(), end = data3.end(); it != end; ++it, ++total) assert(mc3[total] == *it);

		// test array<T, N>
		const array<int, 4> ar1{ 1, 2, 3, 4 };
		array<TestInt, 4> ar2{ 5, 6, 7, 8 };
		const array<double, 3> ar3{ 5.5, 6.6, 7.7 };

		array<int, 8> ma1 = merge(ar1, ar1);
		array<double, 7> ma2 = merge(ar1, ar3);
		decltype(data1d) ma3 = merge(ar1, data1d);
		decltype(data1d) ma4 = merge(data1d, ar1);

#if SIG_ENABLE_MOVEITERATOR
		auto mmm = merge<std::set<double, std::greater<double>>>(ar3, ar1);
		auto ma1m = merge(ar1, std::move(ar2));
		assert(ar2[0].empty() && ar2[3].empty());
#endif
	}

	/// take top n elements
	{
		auto t0 = take(1, data0);
		auto t1 = take(2, data1);
		auto t2 = take(3, data2);
		auto t3 = take(4, data3);

		for_each(DebugEqual(), t0, array<int, 1>{ 1 });
		for_each(DebugEqual(), t1, array<int, 2>{ 1, -3 });
		for_each(DebugEqual(), t2, array<int, 3>{ 1, -3, 5 });
		for_each(DebugEqual(), t3, array<int, 4>{ -3, 1, 2, 5 });

#if SIG_ENABLE_MOVEITERATOR
		std::vector<TestInt> vec{ 1, 2, 3 };
		auto t4 = take(2, std::move(vec));

		for_each(DebugEqual(), t4, array<int, 2>{ 1, 2 });
		assert(vec[0].empty() && vec[1].empty());
#endif
	}

	/// drop top n elements
	{
		auto d0 = drop(1, data0);
		auto d1 = drop(2, data1);
		auto d2 = drop(3, data2);
		auto d3 = drop(4, data3);

		for_each(DebugEqual(), d0, array<int, 4>{ 2, 3, 4, 5 });
		for_each(DebugEqual(), d1, array<int, 3>{ 5, 2, 10 });
		for_each(DebugEqual(), d2, array<int, 1>{ 2 });
		for_each(DebugEqual(), d3, array<int, 1>{ 10 });
	}

#if SIG_GCC_GT4_8_0 || SIG_CLANG_GT_3_4 || !(SIG_MSVC_VER < 140)
	/// sort
	{
		auto s0 = sort(std::greater<int>(), data0);
		auto s1 = sort(std::less<int>(), data1);
		auto s2 = sort([](int l, int r){ return std::abs(l) < std::abs(r); }, data2);

		for_each(DebugEqual(), s0, array<int, 5>{ 5, 4, 3, 2, 1 });
		for_each(DebugEqual(), s1, array<int, 5>{ -3, 1, 2, 5, 10 });
		for_each(DebugEqual(), s2, array<int, 4>{ 1, 2, -3, 5 });
	}
#endif

}
