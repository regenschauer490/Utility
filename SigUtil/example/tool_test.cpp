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

	tw.Stop();		//ここまでのラップタイムを記録し、一時停止

	Sleep(300);

	tw.ReStart();	//計測再開

	Sleep(500);

	tw.Stop();

	tw.ReStart();

	Sleep(1000);

	tw.Stop();

#if SIG_ENABLE_BOOST
	std::cout << *tw.GetLapTime(0) << std::endl;	//100
	std::cout << *tw.GetLapTime(1) << std::endl;	//500
	std::cout << *tw.GetLapTime(2) << std::endl;	//1000
	assert(!tw.GetLapTime(3));

	std::cout << *tw.GetSplitTime(0) << std::endl;	//100
	std::cout << *tw.GetSplitTime(1) << std::endl;	//600
	std::cout << *tw.GetSplitTime(2) << std::endl;	//1600

	std::cout << *tw.GetTotalTime() << std::endl;	//1600
	std::cout << *tw.GetTotalTime<std::chrono::seconds>() << std::endl;			//1
	std::cout << *tw.GetTotalTime<std::chrono::microseconds>() << std::endl;	//1600090 ※環境による誤差
#else
	std::cout << tw.GetLapTime(0) << std::endl;		//100
	std::cout << tw.GetLapTime(1) << std::endl;		//500
	std::cout << tw.GetLapTime(2) << std::endl;		//1000
	
	std::cout << tw.GetSplitTime(0) << std::endl;	//100
	std::cout << tw.GetSplitTime(1) << std::endl;	//600
	std::cout << tw.GetSplitTime(2) << std::endl;	//1600

	std::cout << tw.GetTotalTime() << std::endl;	//1600
	std::cout << tw.GetTotalTime<std::chrono::seconds>() << std::endl;			//1
	std::cout << tw.GetTotalTime<std::chrono::microseconds>() << std::endl;	//1600090 ※環境による誤差
#endif

#else
#endif

}