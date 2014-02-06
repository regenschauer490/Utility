#include "functional_test.h"

void MapTest()
{
	std::vector<int> data1{ 1, -3, 5 };
	std::list<int> data2{ 1, -3, 5 };
	std::multiset<int, std::greater<int>> data3{ 1, -3, 5 };
	std::unordered_multiset<int> data4{ 1, -3, 5 };

	auto r1 = sig::Map([](int v){ return v * 2; }, data1);
#if _MSC_VER > 1800
	auto r2 = sig::Map([](int v){ return [v](int th){ return v < th; }; }, data2);
#endif
	auto r3 = sig::Map([](int v){ return v / 4.0; }, data3);
	auto r4 = sig::Map([](int v){ return v < 0; }, data4);

	for (auto v : data1) std::cout << v << ", ";	//1, -3, 5
	std::cout << std::endl;
	for (auto v : r1) std::cout << v << ", ";		//2, -6, 10
	std::cout << std::endl;
#if _MSC_VER > 1800
	for (auto v : data2) std::cout << v << ", ";	//1, -3, 5
	std::cout << std::endl;
	for (auto v : r2) std::cout << v(2) << ", ";	//1, 1, 0
#endif
	std::cout << std::endl;
	for (auto v : data3) std::cout << v << ", ";	//5, 1, -3
	std::cout << std::endl;
	for (auto v : r3) std::cout << v << ", ";		//1.25, 0.25, -0.75, 
	std::cout << std::endl;
	for (auto v : data4) std::cout << v << ", ";	//5, 1, -3	//ŠÂ‹«ˆË‘¶
	std::cout << std::endl;
	for (auto v : r4) std::cout << v << ", ";		//1, 0, 0	//unordered‚Å‚ ‚é‚½‚ß‡•s“¯
}

void ZipWithTest()
{
	std::vector<int> data1{ 1, -3, 5 };
	std::list<int> data2{ 1, -3, 5 };
	std::multiset<int> data3{ 1, -3, 5 };
	std::unordered_multiset<int> data4{ 1, -3, 5 };

	auto mask = sig::Merge<std::list<bool>>(std::deque<bool>{false}, sig::Fill(2, true));

	for (auto v : mask) std::cout << v << ", ";		//0, 1, 1
	std::cout << std::endl;

	auto r12 = sig::ZipWith([](int v1, int v2){ return v1 * v2; }, data1, data2);
	auto r23 = sig::ZipWith([](int v1, int v2){ return v1 == v2; }, data2, data3);
	auto r4 = sig::ZipWith([](int v1, bool f){ return f ? v1 : 0; }, data4, mask);

	for (auto v : data1) std::cout << v << ", ";	//1, -3, 5
	std::cout << std::endl;
	for (auto v : data2) std::cout << v << ", ";	//1, -3, 5
	std::cout << std::endl;
	for (auto v : r12) std::cout << v << ", ";		//1, 9, 25
	std::cout << std::endl;
	for (auto v : data3) std::cout << v << ", ";	//-3, 1, 5
	std::cout << std::endl;
	for (auto v : r23) std::cout << v << ", ";		//0, 0, 1
	std::cout << std::endl;
	for (auto v : data4) std::cout << v << ", ";	//5, 1, -3	//ŠÂ‹«ˆË‘¶
	std::cout << std::endl;
	for (auto v : r4) std::cout << v << ", ";		//0, 1, -3
	std::cout << std::endl;

}

void HigherOrederFuncTest()
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
	for (auto v : data5) std::cout << v << ", ";	//a, b, c, d, e	//ŠÂ‹«ˆË‘¶
	std::cout << std::endl << std::endl;

	auto h123 = sig::HigherOrderFunction([](int i, double d, bool b){ return b ? i * d : 0; }, data1, data2, data3);
	auto h12345 = sig::HigherOrderFunction([](int i1, double d, bool b, int i2, std::string s){
		return b ? s + std::to_string(i1) : std::to_string(i2 * d);
	}, data1, data2, data3, data4, data5);

	for (auto v : h123) std::cout << v << ", ";		//0, -6.6, 0, 44
	std::cout << std::endl;
	for (auto v : h12345) std::cout << v << ", ";	//-3.300000, b-3, 16.500000, d10
	std::cout << std::endl;
}

void FunctionalTest()
{
	std::vector<int> data1{ 1, -3, 5, 2, 10 };
	std::list<int> data2{ 1, -3, 5, 2 };
	std::multiset<int> data3{ 1, -3, 5, 2, 10 };
	std::unordered_multiset<int> data4{ 1, -3, 5, 2, 10 };

	for (auto v : data1) std::cout << v << ", ";	//1, -3, 5, 2, 10
	std::cout << std::endl;
	for (auto v : data2) std::cout << v << ", ";	//1, -3, 5, 2
	std::cout << std::endl;
	for (auto v : data3) std::cout << v << ", ";	//-3, 1, 2, 5, 10
	std::cout << std::endl;
	for (auto v : data4) std::cout << v << ", ";	//10, 1, 5, -3, 2	//ŠÂ‹«ˆË‘¶
	std::cout << std::endl << std::endl;


	auto zipped = sig::Zip(data1, data2, data3, data4);

	for (auto v : zipped) std::cout << std::get<0>(v) << ", " << std::get<1>(v) << ", " << std::get<2>(v) << ", " << std::get<3>(v) << std::endl;
	//1, 1, -3, 1
	//-3, -3, 1, -3
	//5, 5, 2, 5
	//2, 2, 5, 10
	std::cout << std::endl;


	auto fill = sig::Fill(3, std::string("fill"));
	auto rev = sig::Reverse(data2);

	for (auto v : fill) std::cout << v << ", ";		//fill, fill, fill
	std::cout << std::endl;
	for (auto v : rev) std::cout << v << ", ";		//2, 5, -3, 1
	std::cout << std::endl << std::endl;


	auto t1 = sig::Take(2, data1);
	auto t4 = sig::Take(2, data4);

	for (auto v : t1) std::cout << v << ", ";		//1, -3
	std::cout << std::endl;
	for (auto v : t4) std::cout << v << ", ";		//1, 10		//unordered‚Å‚ ‚é‚½‚ß‡•s“¯
	std::cout << std::endl << std::endl;


	auto d2 = sig::Drop(2, data2);
	auto d3 = sig::Drop(2, data3);

	for (auto v : d3) std::cout << v << ", ";		//2, 5, 10
	std::cout << std::endl << std::endl;
}