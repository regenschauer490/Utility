#include "functional_test.h"

void MapTest()
{
	std::vector<int> data1{ 1, -3, 5 };
	std::list<int> data2{ 1, -3, 5 };
	std::multiset<int, std::greater<int>> data3{ 1, -3, 5 };
	std::unordered_multiset<int> data4{ 1, -3, 5, -7 };

	auto r1 = sig::map([](int v){ return v * 2; }, data1);
#if _MSC_VER > 1800
	auto r2 = sig::map([](int v){ return [v](int th){ return v < th; }; }, data2);
#endif
	auto r3 = sig::map([](int v){ return v / 4.0; }, data3);
	auto r4 = sig::map([](int v){ return v < 0; }, data4);

	sig::zipWith([](int v1, int v2){ assert(v1 * 2 == v2); return 0; }, data1, r1);		//検証

#if _MSC_VER > 1800
	for (auto v : data2) std::cout << v << ", ";	//1, -3, 5
	std::cout << std::endl;
	for (auto v : r2) std::cout << v(2) << ", ";	//1, 1, 0
#endif
	sig::zipWith([](int v1, double v2){ assert(v1 / 4.0 == v2); return 0; }, data3, r3);

	assert(std::accumulate(data4.begin(), data4.end(), 0, [](int sum, int v1){ return sum + static_cast<int>(v1 < 0); }) == std::accumulate(r4.begin(), r4.end(), 0));
}

void ZipWithTest()
{
	std::vector<int> data1{ 1, -3, 5, 10, -8 };
	std::deque<double> data2{ 1.1, 2.2, 3.3, 4.4 };
	std::list<bool> data3{ false, true, false, true };
	std::multiset<int> data4{ 1, -3, 5, 10 };
	std::unordered_multiset<std::string> data5{ "a", "b", "c", "d", "e" };

	for (auto v : data1) std::cout << v << ", ";	//1, -3, 5, 10, -8
	std::cout << std::endl;
	for (auto v : data2) std::cout << v << ", ";	//1.1, 2.2, 3.3, 4.4
	std::cout << std::endl;
	for (auto v : data3) std::cout << v << ", ";	//0, 1, 0, 1
	std::cout << std::endl;
	for (auto v : data4) std::cout << v << ", ";	//-3, 1, 5, 10
	std::cout << std::endl;
	for (auto v : data5) std::cout << v << ", ";	//a, b, c, d, e	//環境依存
	std::cout << std::endl << std::endl;

	//0, -6.6, 0, 44
	auto h123 = sig::variadicZipWith([](int i, double d, bool b){ return b ? i * d : 0; }, data1, data2, data3);
	
	//-3.300000, b-3, 16.500000, d10
	auto h12345 = sig::variadicZipWith([](int i1, double d, bool b, int i2, std::string s){
		return b ? s + std::to_string(i1) : std::to_string(i2 * d);
	}, data1, data2, data3, data4, data5);


	//forループで処理して検証
	auto d3it = data3.begin();
	for (int i = 0, size = sig::Min(data1.size(), data2.size(), data3.size()); i < size; ++i, ++d3it){
		assert( sig::Equal(h123[i], *d3it ? data1[i] * data2[i] : 0) );
	}

	d3it = data3.begin();
	auto d4it = data4.begin();
	auto d5it = data5.begin();
	for (int i = 0, size = sig::Min(data1.size(), data2.size(), data3.size(), data4.size(), data5.size()); i < size; ++i, ++d3it, ++d4it, ++d5it){
		assert( h12345[i] == (*d3it ? *d5it + std::to_string(data1[i]) : std::to_string(*d4it * data2[i])) );
	}
}

void FunctionalTest()
{
	std::vector<int> data1{ 1, -3, 5, 2, 10 };
	std::list<int> data2{ 1, -3, 5, 2 };
	std::multiset<int, std::less<int>> data3{ 1, -3, 5, 2, 10 };
	std::unordered_multiset<int> data4{ 1, -3, 5, 2, 10 };

	//make container filled with same value
	auto rep = sig::replicate(3, std::string("replicate"));
	for (auto v : rep) assert(v == "replicate");

	//make container whose element reversed
	auto rev = sig::reverse(data2);
	sig::zipWith([](int v1, int v2){ assert(v1 == v2); return 0; }, rev, std::vector<int>{2, 5, -3, 1});

	//take top n elements
	auto t1 = sig::take(2, data1);
	auto t4 = sig::take(2, data4);

	sig::zipWith([](int v1, int v2){ assert(v1 == v2); return 0; }, t1, std::vector<int>{ 1, -3 });

	for (auto v : t4) std::cout << v << ", ";		//1, 10		//unorderedであるため順不同
	std::cout << std::endl << std::endl;

	//drop top n elements
	auto d2 = sig::drop(2, data2);
	auto d3 = sig::drop(2, data3);

	sig::zipWith([](int v1, int v2){ assert(v1 == v2); return 0; }, d2, std::vector<int>{ 5, 2 });
	sig::zipWith([](int v1, int v2){ assert(v1 == v2); return 0; }, d3, std::vector<int>{ 2, 5, 10 });

#ifndef SIG_MSVC_LT1800
	//sort
	auto sorted1 = sig::sort(std::less<int>(), data1);
	auto sorted2 = sig::sort([](int l, int r){ return std::abs(l) < std::abs(r); }, data2);

	sig::zipWith([](int v1, int v2){ assert(v1 == v2); return 0; }, sorted1, std::vector<int>{ -3, 1, 2, 5, 10 });
	sig::zipWith([](int v1, int v2){ assert(v1 == v2); return 0; }, sorted2, std::vector<int>{ 1, 2, -3, 5 });
#endif

	//zip, unzip test (move and const& ver)
#ifndef SIG_MSVC_LT1800
	//move ver (run only sequence container)
	auto mdata1 = data1;
	auto mdata2 = data2;

	//make tuple from containers (container type equals data1's)
	auto zipped = sig::zip(std::move(mdata1), std::move(mdata2));	//std::vector< std::tuple<int, int, int, int>>

	auto unzipped = sig::unzip(std::move(zipped));		//std::tuple< std::vector<int>, std::vector<int>, std::vector<int>, std::vector<int>>

#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 8)
	auto rezipped = sig::zip(std::move(unzipped));		//std::vector< std::tuple<int, int, int, int>>
#endif

	sig::variadicZipWith([](int v1, int v2, std::tuple<int, int> t){
		assert(std::get<0>(t) == v1 && std::get<1>(t) == v2); return 0;
	}, data1, data2, zipped);
#endif
	
	//const ver
	const std::vector<int> cdata1{ 1, -3, 5, 2, 10 };
	const std::list<int> cdata2{ 1, -3, 5, 2 };
	const std::multiset<int, std::less<int>> cdata3{ 1, -3, 5, 2, 10 };
	const std::unordered_multiset<int> cdata4{ 1, -3, 5, 2, 10 };

	const auto czipped = sig::zip(cdata1, cdata2, cdata3, cdata4);	//std::vector< std::tuple<int, int, int, int>>

	auto cunzipped = sig::unzip(czipped);		//std::tuple< std::vector<int>, std::vector<int>, std::vector<int>, std::vector<int>>

#ifndef SIG_MSVC_LT1800
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 8)
	auto crezipped = sig::zip(cunzipped);		//std::vector< std::tuple<int, int, int, int>>
#endif
#endif

	sig::variadicZipWith([](int v1, int v2, int v3, int v4, std::tuple<int, int, int, int> t){
		assert(std::get<0>(t) == v1 && std::get<1>(t) == v2 && std::get<2>(t) == v3 && std::get<3>(t) == v4); return 0;
	}, data1, data2, data3, data4, czipped);
}