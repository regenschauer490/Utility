#include "array_test.h"
#include "debug.hpp"

using namespace sig;

void ArrayTest()
{
	//constructor
	array<TestInt, 5> ar1;
	array<TestInt, 4> ar2{ 1, 2, 3 };
	array<TestInt, 5> ar3(3, 1);	//{ 1, 1, 1 }

	std::array<TestInt, 4> stdar = { { 1, 2, 3, 4 } };
	array<TestInt, 5> ar4{ stdar };

	array<TestInt, 5> ar5{ ar2 };
	array<TestInt, 5> ar6{ std::move(ar5) };

	assert_foreach(Identity(), ar2, std::vector<TestInt>{1, 2, 3});	//test elements
	assert_foreach(Identity(), ar3, std::vector<TestInt>{1, 1, 1});
	assert_foreach(Identity(), ar4, stdar);
	assert(ar5.size() == 0);
	assert_foreach(Identity(), ar6, ar2);

	//copy assignment
	ar1 = ar2;
	assert_foreach(Identity(), ar1, ar2);

	ar1 = { 3, 2, 1 };
	assert_foreach(Identity(), ar1, std::vector<TestInt>{3, 2, 1});

	ar1 = stdar;
	assert_foreach(Identity(), ar1, stdar);

	//equal
	auto ar7 = ar6;
	assert(ar6 == ar7);
	assert(ar7 == ar6);
	assert(ar1 != ar7);
	assert(ar7 != ar1);

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
	assert(ar2.max_size() == 4);

	TestInt* raw = ar2.data();
	std::array<TestInt, 4> stdar2 = ar2.std_array();

	ar3.fill(2);
	assert_foreach(Identity(), ar3, std::vector<TestInt>{2, 2, 2, 2, 2});

	ar3 = ar2;
	ar1.swap(ar3);
	assert_foreach(Identity(), ar1, ar6);
	assert(ar3.empty());

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

	auto it1 = ar1.insert(ar1.begin()+1, 10);
	assert(*it1 == 10);
	assert(ar1[0] == 0);
	assert(ar1[1] == 10);
	assert(ar1[2] == 1);
	assert(ar1[3] == 2);
	assert(ar1[4] == 3);

	auto it2 = ar1.erase(it1 + 1);
	assert(*it2 == 2);
	assert(ar1.size() == 4);
	assert(ar1[0] == 0);
	assert(ar1[1] == 10);
	assert(ar1[2] == 2);
	assert(ar1[3] == 3);
}
