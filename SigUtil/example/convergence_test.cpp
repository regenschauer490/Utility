#include "convergence_test.h"

void ConvergenceTest()
{
	const double eps = 0.01;
	
	sig::ManageConvergenceSimple conv(eps);
	double value = 100;
	int ct = 0;

	while (!conv.update(value)){
		value /= 2;
		++ct;
	}

	std::cout << ct;
}