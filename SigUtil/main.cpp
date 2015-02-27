#include "example/container_specialize_test.h"
#include "example/helper_test.h"
#include "example/maybe_test.h"
#include "example/array_test.h"
#include "example/string_test.h"
#include "example/file_test.h"
#include "example/tool_test.h"
#include "example/modify_test.h"
#include "example/functional_test.h"
#include "example/calculate_test.h"
#include "example/distance_test.h"
#include "example/ublas_test.h"
#include "example/performance_test.h"

/// test completed compilers
// Visual C++ Compiler Nov 2013 CTP (CTP_Nov2013)
// Visual C++ Compiler 2014 (v140)
// Visual C++ Compiler 2015 Preview (v140)
// g++ 4.8
// g++ 4.9
// clang 3.4 (based on LLVM 3.4)


int main()
{
	setlocale(LC_ALL, "Japanese");

	//performance test
	//SplitPerformanceTest();
	//OptionalPerformanceTest();
	//ContainerTraitsEffectiveTest();

	//container_traits.hpp test (利用可能コンテナの拡張)
	ContainerSpecializeTest();

	//helper.hpp test
	TestEval();
	TestTypeTraits();
	TestHelperModules();
	MaybeTest();

	//array.hpp test
	ArrayTest();

	//calculation.hpp test
	ArithmeticOperationsTest();
	StatisticalOperationTest();
	CompoundAssignmentTest();
	ForeachTest();


	//functional.hpp test
	MapTest();
	ZipWithTest();
	FunctionalTest();

	//string.hpp test
	RegexTest();
	SplitTest();
	CatStrTest();
	StrConvertTest();
	ZenHanTest();

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
	TagDealerTest();

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

	//file.hpp test
	GetDirectoryNamesTest();
	FileSaveLoadTest();

	return 0;
}
