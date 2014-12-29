#include "ublas_test.h"
#include "../lib/functional/high_order.hpp"

using namespace sig;

const std::vector<double> vec{ 1.1, 2.2, 3.3 };
const std::vector<std::vector<int>> mat{ { 2, 1, 3 }, { 1, 2, 1 }, { 3, 1, 2 } };

const auto uvec_raw = [](){
	boost::numeric::ublas::vector<double> tmp(3);
	tmp[0] = 1.1;
	tmp[1] = 2.2;
	tmp[2] = 3.3;

	return tmp;
}();

void UblasConvertTest()
{
	auto uvec = to_vector_ublas(vec);
	auto umat = to_matrix_ublas(mat);

	auto vec_r = from_vector_ublas(uvec + uvec);
	auto mat_r = from_matrix_ublas(boost::numeric::ublas::prod(umat, umat));

	auto vec_mr = from_matrix_ublas(umat, 1);

	for (uint i=0; i<vec.size(); ++i){
		assert(uvec[i] == vec[i]);
		assert(vec_r[i] == vec[i] + uvec[i]);
		assert(vec_mr[i] == mat[1][i]);
	}

	for (uint i = 0; i<mat.size(); ++i){
		for (int j = 0; j<mat[i].size(); ++j){
			assert(umat(i, j) == mat[i][j]);
			assert(mat_r[i][j] == mat[i][0] * mat[0][j] + mat[i][1] * mat[1][j] + mat[i][2] * mat[2][j]);
		}
	}


	auto mpu = map_v([](double v){ return static_cast<int>(v); }, uvec);

	auto mp_test = array<int, 3>{static_cast<int>(uvec[0]), static_cast<int>(uvec[1]), static_cast<int>(uvec[2])};
	for (uint i = 0; i<mp_test.size(); ++i){
		assert(mpu(i) == mp_test[i]);
	}

	//uvec = mpu;
}

void UblasCalculation()
{
	const auto uvec = to_vector_ublas(vec);
	const auto umat = to_matrix_ublas(mat);

	auto inv_umat = fromJust(invert_matrix(umat));

	auto inv_mat = from_matrix_ublas(inv_umat);

	const array<array<double, 3>, 3> inv_test{{-0.375, -0.125, 0.625}, {-0.125, 0.625, -0.125}, {0.625, -0.125, -0.375}};

	for (uint i = 0; i<mat.size(); ++i){
		for (uint j = 0; j<mat[i].size(); ++j){
			assert(equal(inv_umat(i, j), inv_test[i][j]));
			assert(equal(inv_mat[i][j], inv_test[i][j]));
		}
	}


	auto uvec2 = to_vector_ublas(vec);
	auto uvec3 = uvec2;
	auto umat2 = to_matrix_ublas(mat);

	const int c = 1;
	for_each_v([&](double& v){ v += c; }, uvec2);
	for_each_v([](double& v, double c){ v += c * 2; }, uvec3, uvec);
	for_each_m([&](int& v){ v += c; }, umat2);

	auto caa_test1 = array<double, 3>{uvec[0] + 1, uvec[1] + 1, uvec[2] + 1 };
	auto caa_test2 = array<double, 3>{uvec[0] * 3, uvec[1] * 3, uvec[2] * 3 };

	for (uint i = 0; i<vec.size(); ++i){
		assert(uvec2(i) == caa_test1[i]);
		assert(equal(uvec3(i), caa_test2[i]));
	}


	matrix_u<int> A(3, 3, 0);
	A(0, 0) = 1; A(0, 1) = -2; A(0, 2) = 3;
	A(1, 0) = 3; A(1, 1) = 1; A(1, 2) = -5;
	A(2, 0) = -2; A(2, 1) = 6; A(2, 2) = -9;

	vector_u<int> b(3);
	b(0) = 1; b(1) = -4; b(2) = -2;

	auto solved = *matrix_vector_solve(A, b);

	assert(sig::equal(solved(0), 1));
	assert(sig::equal(solved(1), 3));
	assert(sig::equal(solved(2), 2));
}
