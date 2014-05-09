#include "calculate_test.hpp"
#include "debug.hpp"

void ArithmeticOperationsTest()
{
	const std::array<int, 4> data0{{1, 2, 3, 4 }};
	const std::vector<int> data1{ 1, -3, 5 };
	const std::list<double> data2{ 1.0, -3.0, 5.0 };
	const std::multiset<int, std::greater<int>> data3{ 1, -3, 5 };
	const std::unordered_multiset<int> data4{ 1, -3, 5, -7 };

	// スカラー + スカラー
	assert(sig::plus(1, 1.0) == 2.0);
	
	// ベクトル + スカラー
	sig::zipWith(
		sig::DebugEqual(),
		sig::plus(data2, 1.0),
		sig::zipWith(std::plus<double>(), data2, sig::replicate(data2.size(), 1))
	);

	// スカラー + ベクトル
	sig::zipWith(
		sig::DebugEqual(),
		sig::plus(1.0, data3),
		sig::zipWith(std::plus<double>(), sig::replicate(data3.size(), 1), data3)
	);

	// ベクトル(sequence) + ベクトル(sequence)
	sig::zipWith(
		sig::DebugEqual(),
		sig::plus(data1, data2), 
		sig::zipWith(std::plus<double>(), data1, data2)
	);	

	// ベクトル(set) + ベクトル(sequence)
	sig::zipWith(
		sig::DebugEqual(),
		sig::plus(data3, data0),
		sig::zipWith(std::plus<double>(), data3, data0)
	);

	// ベクトル(hash set) + ベクトル(sequence)
	assert(sig::foldl(std::plus<double>(), 0.0, sig::plus(data4, data0)) == sig::foldl(std::plus<double>(), 0.0, sig::zipWith(std::plus<double>(), data4, data0)));


	// 減算
	assert(sig::minus(2, 1.0) == 1.0);
	assert(sig::minus(1.0, 2) == -1.0);

	sig::zipWith(
		sig::DebugEqual(),
		sig::minus(data3, 1.0),
		sig::zipWith(std::minus<double>(), data3, sig::replicate(data3.size(), 1))
	);

	sig::zipWith(
		sig::DebugEqual(),
		sig::minus(1.0, data1),
		sig::zipWith(std::minus<double>(), sig::replicate(data1.size(), 1), data1)
	);

	sig::zipWith(
		sig::DebugEqual(),
		sig::minus(data2, data0),
		sig::zipWith(std::minus<double>(), data2, data0)
	);

	// 乗算
	assert(sig::multiplies(2, 2.0) == 4.0);

	sig::zipWith(
		sig::DebugEqual(),
		sig::multiplies(data3, 2.5),
		sig::zipWith(std::multiplies<double>(), data3, sig::replicate(data3.size(), 2.5))
	);

	sig::zipWith(
		sig::DebugEqual(),
		sig::multiplies(2.5, data1),
		sig::zipWith(std::multiplies<double>(), sig::replicate(data1.size(), 2.5), data1)
	);

	sig::zipWith(
		sig::DebugEqual(),
		sig::multiplies(data2, data0),
		sig::zipWith(std::multiplies<double>(), data2, data0)
	);

	// 除算
	assert(sig::divides(2, 1.0) == 2.0);
	assert(sig::divides(1.0, 2) == 0.5);

	sig::zipWith(
		sig::DebugEqual(),
		sig::divides(data3, 3.0),
		sig::zipWith(std::divides<double>(), data3, sig::replicate(data2.size(), 3.0))
	);

	sig::zipWith(
		sig::DebugEqual(),
		sig::divides(3.0, data1),
		sig::zipWith(std::divides<double>(), sig::replicate(data1.size(), 3.0), data1)
	);

	sig::zipWith(
		sig::DebugEqual(),
		sig::divides(data2, data0),
		sig::zipWith(std::divides<double>(), data2, data0)
	);


	// 複合代入(コンテナの各要素に対して何らかの演算を行い、演算後の値を代入する)
	std::vector<int> data5{ 1, 2, 3 };
	auto data6 = data5;
	const auto data7 = data5;

	sig::compound_assignment([](int& v1, int v2){ v1 += v2; }, data5, 1);
	sig::zipWith(
		sig::DebugEqual(),
		data5,
		sig::zipWith(std::plus<int>(), data7, sig::replicate(data7.size(), 1))
	);

	sig::compound_assignment([](int& v1, int v2){ v1 -= v2; }, data6, data0);
	sig::zipWith(
		sig::DebugEqual(),
		data6,
		sig::zipWith(std::minus<int>(), data7, data0)
	);

	std::vector<std::string> str_set{"ein", "zwei", "drei"};
	sig::compound_assignment([](std::string& str, std::string add){ str += add; }, str_set, std::string("-hander"));
}