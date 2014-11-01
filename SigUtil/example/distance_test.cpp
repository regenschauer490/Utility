#include "distance_test.h"
#include "../lib/array.hpp"
#include "../lib/calculation/for_each.hpp"

using namespace sig;

const array<int, 5> data1{ -1, 0, 1, 2, 3 };
const std::list<double> data2{ 1, 1.5, 2, 2.5, 3 };

void NormTest()
{
	double n1 = norm_L1(data1);	// == norm<1>(data1)
	double n2 = norm_L2(data1);
	double n3 = Norm<3>{}(data1);
	double nmax = norm_max(data1);
	
	assert(equal(n1, 7));
	assert(equal(n2, std::sqrt(15)));
	assert(equal(n3, std::pow(37, 1/3.0)));
	assert(equal(nmax, 3));

	double n1d12 = norm_L1(data1, data2);
	double n1d21 = norm_L1(data2, data1);
	double n2d12 = norm_L2(data1, data2);
	double n2d21 = norm_L2(data2, data1);
	double nmax12 = norm_max(data1, data2);
	double nmax21 = norm_max(data2, data1);

	assert(equal(n1d12, 5));
	assert(equal(n1d12, n1d21));
	assert(equal(n2d12, std::sqrt(7.5)));
	assert(equal(n2d12, n2d21));
	assert(equal(nmax12, 2));
	assert(equal(nmax12, nmax21));
}

void MinkowskiDistanceTest()
{
	double m1d12 = manhattan_distance(data1, data2);		// == norm_L1(data1, data2)
	double m1d21 = manhattan_distance(data2, data1);

	assert(equal(m1d12, 5));
	assert(equal(m1d21, m1d21));

	double m2d12 = euclidean_distance(data1, data2);		// == norm_L2(data1, data2)
	double m2d21 = euclidean_distance(data2, data1);

	assert(equal(m2d12, std::sqrt(7.5)));
	assert(equal(m2d21, m2d21));

	MinkowskiDistance<3> minkowski;
	double m3d12 = minkowski(data1, data2);
	double m3d21 = MinkowskiDistance<3>()(data2, data1);
	
	assert(equal(m3d12, std::pow(12.5, 1/3.0)));
	assert(equal(m3d21, m3d21));
}

void CosineSimilarityTest()
{
	double cosim = cosine_similarity(data1, data2);

	double test = std::inner_product(std::begin(data1), std::end(data1), std::begin(data2), 0.0, std::plus<double>(), std::multiplies<double>()) / (norm_L2(data1) * norm_L2(data2));

	assert(equal(cosim, test));
}

void CanberraDistanceTest()
{
	double candist = canberra_distance(data1, data2);

	assert(equal(candist, 22 / 9.0));
}

void BinaryDistanceTest()
{
	// element must be an integral-type: bool, uint, int, char,...
	const array<uint, 4> bdata1{ 1, 1, 0, 0 };
	const std::list<bool> bdata2{ true, false, true, true };

	double bindist = binary_distance(bdata1, bdata2);

	assert(equal(bindist, 0.75));
}

const array<double, 5> dist1{ 0.2, 0.1, 0, 0.4, 0.3 };
const std::list<double> dist2{ 0.1, 0.3, 0.1, 0.4, 0.1 };
const std::vector<double> dist3{ 0.2, 0.1, 0.1, 0.4, 0.2 };

void KL_DivergenceTest()
{
	double kl12 = fromJust(kl_divergence(dist1, dist2));
	//double kl21 = kl_divergence(dist2, dist1);	// error because dist1 has element whose value is 0
	double kl23 = fromJust(kl_divergence(dist2, dist3));
	double kl32 = fromJust(kl_divergence(dist3, dist2));

	double test12 = 0;
	double test23 = 0;
	double test32 = 0;
	for_each(
		[&](double d1, double d2, double d3){
			test12 += d1 ? d1 * std::log2(d1 / d2) : 0;
			test23 += d2 * std::log2(d2 / d3);
			test32 += d3 * std::log2(d3 / d2);
		},
		dist1, dist2, dist3
	);

	assert(equal(kl12, test12));
	assert(equal(kl23, test23));
	assert(equal(kl32, test32));
	assert(!equal(kl23, kl32));
}

void JS_DivergenceTest()
{
	double js12 = *js_divergence(dist1, dist2);
	double js21 = fromJust(js_divergence(dist2, dist1));

	double test = 0;
	for_each(
		[&](double d1, double d2){
		auto r = (d1 + d2) * 0.5;
		test += (d1 ? d1 * std::log2(d1 / r) : 0) + (d2 ? d2 * std::log2(d2 / r) : 0);
		},
		dist1, dist2
	);
	test /= 2;

	assert(equal(js12, test));
	assert(equal(js12, js21));
}
