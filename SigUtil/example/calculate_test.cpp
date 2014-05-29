#include "calculate_test.h"
#include "debug.hpp"

void ArithmeticOperationsTest()
{
	const sig::array<int, 4> data0{1, 2, 3, 4 };
	const std::vector<int> data1{ 1, -3, 5 };
	const std::list<double> data2{ 1.0, -3.0, 5.0 };
	const std::multiset<int, std::greater<int>> data3{ 1, -3, 5 };
	const std::unordered_multiset<int> data4{ 1, -3, 5, -7 };

	// スカラー + スカラー
	assert(sig::plus(1, 1.0) == 2.0);

	// ベクトル + スカラー
	sig::for_each(
		sig::DebugEqual(),
		sig::plus(data2, 1.0),
		sig::zipWith(std::plus<double>(), data2, sig::replicate(data2.size(), 1))
	);

	// スカラー + ベクトル
	sig::for_each(
		sig::DebugEqual(),
		sig::plus(1.0, data3),
		sig::zipWith(std::plus<double>(), sig::replicate(data3.size(), 1), data3)
	);

	// ベクトル(sequence) + ベクトル(sequence)
	sig::for_each(
		sig::DebugEqual(),
		sig::plus(data1, data2), 
		sig::zipWith(std::plus<double>(), data1, data2)
	);	

	// ベクトル(set) + ベクトル(sequence)
	sig::for_each(
		sig::DebugEqual(),
		sig::plus(data3, data0),
		sig::zipWith(std::plus<double>(), data3, data0)
	);

	// ベクトル(hash set) + ベクトル(sequence)
	assert(sig::foldl(std::plus<double>(), 0.0, sig::plus(data4, data0)) == sig::foldl(std::plus<double>(), 0.0, sig::zipWith(std::plus<double>(), data4, data0)));


	// 減算
	assert(sig::minus(2, 1.0) == 1.0);
	assert(sig::minus(1.0, 2) == -1.0);

	sig::for_each(
		sig::DebugEqual(),
		sig::minus(data3, 1.0),
		sig::zipWith(std::minus<double>(), data3, sig::replicate(data3.size(), 1))
	);

	sig::for_each(
		sig::DebugEqual(),
		sig::minus(1.0, data1),
		sig::zipWith(std::minus<double>(), sig::replicate(data1.size(), 1), data1)
	);

	sig::for_each(
		sig::DebugEqual(),
		sig::minus(data2, data0),
		sig::zipWith(std::minus<double>(), data2, data0)
	);

	// 乗算
	assert(sig::multiplies(2, 2.0) == 4.0);

	sig::for_each(
		sig::DebugEqual(),
		sig::multiplies(data3, 2.5),
		sig::zipWith(std::multiplies<double>(), data3, sig::replicate(data3.size(), 2.5))
	);

	sig::for_each(
		sig::DebugEqual(),
		sig::multiplies(2.5, data1),
		sig::zipWith(std::multiplies<double>(), sig::replicate(data1.size(), 2.5), data1)
	);

	sig::for_each(
		sig::DebugEqual(),
		sig::multiplies(data2, data0),
		sig::zipWith(std::multiplies<double>(), data2, data0)
	);

	// 除算
	assert(sig::divides(2, 1.0) == 2.0);
	assert(sig::divides(1.0, 2) == 0.5);

	sig::for_each(
		sig::DebugEqual(),
		sig::divides(data3, 3.0),
		sig::zipWith(std::divides<double>(), data3, sig::replicate(data2.size(), 3.0))
	);

	sig::for_each(
		sig::DebugEqual(),
		sig::divides(3.0, data1),
		sig::zipWith(std::divides<double>(), sig::replicate(data1.size(), 3.0), data1)
	);

	sig::for_each(
		sig::DebugEqual(),
		sig::divides(data2, data0),
		sig::zipWith(std::divides<double>(), data2, data0)
	);

	// accumulation (sum, product)
	int sum1 = sig::sum(data0);
	double sum2 = sig::sum(data2);

	assert(sig::equal(sum1, std::accumulate(std::begin(data0), std::end(data0), 0)));
	assert(sig::equal(sum2, std::accumulate(std::begin(data2), std::end(data2), 0.0)));

	int pi1 = sig::product(data0);
	double pi2 = sig::product(data2);

	assert(sig::equal(pi1, std::accumulate(std::begin(data0), std::end(data0), 0, std::multiplies<int>{})));
	assert(sig::equal(pi2, std::accumulate(std::begin(data2), std::end(data2), 0.0, std::multiplies<double>{})));

	// average, variance
	double ave = sig::average(data2);
	double var = sig::variance(data3);

	assert(sig::equal(ave, 1.0));
	assert(sig::equal(var, 32.0 / 3));

	// normalization, Standardization
	std::list<double> data5{ -5, -1.5, 0, 0.5, 5 };
	std::vector<double> data6{ -5, -1.5, 0, 0.5, 5 };
	const sig::array<double, 5> data7{ -5, -1.5, 0, 0.5, 5 };
	sig::array<int, 5> data8{ -5, -2, 0, 1, 5 };

	sig::array<double, 5> normal_test1{ 0, 0.35, 0.5, 0.55, 1 };
	sig::array<double, 5> normal_test2{ 0, 0.3, 0.5, 0.6, 1 };
	double m1 = sig::average(data6);
	double v1 = sig::variance(data6);
	sig::array<double, 5> standard_test1{ (data6[0] - m1) / v1, (data6[1] - m1) / v1, (data6[2] - m1) / v1, (data6[3] - m1) / v1, (data6[4] - m1) / v1 };
	double m2 = sig::average(data8);
	double v2 = sig::variance(data8);
	sig::array<double, 5> standard_test2{ (data8[0] - m2) / v2, (data8[1] - m2) / v2, (data8[2] - m2) / v2, (data8[3] - m2) / v2, (data8[4] - m2) / v2 };

	sig::normalize(data5);
	auto normal1 = sig::normalize(data7);
	auto normal2 = sig::normalize(data8);	// sig::array<int, 5> -> sig::array<double, 5>

	sig::for_each(sig::DebugEqual(), data5, normal_test1);
	sig::for_each(sig::DebugEqual(), normal1, normal_test1);
	sig::for_each(sig::DebugEqual(), normal2, normal_test2);

	sig::standardize(data6);
	auto standard1 = sig::standardize(data7);
	auto standard2 = sig::standardize(data8);	// sig::array<int, 5> -> sig::array<double, 5>

	sig::for_each(sig::DebugEqual(), data6, standard_test1);
	sig::for_each(sig::DebugEqual(), standard1, standard_test1);
	sig::for_each(sig::DebugEqual(), standard2, standard_test2);
}
