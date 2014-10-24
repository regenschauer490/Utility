#include "calculate_test.h"
#include "debug.hpp"

using namespace sig;

const array<int, 4> data0{1, 2, 3, 4 };
const std::vector<int> data1{ 1, -3, 5 };
const std::list<double> data2{ 1.0, -3.0, 5.0 };
const std::multiset<int, std::greater<int>> data3{ 1, -3, 5 };
const std::unordered_multiset<int> data4{ 1, -3, 5, -7 };
const std::unordered_multiset<char> data5{ 'a', 'b', 'c', 'd' };
const array<std::string, 3> data6{ "A", "B", "C" };


void ArithmeticOperationsTest()
{
	// スカラー + スカラー
	assert(plus(1, 1.0) == 2.0);

	// ベクトル + スカラー
	for_each(
		DebugEqual(),
		plus(data2, 1.0),
		zipWith(std::plus<double>(), data2, replicate(data2.size(), 1))
	);

	// スカラー + ベクトル
	for_each(
		DebugEqual(),
		plus(1.0, data3),
		zipWith(std::plus<double>(), replicate(data3.size(), 1), data3)
	);
	std::vector<int> vec1{ 1, 2, 3 };
	std::vector<int> vec2{ 3, 2, 1 };

	// ベクトル(sequence) + ベクトル(sequence)
	for_each(
		DebugEqual(),
		plus(data1, data2), 
		zipWith(std::plus<double>(), data1, data2)
	);	

	// ベクトル(set) + ベクトル(sequence)
	for_each(
		DebugEqual(),
		plus(data3, data0),
		zipWith(std::plus<double>(), data3, data0)
	);

	// ベクトル(hash set) + ベクトル(sequence)
	assert(foldl(std::plus<double>(), 0.0, plus(data4, data0)) == foldl(std::plus<double>(), 0.0, zipWith(std::plus<double>(), data4, data0)));


	// 減算
	assert(minus(2, 1.0) == 1.0);
	assert(minus(1.0, 2) == -1.0);

	for_each(
		DebugEqual(),
		minus(data3, 1.0),
		zipWith(std::minus<double>(), data3, replicate(data3.size(), 1))
	);

	for_each(
		DebugEqual(),
		minus(1.0, data1),
		zipWith(std::minus<double>(), replicate(data1.size(), 1), data1)
	);

	for_each(
		DebugEqual(),
		minus(data2, data0),
		zipWith(std::minus<double>(), data2, data0)
	);

	// 乗算
	assert(multiplies(2, 2.0) == 4.0);

	for_each(
		DebugEqual(),
		multiplies(data3, 2.5),
		zipWith(std::multiplies<double>(), data3, replicate(data3.size(), 2.5))
	);

	for_each(
		DebugEqual(),
		mult(2.5, data1),
		zipWith(std::multiplies<double>(), replicate(data1.size(), 2.5), data1)
	);

	for_each(
		DebugEqual(),
		mult(data2, data0),
		zipWith(std::multiplies<double>(), data2, data0)
	);

	// 除算
	assert(divides(2, 1.0) == 2.0);
	assert(divides(1.0, 2) == 0.5);

	for_each(
		DebugEqual(),
		divides(data3, 3.0),
		zipWith(std::divides<double>(), data3, replicate(data2.size(), 3.0))
	);

	for_each(
		DebugEqual(),
		div(3.0, data1),
		zipWith(std::divides<double>(), replicate(data1.size(), 3.0), data1)
	);

	for_each(
		DebugEqual(),
		div(data2, data0),
		zipWith(std::divides<double>(), data2, data0)
	);
}

void StatisticalOperationTest()
{
	// accumulation (sum, product)
	const array<TestInt, 3> dataT{ 1, 2, 3 };
	const array<array<int, 3>, 2> dmat1{ { 1, 2, 3 }, { 4, 5, 6 } };
	const std::vector<std::vector<double>> dmat2{ { 1.1, 2.2, 3.3 }, { -1.1, -2.2, -3.3 } };
	const int64_t tt1 = 100000, tt2 = 100000;
	const std::vector<int> data_big1(tt1, tt2);
	const int64_t tt3 = 10, tt4 = 10;
	const std::vector<int> data_big2(tt3, tt4);

	int sum1 = sum(data0);
	double sum2 = sum(data2);
	int sum3 = sum(dmat1, [](array<int, 3> const& e){ return e[1]; });
	int sum4 = sum(dataT, [](TestInt const& e){ return e.value(); });
	int64_t sum5 = sum<int64_t>(data_big1);

	assert(equal(sum1, data0[0] + data0[1] + data0[2] + data0[3]));
	assert(equal(sum2, std::accumulate(std::begin(data2), std::end(data2), 0.0)));
	assert(equal(sum3, dmat1[0][1] + dmat1[1][1]));
	assert(equal(sum4, dataT[0].value() + dataT[1].value() + dataT[2].value()));
	assert(equal(sum5, tt1 * tt2));


	int sum_row1 = sum_row(dmat1, 0);
	double sum_row2 = sum_row(dmat2, 1);
	int sum_col1 = sum_col(dmat1, 0);
	double sum_col2 = sum_col(dmat2, 2);

	assert(equal(sum_row1, dmat1[0][0] + dmat1[0][1] + dmat1[0][2]));
	assert(equal(sum_row2, dmat2[1][0] + dmat2[1][1] + dmat2[1][2]));
	assert(equal(sum_col1, dmat1[0][0] + dmat1[1][0]));
	assert(equal(sum_col2, dmat2[0][2] + dmat2[1][2]));


	int pi1 = product(data0);
	double pi2 = product(data2);
	int pi3 = product(dmat1, [](array<int, 3> const& e){ return e[1]; });
	int pi4 = sum(dataT, [](TestInt const& e){ return e.value(); });
	int64_t pi5 = product<int64_t>(data_big2);

	assert(equal(pi1, data0[0] * data0[1] * data0[2] * data0[3]));
	assert(equal(pi2, std::accumulate(std::begin(data2), std::end(data2), 1.0, std::multiplies<double>{})));
	assert(equal(pi3, dmat1[0][1] * dmat1[1][1]));
	assert(equal(pi4, dataT[0].value() * dataT[1].value() * dataT[2].value()));
	assert(equal(pi5, std::pow(tt4, tt3)));


	int pi_row1 = product_row(dmat1, 0);
	double pi_row2 = product_row(dmat2, 1);
	int pi_col1 = product_col(dmat1, 0);
	double pi_col2 = product_col(dmat2, 2);

	assert(equal(pi_row1, dmat1[0][0] * dmat1[0][1] * dmat1[0][2]));
	assert(equal(pi_row2, dmat2[1][0] * dmat2[1][1] * dmat2[1][2]));
	assert(equal(pi_col1, dmat1[0][0] * dmat1[1][0]));
	assert(equal(pi_col2, dmat2[0][2] * dmat2[1][2]));


	// average, variance
	double ave = average(data2);
	double var = variance(data3);

	assert(equal(ave, 1.0));
	assert(equal(var, 32.0 / 3));


	// normalization, Standardization
	std::list<double> data5{ -5, -1.5, 0, 0.5, 5 };
	std::vector<double> data6{ -5, -1.5, 0, 0.5, 5 };
	const array<double, 5> data7{ -5, -1.5, 0, 0.5, 5 };
	array<double, 5> data8{ -5, -2, 0, 1, 5 };


	double m1 = average(data6);
	double m2 = average(data8);
	double v1 = variance(data6);
	double v2 = variance(data8);
	int m3 = average(data_big1);

	array<double, 5> normal_test1{ 0, 0.35, 0.5, 0.55, 1 };
	array<double, 5> normal_test2{ 0, 0.3, 0.5, 0.6, 1 };
	array<double, 5> standard_test1{ (data6[0] - m1) / v1, (data6[1] - m1) / v1, (data6[2] - m1) / v1, (data6[3] - m1) / v1, (data6[4] - m1) / v1 };
	array<double, 5> standard_test2{ (data8[0] - m2) / v2, (data8[1] - m2) / v2, (data8[2] - m2) / v2, (data8[3] - m2) / v2, (data8[4] - m2) / v2 };
	assert(equal(m3, tt2));

	auto normal1 = normalize(data7);
	auto normal2 = normalize(data8, 0);	// 第2引数はダミー(data8はconstでないため、戻り値ありの関数を呼び出すために必要)
	normalize(data5);	

	for_each(DebugEqual(), data5, normal_test1);
	for_each(DebugEqual(), normal1, normal_test1);
	for_each(DebugEqual(), normal2, normal_test2);

	auto standard1 = standardize(data7);
	auto standard2 = standardize(data8, 0);	// 第2引数はダミー
	standardize(data6);	

	for_each(DebugEqual(), data6, standard_test1);
	for_each(DebugEqual(), standard1, standard_test1);
	for_each(DebugEqual(), standard2, standard_test2);

	// normalize distribution
	array<double, 5> data_dist1{ 1, 2, 3, 4, 5 };
	const array<int, 5> data_dist2{ 1, 2, 3, 4, 5 };
	array<double, 5> normal_dist_test{ 1.0/15, 2.0/15, 3.0/15, 4.0/15, 5.0/15  };

	normalize_dist(data_dist1);
	auto normal_dist2 = normalize_dist(data_dist2);

	assert(equal(sum(data_dist1), 1.0));
	assert(equal(sum(normal_dist2), 1.0));
	for_each(DebugEqual(), data_dist1, normal_dist_test);
	for_each(DebugEqual(), normal_dist2, normal_dist_test);
}


void ForeachTest()
{
	auto data1_t = data1;
	auto data2_t = data2;
	auto data6_t = data6;

	for_each([](double& a, int b){ a += b; }, data2_t, data0);

	assert_foreach([](double d0, int d1){ return d1 + d0; }, data2_t, data0, data2);

	// ループ変数も用意された版 (第2引数でループ変数の初期値を指定)
	// 注：ループ変数の値 ≠ インデックス
	for_each([](int i, int a, int& b){ b += (a + i); }, 1, data0, data1_t);

	auto data1_tt = data1;
	for (unsigned i = 0; i<data1.size(); ++i){
		data1_tt[i] += (data0[i] + i + 1);
	}
	assert_foreach(Identity(), data1_t, data1_tt);

	// 上記2つの関数の引数は可変長
	for_each([](int i, int a, double b, int c, char d, std::string& e)
	{
		e.append(std::to_string(i) + d + std::to_string(a + b) + d + std::to_string(c));
	}
	, -1, data1, data2, data3, data5, data6_t
	);

}


void CompoundAssignmentTest()
{
	auto data1_t = data1;
	auto data2_t = data2;

	// 複合代入(コンテナの各要素に対して何らかの演算を行い、演算後の値を代入する)
	compound_assignment([](int& v1, int v2){ v1 += v2; }, data1_t, 1);

	assert_foreach([](int d1){ return d1 + 1; }, data1_t, data1);

	// ex2
	compound_assignment([](double& v1, int v2){ v1 -= v2; }, data2_t, data0);

	assert_foreach([](double d2, int d0){ return d2 - d0; }, data2_t, data2, data0);

	// ex3
	const std::vector<std::string> str{ "ein", "zwei", "drei" };
	auto str_t = str;
	compound_assignment([](std::string& str, std::string add){ str += add; }, str_t, std::string("-hander"));

	assert_foreach([](std::string s){ return s + std::string("-hander"); }, str_t, str);

}

