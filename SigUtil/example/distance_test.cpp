#include "distance_test.h"
#include "../lib/array.hpp"

const sig::array<int, 5> data1{ -1, 0, 1, 2, 3 };
const std::list<double> data2{ 1, 1.5, 2, 2.5, 3 };

void NormTest()
{
	double n1 = sig::norm_L1(data1);	// == sig::norm<1>(data1)
	double n2 = sig::norm_L2(data1);
	double n3 = sig::Norm<3>{}(data1);
	double nmax = sig::norm_max(data1);
	
	assert(sig::equal(n1, 7));
	assert(sig::equal(n2, std::sqrt(15)));
	assert(sig::equal(n3, std::pow(37, 1/3.0)));
	assert(sig::equal(nmax, 3));

	double n1d12 = sig::norm_L1(data1, data2);
	double n1d21 = sig::norm_L1(data2, data1);
	double n2d12 = sig::norm_L2(data1, data2);
	double n2d21 = sig::norm_L2(data2, data1);
	double nmax12 = sig::norm_max(data1, data2);
	double nmax21 = sig::norm_max(data2, data1);

	assert(sig::equal(n1d12, 5));
	assert(sig::equal(n1d12, n1d21));
	assert(sig::equal(n2d12, std::sqrt(7.5)));
	assert(sig::equal(n2d12, n2d21));
	assert(sig::equal(nmax12, 2));
	assert(sig::equal(nmax12, nmax21));
}

void MinkowskiDistanceTest()
{
	double m1d12 = sig::manhattan_distance(data1, data2);		// == norm_L1(data1, data2)
	double m1d21 = sig::manhattan_distance(data2, data1);

	assert(sig::equal(m1d12, 5));
	assert(sig::equal(m1d21, m1d21));

	double m2d12 = sig::euclidean_distance(data1, data2);		// == norm_L2(data1, data2)
	double m2d21 = sig::euclidean_distance(data2, data1);

	assert(sig::equal(m2d12, std::sqrt(7.5)));
	assert(sig::equal(m2d21, m2d21));

	sig::MinkowskiDistance<3> minkowski;
	double m3d12 = minkowski(data1, data2);
	double m3d21 = sig::MinkowskiDistance<3>()(data2, data1);
	
	assert(sig::equal(m3d12, std::pow(12.5, 1/3.0)));
	assert(sig::equal(m3d21, m3d21));
}

void CosineSimilarityTest()
{
	double cosim = sig::cosine_similarity(data1, data2);

	double test = std::inner_product(std::begin(data1), std::end(data1), std::begin(data2), 0.0, std::plus<double>(), std::multiplies<double>()) / (sig::norm_L2(data1) * sig::norm_L2(data2));

	assert(sig::equal(cosim, test));
}

void CanberraDistanceTest()
{
	double candist = sig::canberra_distance(data1, data2);

	assert(sig::equal(candist, 22 / 9.0));
}

void BinaryDistanceTest()
{
	// element must be an integral-type: bool, uint, int, char,...
	const sig::array<sig::uint, 4> bdata1{ 1, 1, 0, 0 };
	const std::list<bool> bdata2{ true, false, true, true };

	double bindist = sig::binary_distance(bdata1, bdata2);

	assert(sig::equal(bindist, 0.75));
}

const sig::array<double, 5> dist1{ 0.2, 0.1, 0, 0.4, 0.3 };
const std::list<double> dist2{ 0.1, 0.3, 0.1, 0.4, 0.1 };
const std::vector<double> dist3{ 0.2, 0.1, 0.1, 0.4, 0.2 };

void KL_DivergenceTest()
{
	double kl12 = sig::fromJust(sig::kl_divergence(dist1, dist2));
	//double kl21 = sig::kl_divergence(dist2, dist1);	// error because dist1 has element whose value is 0
	double kl23 = sig::fromJust(sig::kl_divergence(dist2, dist3));
	double kl32 = sig::fromJust(sig::kl_divergence(dist3, dist2));

	double test12 = 0;
	double test23 = 0;
	double test32 = 0;
	sig::for_each(
		[&](double d1, double d2, double d3){
			test12 += d1 ? d1 * std::log2(d1 / d2) : 0;
			test23 += d2 * std::log2(d2 / d3);
			test32 += d3 * std::log2(d3 / d2);
		},
		dist1, dist2, dist3
	);

	assert(sig::equal(kl12, test12));
	assert(sig::equal(kl23, test23));
	assert(sig::equal(kl32, test32));
	assert(!sig::equal(kl23, kl32));
}

void JS_DivergenceTest()
{
	double js12 = sig::fromJust(sig::js_divergence(dist1, dist2));
	double js21 = sig::fromJust(sig::js_divergence(dist2, dist1));

	double test = 0;
	sig::for_each(
		[&](double d1, double d2){
		auto r = (d1 + d2) * 0.5;
		test += (d1 ? d1 * std::log2(d1 / r) : 0) + (d2 ? d2 * std::log2(d2 / r) : 0);
		},
		dist1, dist2
	);
	test /= 2;

	assert(sig::equal(js12, test));
	assert(sig::equal(js12, js21));
}
