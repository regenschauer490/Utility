#include "tool_test.h"
#include "../lib/helper.hpp"
#include "../lib/calculation.hpp"

#if SIG_MSVC_ENV
#include <windows.h>
#endif

//SIG_ENABLE_BOOST = 1 の際にはboost::optionalが有効になる

void RandomTest()
{
	sig::SimpleRandom<int> rand_maker(-10, 10, true);	//[-10, 10]の一様分布乱数、デバッグモード(シード固定)
	
	std::cout << rand_maker() << std::endl;

	auto rints = sig::RandomUniqueNumbers(10, 0, 30, true);	//[0, 30]の一様分布から10個重複無くサンプル

	for (auto v : rints) std::cout << v << std::endl;
}

void TimeWatchTest()
{
	sig::TimeWatch tw;		//計測開始

#if SIG_MSVC_ENV
	Sleep(100);

	tw.Save();		//ここまでのタイムを記録

	Sleep(200);

	tw.Save();		//ここまでのタイムを記録

	Sleep(300);

	tw.Stop();		//一時停止

	Sleep(10);

	tw.Save();		//ここまでのタイムを記録

	Sleep(20);

	tw.ReStart();	//計測再開

	Sleep(400);

	tw.Save();		//ここまでのタイムを記録

#if SIG_ENABLE_BOOST
	assert(sig::equal_tolerant(*tw.GetLapTime(0), 100, 1));		//100 ± 1 (ms)
	assert(sig::equal_tolerant(*tw.GetLapTime(1), 200, 1));		//200 ± 1 (ms)
	assert(sig::equal_tolerant(*tw.GetLapTime(2), 300, 1));		//300 ± 1 (ms)
	assert(sig::equal_tolerant(*tw.GetLapTime(3), 400, 1));		//400 ± 1 (ms)
	assert(! tw.GetLapTime(4));

	assert(sig::equal_tolerant(*tw.GetSplitTime(0), 100, 1));	//100 ± 1 (ms)
	assert(sig::equal_tolerant(*tw.GetSplitTime(1), 300, 1));	//300 ± 1 (ms)
	assert(sig::equal_tolerant(*tw.GetSplitTime(2), 600, 1));	//600 ± 1 (ms)
	assert(sig::equal_tolerant(*tw.GetSplitTime(3), 1000, 1));	//1000 ± 1 (ms)
#else
	assert(sig::equal_tolerant(tw.GetLapTime(0), 100, 1));		//100 ± 1 (ms)
	assert(sig::equal_tolerant(tw.GetLapTime(1), 200, 1));		//200 ± 1 (ms)
	assert(sig::equal_tolerant(tw.GetLapTime(2), 300, 1));		//300 ± 1 (ms)
	assert(sig::equal_tolerant(tw.GetLapTime(3), 400, 1));		//400 ± 1 (ms)
	assert(tw.GetLapTime(4) == -1);

	assert(sig::equal_tolerant(tw.GetSplitTime(0), 100, 1));		//100 ± 1 (ms)
	assert(sig::equal_tolerant(tw.GetSplitTime(1), 300, 1));		//300 ± 1 (ms)
	assert(sig::equal_tolerant(tw.GetSplitTime(2), 600, 1));		//600 ± 1 (ms)
	assert(sig::equal_tolerant(tw.GetSplitTime(3), 1000, 1));	//1000 ± 1 (ms)
#endif
	assert(sig::equal_tolerant(tw.GetTotalTime(), 1000, 10));				//1000 ± 10 (ms)
	assert(sig::equal(tw.GetTotalTime<std::chrono::seconds>(), 1));		//1 (s)
	auto total = tw.GetTotalTime<std::chrono::microseconds>();
	assert(sig::equal_tolerant(tw.GetTotalTime<std::chrono::microseconds>(), 1000000, 10000));	//1000000  ± 10000 (μs)
#else

#endif
}

void HistgramTest()
{
#if SIG_MSVC_ENV
	auto file1 = L"./example/test_file/histgram1.txt";
	auto file2 = L"./example/test_file/histgram2.txt";
#else
	auto file1 = "./example/test_file/histgram1.txt";
	auto file2 = "./example/test_file/histgram2.txt";
#endif

	std::vector<int> data1{-100, -10, -6, -5, -1, 0, 3, 5, 5, 6, 6, 6, 7, 9, 10};

	sig::Histgram<int, 10> hist(-10, 10);	//int型、ビン数10、[-10～10）の範囲の数値を集計

	hist.Count(data1);

	hist.Print();						//コンソールへ出力
	hist.Print(file1);	//ファイルへ出力

	/*
	-- Histgram --

	[-∞,-10)：0
	[-10, -8)：1 |
	[ -8, -6)：0
	[ -6, -4)：2 ||
	[ -4, -2)：0
	[ -2,  0)：1 |
	[  0,  2)：1 |
	[  2,  4)：1 |
	[  4,  6)：2 ||
	[  6,  8)：4 ||||
	[  8, 10)：1 |
	[ 10,+∞)：1 |
	*/

	auto count = hist.GetCount();		//0 ～ BIN_NUM-1 の頻度を取得
	assert(count[2] == 2);				//[ -6, -4)の個数

	auto c2 = hist.GetCount(2);
	auto c100 = hist.GetCount(100);

#if SIG_ENABLE_BOOST && SIG_USE_OPTIONAL
	if(c2){
		assert(std::get<1>(*c2) == -6);
		assert(std::get<2>(*c2) == -4);
		assert(std::get<0>(*c2) == 2);
	}
	assert(!c100);
#else
	assert(std::get<1>(c2) == -6);
	assert(std::get<2>(c2) == -4);
	assert(std::get<0>(c2) == 2);

	assert(std::get<1>(c100) == 0);
	assert(std::get<2>(c100) == 0);
	assert(std::get<0>(c100) == 0);
#endif

	bool over = hist.IsOverRange();	//初期設定の範囲外の値が存在したか
	assert(over == 1);				//範囲外の値の個数：1
	

	sig::Histgram<double, 15> hist2(0, 1);						//double型、ビン数15、[0～1）の範囲の数値を集計
	auto rand_maker = sig::SimpleRandom<double>(0, 1.5, true);	//[0, 1.5]の一様分布乱数、デバッグモード(シード固定)
	
	for (int i = 0; i<100; ++i) hist2.Count(rand_maker());

	hist2.Print();
	hist2.Print(file2);
	/*
	-- Histgram --

	[-∞   , 0.000)： 0
	[ 0.000, 0.067)： 6 ||||||
	[ 0.067, 0.133)： 6 ||||||
	[ 0.133, 0.200)： 5 |||||
	[ 0.200, 0.267)： 6 ||||||
	[ 0.267, 0.333)： 4 ||||
	[ 0.333, 0.400)： 5 |||||
	[ 0.400, 0.467)： 2 ||
	[ 0.467, 0.533)：11 |||||||||||
	[ 0.533, 0.600)： 2 ||
	[ 0.600, 0.667)： 3 |||
	[ 0.667, 0.733)： 5 |||||
	[ 0.733, 0.800)： 3 |||
	[ 0.800, 0.867)： 4 ||||
	[ 0.867, 0.933)： 7 |||||||
	[ 0.933, 1.000)： 1 |
	[ 1.000,+∞   )：30 ||||||||||||||||||||||||||||||
	*/
}

void PercentTest()
{
	sig::Percent pc1(100);
	sig::Percent pc2(50);
	sig::Percent pc3(0);
	sig::Percent pc4(0);

	pc3 = 1;					//copy assignment. pc4 == 1%
	pc4 = sig::Percent::Unit();	//copy assignment. pc3 == 100%

	assert(pc1.GetPercent() == 100 && pc1.GetDouble() == 1);	//100% = 1
	assert(pc2.GetPercent() == 50 && pc2.GetDouble() == 0.5);	//50% = 0.5
	assert(pc3.GetPercent() == 1 && pc3.GetDouble() == 0.01);	//1% = 0.01
	assert(pc1 == pc4);
}

void ArrayTest()
{
	sig::array<int, 4> ar1{ 1, 2, 3 };

	std::array<double, 2> std_ar{ {1.1, 2.2} };
	sig::array<double, 2> ar2(std_ar);

	ar2.pop_back();
	ar2.push_back(3.3);

	ar1.insert(ar1.begin()+1, 4);
	ar1.erase(ar1.begin());

	auto plus = sig::plus(ar1, std::initializer_list<int>{1, 2, 3, 4});

	for (auto v : plus) std::cout << v << std::endl;
	for (auto it = ar1.rbegin(), end = ar1.rend(); it != end; ++it) std::cout << *it << std::endl;

	ar1.swap(plus);

	sig::array<double, 2> ar3;
	assert(ar3.size() == 0);

	ar3 = ar2;		//copy

}