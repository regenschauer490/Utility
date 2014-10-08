#include "lib/string.hpp"
#include "lib/file.hpp"
#include "lib/tools.hpp"
#include "lib/modify.hpp"
#include "lib/functional.hpp"

#include "example/container_specialize_test.h"
#include "example/helper_test.h"
#include "example/maybe_test.h"
#include "example/string_test.h"
#include "example/file_test.h"
#include "example/tool_test.h"
#include "example/modify_test.h"
#include "example/functional_test.h"
#include "example/calculate_test.h"
#include "example/iteration_test.h"
#include "example/distance_test.h"
#include "example/ublas_test.h"
#include "example/convergence_test.h"
#include "example/performance_test.h"

/// test completed
// Visual C++ Compiler Nov 2013 CTP (CTP_Nov2013)
// g++ (Ubuntu 4.8.1-2ubuntu1~12.04) 4.8.1
// Ubuntu clang version 3.4-1~exp1 (trunk) (based on LLVM 3.4)


int main()
{
	setlocale(LC_ALL, "Japanese");

	//performance test
	//SplitPerformanceTest();
	//OptionalPerformanceTest();

	//container_traits.hpp test (利用可能コンテナの拡張)
	ContainerSpecializeTest();

	//helper.hpp test
	TestEval();
	TestHelperModules();
	MaybeTest();

	//array.hpp test
	ArrayTest();

	//calculation.hpp test
	ArithmeticOperationsTest();
	StatisticalOperationTest();

	//iteration.hpp test
	ForeachTest();
	FoldZipWithTest();
	CompoundAssignmentTest();

	//functional.hpp test
	MapTest();
	ZipWithTest();
	FunctionalTest();

	//string.hpp test
	RegexTest();
	SplitTest();
	TagDealerTest();
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
//	TimeWatchTest();	//テスト結果は処理環境のスペック依存

	//distance.hpp
	NormTest(); 
	MinkowskiDistanceTest();
	CosineSimilarityTest();
	CanberraDistanceTest();
	BinaryDistanceTest();
	KL_DivergenceTest();
	JS_DivergenceTest();

	//ublas.hpp
	UblasConvertTest();
	UblasCalculation();

	//convergence.hpp
	ConvergenceTest();

	return 0;
}
