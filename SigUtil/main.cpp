#include "utility.hpp"

int main()
{
	using namespace sig;

	double d = 1.23;
	auto k = Precision(d);

	std::vector<int> vi1{-1,1, 2, 3, 4, 5,6,7,8,9,10};

	SimpleRandom<double> srand(-500, 500, true);
	Histgram<double, 7> hist(-500, 500);
	
	for (uint i = 0; i < 100; ++i){
		hist.Count(srand());
	}

	hist.Print();

	return 0;
}