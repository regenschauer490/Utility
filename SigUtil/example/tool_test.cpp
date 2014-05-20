#include "tool_test.h"
#include "../lib/helper.hpp"
#include "../lib/calculation.hpp"

#if SIG_MSVC_ENV
#include <windows.h>
#else
#include <unistd.h>
#endif

//SIG_ENABLE_BOOST = 1 の際にはboost::optionalが有効になる

void RandomTest()
{
	sig::SimpleRandom<int> rand_maker(-10, 10, true);	//[-10, 10]の一様分布乱数、デバッグモード(シード固定)
	
	std::cout << "make random number in [-10, 10]:" << rand_maker() << std::endl;

	auto rints = sig::random_unique_numbers(10, 0, 20, true);	//[0, 20]の一様分布から10個重複無くサンプル

	std::cout << "make 10 random numbers in [0, 20]" << std::endl;
	for (auto v : rints) std::cout << v << std::endl;
}

void TimeWatchTest()
{
	sig::TimeWatch tw;		//計測開始

#if SIG_MSVC_ENV
	auto tsleep = [](unsigned milisec){ Sleep(milisec); };
#else
	auto tsleep = [](unsigned milisec){ usleep(milisec*1000); };
#endif
	tsleep(100);

	tw.save();		//ここまでのタイムを記録

	tsleep(200);

	tw.save();		//ここまでのタイムを記録

	tsleep(300);

	tw.stop();		//一時停止

	tsleep(10);

	tw.save();		//ここまでのタイムを記録

	tsleep(20);

	tw.restart();	//計測再開

	tsleep(400);

	tw.save();		//ここまでのタイムを記録

	const unsigned moe = 10;	//環境毎のsleep時間誤差（ms）

	assert(sig::equal_tolerant(sig::fromJust(tw.get_lap_time(0)), 100, moe));		//100 ± moe (ms)
	assert(sig::equal_tolerant(sig::fromJust(tw.get_lap_time(1)), 200, moe));		//200 ± moe (ms)
	assert(sig::equal_tolerant(sig::fromJust(tw.get_lap_time(2)), 300, moe));		//300 ± moe (ms)
	assert(sig::equal_tolerant(sig::fromJust(tw.get_lap_time(3)), 400, moe));		//400 ± moe (ms)
#if SIG_ENABLE_BOOST && SIG_USE_OPTIONAL
	assert(! tw.get_lap_time(4));
#else
	assert(tw.get_lap_time(4) == -1);
#endif
	assert(sig::equal_tolerant(sig::fromJust(tw.get_split_time(0)), 100, moe));	//100 ± moe (ms)
	assert(sig::equal_tolerant(sig::fromJust(tw.get_split_time(1)), 300, moe));	//300 ± moe (ms)
	assert(sig::equal_tolerant(sig::fromJust(tw.get_split_time(2)), 600, moe));	//600 ± moe (ms)
	assert(sig::equal_tolerant(sig::fromJust(tw.get_split_time(3)), 1000, moe));	//1000 ± moe (ms)

	assert(sig::equal_tolerant(tw.get_total_time(), 1000, 4*moe));				//1000 ± 4*moe (ms)
	assert(sig::equal(tw.get_total_time<std::chrono::seconds>(), 1));		//1 (s)
	assert(sig::equal_tolerant(tw.get_total_time<std::chrono::microseconds>(), 1000000, 10000));	//1000000  ± 10000 (μs)
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

	hist.count(data1);

	hist.print();						//コンソールへ出力
	hist.print(file1);	//ファイルへ出力

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

	auto count = hist.get_count();		//0 ～ BIN_NUM-1 の頻度を取得
	assert(count[2] == 2);				//[ -6, -4)の個数

	auto c2 = hist.get_count(2);
	auto c100 = hist.get_count(100);

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

	bool over = hist.is_over_range();	//初期設定の範囲外の値が存在したか
	assert(over == 1);				//範囲外の値の個数：1
	

	sig::Histgram<double, 15> hist2(0, 1);						//double型、ビン数15、[0～1）の範囲の数値を集計
	auto rand_maker = sig::SimpleRandom<double>(0, 1.5, true);	//[0, 1.5]の一様分布乱数、デバッグモード(シード固定)
	
	for (int i = 0; i<100; ++i) hist2.count(rand_maker());

	hist2.print();
	hist2.print(file2);
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
	pc4 = sig::Percent::unit();	//copy assignment. pc3 == 100%

	assert(pc1.get_percent() == 100 && sig::equal(pc1.get_double(), 1));	//100% = 1
	assert(pc2.get_percent() == 50 && sig::equal(pc2.get_double(), 0.5));	//50% = 0.5
	assert(pc3.get_percent() == 1 && sig::equal(pc3.get_double(), 0.01));	//1% = 0.01
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
