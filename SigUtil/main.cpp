#include "lib/string.hpp"
#include "lib/file.hpp"
#include "lib/tool.hpp"
#include "lib/modify.hpp"
#include "lib/functional.hpp"

#include "example/helper_test.h"
#include "example/string_test.h"
#include "example/file_test.h"
#include "example/tool_test.h"
#include "example/modify_test.h"
#include "example/functional_test.h"
#include "example/calculate_test.h"
#include "example/iteration_test.h"


int main()
{
	setlocale(LC_ALL, "Japanese");

	//StrConvertPerformanceTest();

	//helper.hpp test
	TestHelperModules();

	//array.hpp test
	ArrayTest();

	//calculation.hpp test
	ArithmeticOperationsTest();

	//
	ForeachTest();
	CompoundAssignmentTest();

	//functional.hpp test
	MapTest();
	ZipWithTest();
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
	HistgramTest();
	PercentTest();
//	TimeWatchTest();	//処理環境のスペック依存

	return 0;
}
