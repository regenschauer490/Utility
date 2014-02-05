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

	auto tmp = sig::Fill(true, 3);
}