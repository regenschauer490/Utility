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

int main()
{
	setlocale(LC_ALL, "Japanese");

/*	sig::TimeWatch tw;
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

	return 0;
}