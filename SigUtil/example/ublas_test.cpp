#include "ublas_test.h"
#include "../lib/array.hpp"

const std::vector<double> vec{ 1, 2, 3 };
const std::vector<std::vector<double>> mat{ { 2, 1, 3 }, { 1, 2, 1 }, { 3, 1, 2 } };

void UblasConvertTest()
{
	auto uvec = sig::to_vector_ublas(vec);
	auto umat = sig::to_matrix_ublas(mat);

	auto vec_r = sig::from_vector_ublas(uvec);
	auto mat_r = sig::from_matrix_ublas(umat);

	auto vec_mr = sig::from_vector_ublas(umat, 1);

	for (int i=0; i<vec.size(); ++i){
		assert(vec[i] == uvec[i]);
		assert(vec[i] == vec_r[i]);
		assert(mat[1][i] == vec_mr[i]);
	}

	for (int i = 0; i<mat.size(); ++i){
		for (int j = 0; j<mat[i].size(); ++j){
			assert(mat[i][j] == umat(i, j));
			assert(mat[i][j] == mat_r[i][j]);
		}
	}
}

void UblasCalculation()
{
	auto umat = sig::to_matrix_ublas(mat);

	auto inv_mat = sig::fromJust(sig::invert_matrix(umat));

	const sig::array<sig::array<double, 3>, 3> inv_test{{-0.375, -0.125, 0.625}, {-0.125, 0.625, -0.125}, {0.625, -0.125, -0.375}};

	for (int i = 0; i<mat.size(); ++i){
		for (int j = 0; j<mat[i].size(); ++j){
			assert(sig::equal(inv_mat(i, j), inv_test[i][j]));
		}
	}
}