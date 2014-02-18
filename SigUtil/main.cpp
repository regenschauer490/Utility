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
	decltype(std::declval<std::vector<int>::iterator>().operator<(std::declval<std::vector<int>::iterator>())) a;
	
	auto v1 = sig::Equal(1u, 1.000);
	auto v2 = sig::Equal(1u, 1.001);
	auto v3 = sig::Equal(1u, 1);

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