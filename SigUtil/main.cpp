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

	sig::TimeWatch tw;
	auto vec = sig::Replicate<std::wstring, std::multiset<std::wstring>>(10, std::wstring(5000000, L'あ'));
	tw.Save();
	auto vec2 = vec;
	tw.Save();
	auto tmp = sig::WSTRtoSTR(vec);
	tw.Save();

//	std::cout << sig::FromJust(tw.GetLapTime(0)) << std::endl;
//	std::cout << sig::FromJust(tw.GetLapTime(1)) << std::endl;
	std::cout << sig::FromJust(tw.GetLapTime(2)) << std::endl;

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