#include "array_test.h"
#include "debug.hpp"

void ArrayTest()
{
	//constructor
	sig::array<int, 5> ar1;
	sig::array<int, 5> ar2{ 1, 2, 3 };
	sig::array<int, 5> ar3(3, 1);	//{ 1, 1, 1 }

	std::array<int, 5> stdar = { { 1, 2, 3, 4, 5 } };
	sig::array<int, 5> ar4{ stdar };

	sig::array<int, 5> ar5{ ar2 };

	sig::for_each(sig::DebugEqual(), ar2, std::vector<int>{1, 2, 3});	//test elements
	sig::for_each(sig::DebugEqual(), ar3, std::vector<int>{1, 1, 1});
	sig::for_each(sig::DebugEqual(), ar4, stdar);
	sig::for_each(sig::DebugEqual(), ar5, ar2);

	//copy assignment
	ar1 = ar2;
	sig::for_each(sig::DebugEqual(), ar1, ar2);

	ar1 = { 3, 2, 1 };
	sig::for_each(sig::DebugEqual(), ar1, std::vector<int>{3, 2, 1});

	ar1 = stdar;
	sig::for_each(sig::DebugEqual(), ar1, stdar);

	//equal
	auto ar6 = ar5;
	assert(ar5 == ar6);
	assert(ar6 == ar5);
	assert(ar1 != ar6);
	assert(ar6 != ar1);

	//member functions
	assert(*ar2.begin() == 1);
	assert(*(ar2.end()-1) == 3);
	assert(*ar2.rbegin() == 3);
	assert(*(ar2.rend()-1) == 1);

	assert(ar2.at(0) == 1);
	assert(ar2.at(1) == 2);
	assert(ar2.at(2) == 3);
	try{
		ar2.at(3);
	}
	catch (std::out_of_range e){}
	catch (...){ assert(false); }

	assert(ar2[0] == 1);
	assert(ar2[1] == 2);
	assert(ar2[2] == 3);

	assert(ar2.front() == 1);
	assert(ar2.back() == 3);

	ar1.clear();

	assert(ar1.empty());
	assert(!ar2.empty());

	assert(ar2.size() == 3);
	assert(ar2.max_size() == 5);

	int* raw = ar2.data();
	std::array<int, 5> stdar2 = ar2.std_array();

	ar3.fill(2);
	sig::for_each(sig::DebugEqual(), ar3, std::vector<int>{2, 2, 2, 2, 2});

	ar1.swap(ar2);
	sig::for_each(sig::DebugEqual(), ar1, ar5);
	assert(ar2.empty());

	ar1.clear();
	for (int i=0; i<(int)ar1.max_size(); ++i){
		ar1.push_back(i);
		assert(ar1[i] == i);
	}

	assert(ar1.size() == 5);
	ar1.pop_back();
	assert(ar1.size() == 4);
	assert(ar1[0] == 0);
	assert(ar1[1] == 1);
	assert(ar1[2] == 2);
	assert(ar1[3] == 3);

	ar1.insert(ar1.begin()+1, 10);
	assert(ar1[0] == 0);
	assert(ar1[1] == 10);
	assert(ar1[2] == 1);
	assert(ar1[3] == 2);
	assert(ar1[4] == 3);

	ar1.erase(ar1.begin() + 2);
	assert(ar1.size() == 4);
	assert(ar1[0] == 0);
	assert(ar1[1] == 10);
	assert(ar1[2] == 2);
	assert(ar1[3] == 3);

}
