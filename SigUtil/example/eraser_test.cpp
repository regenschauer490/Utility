#include "eraser_test.h"

void RemoveTest()
{
	std::vector<int> data1{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };
	std::list<int> data2{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };
	std::multiset<int, std::greater<int>> data3{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };
	std::unordered_multiset<int> data4{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };

	auto removed1 = sig::RemoveDuplicates(data1, true);
	auto removed2 = sig::RemoveDuplicates(data2, true);
	auto removed3 = sig::RemoveDuplicates(data3, true);
	auto removed4 = sig::RemoveDuplicates(data4, true);
	
	for (auto v : data1) std::cout << v << ", ";		//1, 5, 3, 0, 4
	std::cout << std::endl;
	for (auto v : removed1) std::cout << v << ", ";		//3, 0, 1, 3
	std::cout << std::endl;
	for (auto v : data2) std::cout << v << ", ";		//0, 1, 3, 4, 5
	std::cout << std::endl;
	for (auto v : removed2) std::cout << v << ", ";		//3, 0, 1, 3
	std::cout << std::endl;
	for (auto v : data3) std::cout << v << ", ";		//0, 1, 3, 4, 5
	std::cout << std::endl;
	for (auto v : removed3) std::cout << v << ", ";		//0, 1, 3, 3
	std::cout << std::endl;
	for (auto v : data4) std::cout << v << ", ";		//1, 5, 3, 0, 4
	std::cout << std::endl;
	for (auto v : removed4) std::cout << v << ", ";		//1, 3, 3, 0

#if _MSC_VER > 1800
	std::multimap<int, std::string> data5{ { 1, "a"}, {5, "b"}, {3, "c"}, {3, "d"}, {0, "e"}, {4, "f"}, {0, "g"}, {1, "h"}, {3, "i"} };
	auto removed5 = sig::RemoveDuplicates(data5, true);
	for (auto v : data5) std::cout << v.second << ", ";		//e, g, a, h, c, d, i, f, b,
	std::cout << std::endl;
	for (auto v : removed5) std::cout << v.second << ", ";	//
	std::cout << std::endl;
#endif
}