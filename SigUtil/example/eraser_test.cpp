#include "eraser_test.h"

void RemoveTest()
{
	std::vector<int> data1{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };
	std::list<int> data2{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };
	std::array<int, 9> data3{{ 1, 5, 3, 3, 0, 4, 0, 1, 3 }};
	sig::Sort(data1);
	//sig::Sort(data2);
	//sig::Sort(data3);
/*
	auto removed1 = sig::RemoveDuplicates(data1, true);
	
	for (auto v : data1) std::cout << v << ", ";		//0, 1, 3, 4, 5
	std::cout << std::endl;
	for (auto v : removed1) std::cout << v << ", ";		//0, 1, 3, 3
*/
}