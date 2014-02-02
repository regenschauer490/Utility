#include "tool_test.h"

#if SIG_WINDOWS_ENV
#include <windows.h>
#endif

//SIG_ENABLE_BOOST = 1 の際にはboost::optionalが有効になる

void TimeWatchTest()
{
	sig::TimeWatch tw;		//計測開始

#if SIG_WINDOWS_ENV
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
	std::cout << *tw.GetLapTime(0) << std::endl;	//100
	std::cout << *tw.GetLapTime(1) << std::endl;	//200
	std::cout << *tw.GetLapTime(2) << std::endl;	//300
	std::cout << *tw.GetLapTime(3) << std::endl;	//400
	assert(!tw.GetLapTime(4));

	std::cout << *tw.GetSplitTime(0) << std::endl;	//100
	std::cout << *tw.GetSplitTime(1) << std::endl;	//300
	std::cout << *tw.GetSplitTime(2) << std::endl;	//600
	std::cout << *tw.GetSplitTime(3) << std::endl;	//1000

	std::cout << tw.GetTotalTime() << std::endl;	//1000
	std::cout << tw.GetTotalTime<std::chrono::seconds>() << std::endl;			//1
	std::cout << tw.GetTotalTime<std::chrono::microseconds>() << std::endl;	//10000037 ※環境による誤差
#else
	std::cout << tw.GetLapTime(0) << std::endl;		//100
	std::cout << tw.GetLapTime(1) << std::endl;		//200
	std::cout << tw.GetLapTime(2) << std::endl;		//300
	std::cout << tw.GetLapTime(3) << std::endl;		//400
	
	std::cout << tw.GetSplitTime(0) << std::endl;	//100
	std::cout << tw.GetSplitTime(1) << std::endl;	//300
	std::cout << tw.GetSplitTime(2) << std::endl;	//600
	std::cout << tw.GetSplitTime(3) << std::endl;	//1000

	std::cout << tw.GetTotalTime() << std::endl;	//1000
	std::cout << tw.GetTotalTime<std::chrono::seconds>() << std::endl;			//1
	std::cout << tw.GetTotalTime<std::chrono::microseconds>() << std::endl;	//10000037 ※環境による誤差
#endif

#else
#endif

}

void HistgramTest()
{
	std::vector<int> data1{-100, -10, -6, -5, -1, 0, 3, 5, 5, 6, 6, 6, 7, 9, 10};

	sig::Histgram<int, 10> hist(-10, 10);	//int型、ビン数10、[-10～10）の範囲の数値を集計

	hist.Count(data1);

	hist.Print();					//コンソールへ出力
	hist.Print(L"./example/test_file/histgram1.txt");	//ファイルへ出力
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

	auto count = hist.GetCount();			//0 ～ BIN_NUM-1 の頻度を取得
	std::cout << count[2] << std::endl;		//2　//[ -6, -4)の個数

	auto c2 = hist.GetCount(2);
	auto c100 = hist.GetCount(100);

#if SIG_ENABLE_BOOST
	if(c2) std::cout << "[" << std::get<1>(*c2) << ", " << std::get<2>(*c2) << "): " << std::get<0>(*c2) << std::endl; //[-6, -4): 2
	assert(!c100);
#else
	std::cout << "[" << std::get<1>(c2) << ", " << std::get<2>(c2) << "): " << std::get<0>(c2) << std::endl;		//[-6, -4): 2
	std::cout << "[" << std::get<1>(c100) << ", " << std::get<2>(c100) << "): " << std::get<0>(c100) << std::endl;	//[0, 0): 0
#endif

	bool over = hist.IsOverRange();		//初期設定の範囲外の値が存在したか
	std::cout << "範囲外の値の個数：" << over << std::endl;		//範囲外の値の個数：1
	

	sig::Histgram<double, 15> hist2(0, 1);						//double型、ビン数15、[0～1）の範囲の数値を集計
	auto rand_maker = sig::SimpleRandom<double>(0, 1.5, true);	//[0, 1.5]の一様分布乱数、デバッグモード(シード固定)
	
	for (int i = 0; i<100; ++i) hist2.Count(rand_maker());

	hist2.Print();
	hist2.Print(L"./example/test_file/histgram2.txt");
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