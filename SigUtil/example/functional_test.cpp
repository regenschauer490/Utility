#include "functional_test.h"
#include "debug.hpp"
#include "../lib/array.hpp"
#include "../lib/calculation.hpp"

void MapTest()
{
	const sig::array<int, 3> data0{ 1, -3, 5 };
	const std::vector<int> data1{ 1, -3, 5 };
	const std::list<int> data2{ 1, -3, 5 };
	const std::multiset<int, std::greater<int>> data3{ 1, -3, 5 };
	const std::unordered_multiset<int> data4{ 1, -3, 5, -7 };

	auto r0 = sig::map(sig::increment(), data0);

	auto r1 = sig::map([](int v){ return v * 2; }, data1);

#if !SIG_MSVC_ENV || !(MSC_VER < 1900)
	auto r2 = sig::map([](int v){ return [v](int th){ return v < th; }; }, data2);
#endif
	auto r3 = sig::map([](int v){ return v / 4.0; }, data3);
	auto r4 = sig::map([](int v){ return v < 0; }, data4);

	//test
	for(unsigned i=0; i<data0.size(); ++i){
		assert(data0[i] + 1 == r0[i]);
	}

	for(unsigned i=0; i<data1.size(); ++i){
		assert(data1[i] * 2 == r1[i]);
	}
#if !SIG_MSVC_ENV || !(MSC_VER < 1900)
	{
	const int th = 2;
	auto it2 = data2.begin();
	auto itr2 = r2.begin();
	for(unsigned i=0; i<data2.size(); ++i, ++it2, ++itr2){
		assert(sig::Consistency(*it2 < th, (*itr2)(th)));
	}
	}
#endif
	{
	auto it3 = data3.begin();
	auto itr3 = r3.begin();
	for(unsigned i=0; i<data3.size(); ++i, ++it3, ++itr3){
		assert(sig::equal(*it3 / 4.0, *itr3));
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
	auto h123 = sig::variadicZipWith([](int i, double d, bool b){ return b ? i * d : -1; }, data1, data2, data3);

	{
	auto it3 = data3.begin();
	for(unsigned i=0, end = sig::Min(data1.size(), data2.size(), data3.size()); i<end; ++i, ++it3){
		assert(sig::equal(h123[i], *it3 ? data1[i] * data2[i] : -1));
	}
	}
	
	//-3.300000, b-3, 16.500000, d10
	auto h12345 = sig::variadicZipWith([](int i1, double d, bool b, int i2, std::string s){
		return b ? s + std::to_string(i1) : std::to_string(i2 * d);
	}, data1, data2, data3, data4, data5);

	{
	auto it3 = data3.begin();
	auto it4 = data4.begin();
	auto it5 = data5.begin();
	for(unsigned i=0, end = sig::Min(data1.size(), data2.size(), data3.size(), data4.size(), data5.size()); i<end; ++i, ++it3, ++it4, ++it5){
		assert(h12345[i] == (*it3 ? *it5 + std::to_string(data1[i]) : std::to_string(*it4 * data2[i])));
	}
	}
}

void FunctionalTest()
{
	const sig::array<int, 5> data0{ 1, 2, 3, 4, 5 };
	const std::vector<int> data1{ 1, -3, 5, 2, 10 };
	const std::list<int> data2{ 1, -3, 5, 2 };
	const std::multiset<int, std::less<int>> data3{ 1, -3, 5, 2, 10 };
	const std::unordered_multiset<int> data4{ 1, -3, 5, 2, 10 };

	/// fold function
	int fl1 = sig::foldl(std::plus<int>(), 0, data1);
	double fl2 = sig::foldl([](double sum, int v){ return sum + v * 0.5; }, 0, data1); //note:init value's type is int, but return's value is double because lambda-function returns double.

	assert(fl1 == sig::sum(data1));
	assert(fl2 == std::accumulate(std::begin(data1), std::end(data1), 0.0, [](double sum, int v){ return sum + v * 0.5; }));

#if SIG_GCC_GT4_9_0 || SIG_CLANG_GT_3_5 || SIG_MSVC_ENV

	int fl0 = sig::foldl(std::minus<int>(), 1, sig::array<int, 3>{ 2, 3, 4 }); //((1-2)-3)-4
	int fr0 = sig::foldr(std::minus<int>(), 4, sig::array<int, 3>{ 1, 2, 3 }); //1-(2-(3-4))

	assert(fl0 == ((1-2)-3)-4);
	assert(fr0 == 1-(2-(3-4)));
#endif

	/// zip, unzip (move and const& ver)
#if SIG_GCC_GT4_8_0 || SIG_CLANG_GT_3_4 || _MSC_VER >= 1900
	//move ver
	auto mdata1 = data1;
	auto mdata2 = data2;

	//make container of tuple (container-type equals data1's)
	auto zipped = sig::zip(std::move(mdata1), std::move(mdata2));	//std::vector< std::tuple<int, int>>

	sig::variadicZipWith([](int v1, int v2, std::tuple<int, int> t){
		assert(std::get<0>(t) == v1 && std::get<1>(t) == v2); return 0;
	}, data1, data2, zipped);

	//make tuple of container
	auto unzipped = sig::unzip(std::move(zipped));		//std::tuple< std::vector<int>, std::vector<int>, std::vector<int>, std::vector<int>>

	sig::variadicZipWith(sig::DebugEqual(), data1, std::get<0>(unzipped));
	sig::variadicZipWith(sig::DebugEqual(), data2, std::get<1>(unzipped));

#if SIG_GCC_GT4_9_0 || SIG_CLANG_GT_3_5 || _MSC_VER >= 1900
	//make container of tuple, from tuple of container
	auto rezipped = sig::zip(std::move(unzipped));		//std::vector< std::tuple<int, int>>

	sig::variadicZipWith([](int v1, int v2, std::tuple<int, int> t){
		assert(std::get<0>(t) == v1 && std::get<1>(t) == v2); return 0;
	}, data1, data2, rezipped);
#endif
#endif
	//const ver
	const auto czipped = sig::zip(data1, data2, data3, data4);	//std::vector< std::tuple<int, int, int, int>>

	sig::variadicZipWith([](int v1, int v2, int v3, int v4, std::tuple<int, int, int, int> t){
		assert(std::get<0>(t) == v1 && std::get<1>(t) == v2 && std::get<2>(t) == v3 && std::get<3>(t) == v4); return 0;
	}, data1, data2, data3, data4, czipped);

	auto cunzipped = sig::unzip(czipped);		//std::tuple< std::vector<int>, std::vector<int>, std::vector<int>, std::vector<int>>

	sig::for_each(sig::DebugEqual(), data1, std::get<0>(cunzipped));
	sig::for_each(sig::DebugEqual(), data2, std::get<1>(cunzipped));
	sig::for_each(sig::DebugEqual(), data3, std::get<2>(cunzipped));
	sig::for_each(sig::DebugEqual(), data4, std::get<3>(cunzipped));

#if SIG_GCC_GT4_9_0 || _MSC_VER >= 1900
	auto crezipped = sig::zip(cunzipped);		//std::vector< std::tuple<int, int, int, int>>

	sig::variadicZipWith([](int v1, int v2, int v3, int v4, std::tuple<int, int, int, int> t){
		ssert(std::get<0>(t) == v1 && std::get<1>(t) == v2 && std::get<2>(t) == v3 && std::get<3>(t) == v4); return 0;
	}, data1, data2, data3, data4, rezipped);
#endif

	/// make container filled with same value
	auto rep1 = sig::replicate(5, 3.14);
	auto rep2 = sig::replicate(3, std::string("replicate"));

	assert(rep1.size() == 5);
	assert(rep2.size() == 3);
	for (auto v : rep1) assert(sig::equal(v, 3.14));
	for (auto v : rep2) assert(v == "replicate");

	/// make arithmetic sequence
	auto as1 = sig::seq(1, 2, 5);
	auto as2 = sig::seq(0, -1.1, 4);

	sig::for_each(sig::DebugEqual(), as1, sig::array<int, 5>{ 1, 3, 5, 7, 9 });
	sig::for_each(sig::DebugEqual(), as2, sig::array<double, 4>{ 0, -1.1, -2.2, -3.3 });

	/// make container whose element reversed
	auto rev0 = sig::reverse(data0);
	auto rev1 = sig::reverse(data1);
	auto rev2 = sig::reverse(data2);

	sig::for_each(sig::DebugEqual(), rev0, sig::array<int, 5>{ 5, 4, 3, 2, 1 });
	sig::for_each(sig::DebugEqual(), rev1, sig::array<int, 5>{ 10, 2, 5, -3, 1 });
	sig::for_each(sig::DebugEqual(), rev2, sig::array<int, 4>{ 2, 5, -3, 1 });

	/// merge containers
	auto mc1 = sig::merge(data1, data1);
	auto mc2 = sig::merge<std::vector<int>>(data2, data3);

	{
	const auto size_d1 = data1.size();
	for(unsigned i=0; i<2; ++i){
		for(unsigned j=0; j<size_d1; ++j) assert(mc1[i*size_d1 +j] == data1[j]);
	}

	unsigned total = 0;
	for(auto it = data2.begin(), end = data2.end(); it != end; ++it, ++total) assert(mc2[total] == *it);
	for(auto it = data3.begin(), end = data3.end(); it != end; ++it, ++total) assert(mc2[total] == *it);
	}

	/// take top n elements
	auto t0 = sig::take(1, data0);
	auto t1 = sig::take(2, data1);
	auto t2 = sig::take(3, data2);
	auto t3 = sig::take(4, data3);

	sig::for_each(sig::DebugEqual(), t0, sig::array<int, 1>{ 1 });
	sig::for_each(sig::DebugEqual(), t1, sig::array<int, 2>{ 1, -3 });
	sig::for_each(sig::DebugEqual(), t2, sig::array<int, 3>{ 1, -3, 5 });
	sig::for_each(sig::DebugEqual(), t3, sig::array<int, 4>{ -3, 1, 2, 5 });

	/// drop top n elements
	auto d0 = sig::drop(1, data0);
	auto d1 = sig::drop(2, data1);
	auto d2 = sig::drop(3, data2);
	auto d3 = sig::drop(4, data3);

	sig::for_each(sig::DebugEqual(), d0, sig::array<int, 4>{ 2, 3, 4, 5 });
	sig::for_each(sig::DebugEqual(), d1, sig::array<int, 3>{ 5, 2, 10 });
	sig::for_each(sig::DebugEqual(), d2, sig::array<int, 1>{ 2 });
	sig::for_each(sig::DebugEqual(), d3, sig::array<int, 1>{ 10 });

#if SIG_GCC_GT4_8_0 || SIG_CLANG_GT_3_4 || _MSC_VER >= 1900
	/// sort
	auto s0 = sig::sort(std::greater<int>(), data0);
	auto s1 = sig::sort(std::less<int>(), data1);
	auto s2 = sig::sort([](int l, int r){ return std::abs(l) < std::abs(r); }, data2);

	sig::for_each(sig::DebugEqual(), s0, sig::array<int, 5>{ 5, 4, 3, 2, 1 });
	sig::for_each(sig::DebugEqual(), s1, sig::array<int, 5>{ -3, 1, 2, 5, 10 });
	sig::for_each(sig::DebugEqual(), s2, sig::array<int, 4>{ 1, 2, -3, 5 });
#endif

}
