#include "calculate_test.h"
#include "debug.hpp"

class Tes
{
	int v_;

public:
	Tes() = default;
	explicit Tes(int v) : v_(v){};

	int get() const{ return v_; }
};

const sig::array<int, 4> data0{1, 2, 3, 4 };
const std::vector<int> data1{ 1, -3, 5 };
const std::list<double> data2{ 1.0, -3.0, 5.0 };
const std::multiset<int, std::greater<int>> data3{ 1, -3, 5 };
const std::unordered_multiset<int> data4{ 1, -3, 5, -7 };


void ArithmeticOperationsTest()
{
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
		sig::mult(2.5, data1),
		sig::zipWith(std::multiplies<double>(), sig::replicate(data1.size(), 2.5), data1)
	);

	sig::for_each(
		sig::DebugEqual(),
		sig::mult(data2, data0),
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
		sig::div(3.0, data1),
		sig::zipWith(std::divides<double>(), sig::replicate(data1.size(), 3.0), data1)
	);

	sig::for_each(
		sig::DebugEqual(),
		sig::div(data2, data0),
		sig::zipWith(std::divides<double>(), data2, data0)
	);
}

void StatisticalOperationTest()
{
	// accumulation (sum, product)
	const sig::array<Tes, 3> dataT{ Tes(1), Tes(2), Tes(3) };
	const sig::array<sig::array<int, 3>, 2> dmat1{ { 1, 2, 3 }, { 4, 5, 6 } };
	const std::vector<std::vector<double>> dmat2{ { 1.1, 2.2, 3.3 }, { -1.1, -2.2, -3.3 } };
	const int64_t tt1 = 100000, tt2 = 100000;
	const std::vector<int> data_big1(tt1, tt2);
	const int64_t tt3 = 10, tt4 = 10;
	const std::vector<int> data_big2(tt3, tt4);

	int sum1 = sig::sum(data0);
	double sum2 = sig::sum(data2);
	int sum3 = sig::sum(dmat1, [](sig::array<int, 3> const& e){ return e[1]; });
	int sum4 = sig::sum(dataT, [](Tes const& e){ return e.get(); });
	int64_t sum5 = sig::sum<int64_t>(data_big1);

	assert(sig::equal(sum1, data0[0] + data0[1] + data0[2] + data0[3]));
	assert(sig::equal(sum2, std::accumulate(std::begin(data2), std::end(data2), 0.0)));
	assert(sig::equal(sum3, dmat1[0][1] + dmat1[1][1]));
	assert(sig::equal(sum4, dataT[0].get() + dataT[1].get() + dataT[2].get()));
	assert(sig::equal(sum5, tt1 * tt2));


	int sum_row1 = sig::sum_row(dmat1, 0);
	double sum_row2 = sig::sum_row(dmat2, 1);
	int sum_col1 = sig::sum_col(dmat1, 0);
	double sum_col2 = sig::sum_col(dmat2, 2);

	assert(sig::equal(sum_row1, dmat1[0][0] + dmat1[0][1] + dmat1[0][2]));
	assert(sig::equal(sum_row2, dmat2[1][0] + dmat2[1][1] + dmat2[1][2]));
	assert(sig::equal(sum_col1, dmat1[0][0] + dmat1[1][0]));
	assert(sig::equal(sum_col2, dmat2[0][2] + dmat2[1][2]));


	int pi1 = sig::product(data0);
	double pi2 = sig::product(data2);
	int pi3 = sig::product(dmat1, [](sig::array<int, 3> const& e){ return e[1]; });
	int pi4 = sig::sum(dataT, [](Tes const& e){ return e.get(); });
	int64_t pi5 = sig::product<int64_t>(data_big2);

	assert(sig::equal(pi1, data0[0] * data0[1] * data0[2] * data0[3]));
	assert(sig::equal(pi2, std::accumulate(std::begin(data2), std::end(data2), 1.0, std::multiplies<double>{})));
	assert(sig::equal(pi3, dmat1[0][1] * dmat1[1][1]));
	assert(sig::equal(pi4, dataT[0].get() * dataT[1].get() * dataT[2].get()));
	assert(sig::equal(pi5, std::pow(tt4, tt3)));


	int pi_row1 = sig::product_row(dmat1, 0);
	double pi_row2 = sig::product_row(dmat2, 1);
	int pi_col1 = sig::product_col(dmat1, 0);
	double pi_col2 = sig::product_col(dmat2, 2);

	assert(sig::equal(pi_row1, dmat1[0][0] * dmat1[0][1] * dmat1[0][2]));
	assert(sig::equal(pi_row2, dmat2[1][0] * dmat2[1][1] * dmat2[1][2]));
	assert(sig::equal(pi_col1, dmat1[0][0] * dmat1[1][0]));
	assert(sig::equal(pi_col2, dmat2[0][2] * dmat2[1][2]));


	// average, variance
	double ave = sig::average(data2);
	double var = sig::variance(data3);

	assert(sig::equal(ave, 1.0));
	assert(sig::equal(var, 32.0 / 3));


	// normalization, Standardization
	std::list<double> data5{ -5, -1.5, 0, 0.5, 5 };
	std::vector<double> data6{ -5, -1.5, 0, 0.5, 5 };
	const sig::array<double, 5> data7{ -5, -1.5, 0, 0.5, 5 };
	sig::array<double, 5> data8{ -5, -2, 0, 1, 5 };


	double m1 = sig::average(data6);
	double m2 = sig::average(data8);
	double v1 = sig::variance(data6);
	double v2 = sig::variance(data8);
	int m3 = sig::average(data_big1);

	sig::array<double, 5> normal_test1{ 0, 0.35, 0.5, 0.55, 1 };
	sig::array<double, 5> normal_test2{ 0, 0.3, 0.5, 0.6, 1 };
	sig::array<double, 5> standard_test1{ (data6[0] - m1) / v1, (data6[1] - m1) / v1, (data6[2] - m1) / v1, (data6[3] - m1) / v1, (data6[4] - m1) / v1 };
	sig::array<double, 5> standard_test2{ (data8[0] - m2) / v2, (data8[1] - m2) / v2, (data8[2] - m2) / v2, (data8[3] - m2) / v2, (data8[4] - m2) / v2 };
	assert(sig::equal(m3, tt2));

	auto normal1 = sig::normalize(data7);
	auto normal2 = sig::normalize(data8, 0);	// 第2引数はダミー(data8はconstでないため、戻り値ありの関数を呼び出すために必要)
	sig::normalize(data5);	

	sig::for_each(sig::DebugEqual(), data5, normal_test1);
	sig::for_each(sig::DebugEqual(), normal1, normal_test1);
	sig::for_each(sig::DebugEqual(), normal2, normal_test2);

	auto standard1 = sig::standardize(data7);
	auto standard2 = sig::standardize(data8, 0);	// 第2引数はダミー
	sig::standardize(data6);	

	sig::for_each(sig::DebugEqual(), data6, standard_test1);
	sig::for_each(sig::DebugEqual(), standard1, standard_test1);
	sig::for_each(sig::DebugEqual(), standard2, standard_test2);

	// normalize distribution
	sig::array<double, 5> data_dist1{ 1, 2, 3, 4, 5 };
	const sig::array<int, 5> data_dist2{ 1, 2, 3, 4, 5 };
	sig::array<double, 5> normal_dist_test{ 1.0/15, 2.0/15, 3.0/15, 4.0/15, 5.0/15  };

	sig::normalize_dist(data_dist1);
	auto normal_dist2 = sig::normalize_dist(data_dist2);

	assert(sig::equal(sig::sum(data_dist1), 1.0));
	assert(sig::equal(sig::sum(normal_dist2), 1.0));
	sig::for_each(sig::DebugEqual(), data_dist1, normal_dist_test);
	sig::for_each(sig::DebugEqual(), normal_dist2, normal_dist_test);
}
