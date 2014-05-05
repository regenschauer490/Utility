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

	//helper.hpp test
	TestHelperModules();

	//calculation.hpp test
	ArithmeticOperationsTest();

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
	TimeWatchTest();
	HistgramTest();
	PercentTest();
	ArrayTest();

	return 0;
}