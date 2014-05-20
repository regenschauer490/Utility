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

/// test completed
// Visual C++ Compiler Nov 2013 CTP (CTP_Nov2013)
// g++ (Ubuntu 4.8.1-2ubuntu1~12.04) 4.8.1
// Ubuntu clang version 3.4-1~exp1 (trunk) (based on LLVM 3.4)

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