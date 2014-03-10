#include "calculate_test.hpp"

void ArithmeticOperationsTest()
{
	std::array<int, 4> data0{{1, 2, 3, 4 }};
	std::vector<int> data1{ 1, -3, 5 };
	std::list<double> data2{ 1.0, -3.0, 5.0 };
	std::multiset<int, std::greater<int>> data3{ 1, -3, 5 };
	std::unordered_multiset<int> data4{ 1, -3, 5, -7 };

	auto plus1 = sig::Plus(1, 1.0);
	auto plus2 = sig::Plus(data1, data2);
	auto plus3 = sig::Plus(data3, data0);
	auto plus4 = sig::Plus(data3, 1.0);
	auto plus5 = sig::Plus(1.0, data4);


	sig::CompoundAssignment([](double& v1, int v2){ v1 += v2; }, data2, data1);
	sig::CompoundAssignment([](int& v1, int v2){ v1 += v2; }, data0, 1);
}