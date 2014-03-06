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

int main()
{
	setlocale(LC_ALL, "Japanese");

	std::vector<int> v {1,2,3};
	std::array<double, 3> ar{ { 1.1, 2.2, 3.3 }};
	auto t = sig::ZipWith([](double v1, double v2){ return v1*v2; }, v, ar);

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