#include "lib/string.hpp"
#include "lib/file.hpp"
#include "lib/tool.hpp"
#include "lib/modify.hpp"
#include "lib/functional.hpp"

#include "example/string_test.h"
#include "example/file_test.h"
#include "example/tool_test.h"
#include "example/modify_test.h"
#include "example/functional_test.h"
#include "example/calculate_test.hpp"

#include <future>

struct Tes{
	double v;
	double v2;
	Tes(double v_) : v(v_), v2(v_*2){}
	double operator+(double d) const{ return d + v; }
	double operator+=(double d) const{ return d += v; }
};

int main()
{
	setlocale(LC_ALL, "Japanese");

	std::array<int, 4> ar{ { 1, 2, 3, 4 } };

	auto e = ar.rend();
	std::cout << *ar.rbegin() << std::endl;
	std::cout << *(ar.rend()-3) << std::endl;

/*
	std::cout << container_traits<int>::exist << std::endl;
	std::cout << container_traits<std::vector<int>>::exist << std::endl;
	std::cout << container_traits<std::array<int,3>>::exist << std::endl;

	sig::TimeWatch tw;
	for (int i=0; i<100; ++i){
		tw.Stop();
		std::vector<Tes> vec(100000, 1);
		tw.ReStart();
		auto d = sig::Plus(2, vec);
		//for (auto& v : vec){ v += 2; }
		tw.Save();
	}

	std::cout << tw.GetTotalTime() << std::endl;


	std::vector<int> vec{1,2, 3, 4, 5, 6, 7};
	std::vector<std::vector<double>> result;

	const auto Task = [&t](int id){
		std::vector<double> r(id, 0);

		std::cout << "id:" << id << std::endl;

		return std::move(r);
	};

	std::vector<std::future< std::vector<double> >> task;

	tw.Save();
	for (auto const& v :vec){
		task.push_back(std::async(std::launch::async, Task, v));
	}

	tw.Save();
	for (auto& t : task){
		result.push_back(t.get());
	}
	tw.Save();

	std::cout << std::endl;
	std::cout << sig::FromJust(tw.GetLapTime(0)) << std::endl;
	std::cout << sig::FromJust(tw.GetLapTime(1)) << std::endl;
	std::cout << sig::FromJust(tw.GetLapTime(2)) << std::endl;
*/

	ArithmeticOperationsTest();

	//functional.hpp test
	MapTest();
	HigherOrederFuncTest();
	FunctionalTest();

	//string.hpp test
	RegexTest();
	TagDealerTest();
	SplitTest();
	CatStrTest();
	StrConvertTest();
	ZenHanTest();

	//file.hpp test
	GetDirectoryNamesTest();
	FileSaveLoadTest();

	//modify.hpp test
	SortTest();
	ShuffleTest();
	RemoveDuplicateTest();
	RemoveTest();

	//tool.hpp test
	RandomTest();
	TimeWatchTest();
	HistgramTest();
	PercentTest();
	ArrayTest();

	return 0;
}