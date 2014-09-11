#include "iteration_test.h"
#include "debug.hpp"

const std::array<int, 4> data0{ { 1, 2, 3, 4 } };
const std::list<int> data1{ 1, 1, 1 };
const std::vector<int> data2{1, 2, 3};
const std::multiset<double, std::greater<double>> data3{ 1.1, 2.2, 3.3 };
const std::unordered_multiset<char> data4{ 'a', 'b', 'c', 'd' };
const sig::array<std::string, 3> data5{ "A", "B", "C" };

void ForeachTest()
{
	auto data1_t = data1;
	auto data2_t = data2;
	auto data5_t = data5;

	// 要素毎に、data1_tにdata0の値を加える
	sig::for_each([](int& a, int b){ a += b; }, data1_t, data0);
	
	sig::assert_foreach([](int d0, int d1){ return d1 + d0; }, data1_t, data0, data1);

	// ループ変数も用意された版 (第2引数でループ変数の初期値を指定)
	// 注：ループ変数の値 ≠ インデックス
	sig::for_each([](int i, int a, int& b){ b += (a + i); }, 1, data0, data2_t);

	auto data2_tt = data2;
	for (unsigned i = 0; i<data2.size(); ++i){
		data2_tt[i] += (data0[i] + i+1);
	}
	sig::assert_foreach(sig::Identity(), data2_t, data2_tt);

	// 上記2つの関数の引数は可変長
	sig::for_each([](int i, int a, int b, double c, char d, std::string& e)
	{
		e.append(std::to_string(i) + d + std::to_string(a + b) + d + std::to_string(c)); 
	}
	, -1, data1, data2, data3, data4, data5_t);
}

void FoldZipWithTest()
{
	const auto data1_t = data1;
	const auto data2_t = data2;

	int fz1 = sig::fold_zipWith(
		std::multiplies<int>(),
		std::plus<int>(),
		0, data1_t, data2_t
	);

	assert(sig::equal(fz1, 6));
}

void CompoundAssignmentTest()
{
	auto data1_t = data1;
	auto data2_t = data2;

	// 複合代入(コンテナの各要素に対して何らかの演算を行い、演算後の値を代入する)
	sig::compound_assignment([](int& v1, int v2){ v1 += v2; }, data1_t, 1);
	
	sig::assert_foreach([](int d1){ return d1 + 1; }, data1_t, data1);

	// ex2
	sig::compound_assignment([](int& v1, int v2){ v1 -= v2; }, data2_t, data0);

	sig::assert_foreach([](int d2, int d0){ return d2 - d0; }, data2_t, data2, data0);

	// ex3
	const std::vector<std::string> str{ "ein", "zwei", "drei" };
	auto str_t = str;
	sig::compound_assignment([](std::string& str, std::string add){ str += add; }, str_t, std::string("-hander"));

	sig::assert_foreach([](std::string s){ return s + std::string("-hander"); }, str_t, str);
}
