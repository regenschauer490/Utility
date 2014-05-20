#include "calculate_test.h"
#include "debug.hpp"

void ArithmeticOperationsTest()
{
	const sig::array<int, 4> data0{1, 2, 3, 4 };
	const std::vector<int> data1{ 1, -3, 5 };
	const std::list<double> data2{ 1.0, -3.0, 5.0 };
	const std::multiset<int, std::greater<int>> data3{ 1, -3, 5 };
	const std::unordered_multiset<int> data4{ 1, -3, 5, -7 };

	// �X�J���[ + �X�J���[
	assert(sig::plus(1, 1.0) == 2.0);

	// �x�N�g�� + �X�J���[
	sig::for_each(
		sig::DebugEqual(),
		sig::plus(data2, 1.0),
		sig::zipWith(std::plus<double>(), data2, sig::replicate(data2.size(), 1))
	);

	// �X�J���[ + �x�N�g��
	sig::for_each(
		sig::DebugEqual(),
		sig::plus(1.0, data3),
		sig::zipWith(std::plus<double>(), sig::replicate(data3.size(), 1), data3)
	);

	// �x�N�g��(sequence) + �x�N�g��(sequence)
	sig::for_each(
		sig::DebugEqual(),
		sig::plus(data1, data2), 
		sig::zipWith(std::plus<double>(), data1, data2)
	);	

	// �x�N�g��(set) + �x�N�g��(sequence)
	sig::for_each(
		sig::DebugEqual(),
		sig::plus(data3, data0),
		sig::zipWith(std::plus<double>(), data3, data0)
	);

	// �x�N�g��(hash set) + �x�N�g��(sequence)
	assert(sig::foldl(std::plus<double>(), 0.0, sig::plus(data4, data0)) == sig::foldl(std::plus<double>(), 0.0, sig::zipWith(std::plus<double>(), data4, data0)));


	// ���Z
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

	// ��Z
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

	// ���Z
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

	int sum1 = sig::sum(data0);
	double sum2 = sig::sum(data2);

	assert(sig::equal(sum1, std::accumulate(std::begin(data0), std::end(data0), 0)));
	assert(sig::equal(sum2, std::accumulate(std::begin(data2), std::end(data2), 0.0)));

	int pi1 = sig::product(data0);
	double pi2 = sig::product(data2);

	assert(sig::equal(pi1, std::accumulate(std::begin(data0), std::end(data0), 0, std::multiplies<int>{})));
	assert(sig::equal(pi2, std::accumulate(std::begin(data2), std::end(data2), 0.0, std::multiplies<double>{})));
}