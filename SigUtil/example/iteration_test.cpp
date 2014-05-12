#include "iteration_test.h"
#include "debug.hpp"

void ForeachTest()
{
	const std::vector<int> data1{1, 2, 3};
	const std::vector<int> data2{ 1, 1, 1 };

	auto data1_t = data1;
	auto data2_t = data2;

	sig::for_each([](int&a, int b){ a += b; }, data1_t, data2);

	sig::zipWith(
		sig::DebugEqual(),
		data1_t,
		sig::zipWith(std::plus<int>(), data1, data2)
	);


	sig::for_each([](int i, int a, int& b){ b += (a + i); }, 1, data1, data2_t);

	auto data2_tt = data2;
	for (int i = 0; i<data2.size(); ++i){
		data2_tt[i] += (data1[i] + i+1);
	}
	sig::zipWith(
		sig::DebugEqual(),
		data2_t,
		data2_tt
	);
}