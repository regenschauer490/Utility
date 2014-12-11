#include "performance_test.h"
#include "../lib/tools.hpp"
#include "../lib/file.hpp"
#include "../lib/string.hpp"
#include "../lib/distance/cosine_similarity.hpp"

// split関数の処理時間の確認
void SplitPerformanceTest()
{
	auto long_text = sig::fromJust(sig::load_line<std::string>(SIG_TO_FPSTR("../SigUtil/example/test_file/long_text.txt")));

	sig::TimeWatch<> tw;
	auto split = sig::split(long_text[0], ",");
	tw.save();
	std::cout << "split time(ms): " << tw.get_total_time() << std::endl;
}

// optionalの有無による、処理時間の違いの確認
void OptionalPerformanceTest()
{
	const int ITERATION = 10;

	/*
	// 文字コード変換タスク
	const int N = 1000;
	const auto str = "test string";

	sig::TimeWatch<> tw1;
	for (int loop = 0; loop<ITERATION; ++loop){
		sig::array<std::wstring, N> ar;

		for (int i = 0; i<N; ++i){
			ar.push_back(sig::str_to_wstr(str));
		}

		tw1.save();
	}

	std::cout << "str_to_wstr time: " << tw1.get_total_time<std::chrono::milliseconds>() << std::endl;
	*/
	/*
	<VC++120>
	[debug]
	raw		952, 977, 916 ms
	maybe	970, 940,  ms

	[release]
	raw		813, 827, 828 ms
	maybe	845, 870, 811 ms
	*/


	// 距離計算タスク
	const int M = 10000;
	const int DIM = 10;
	std::vector<double> dot1, dot2;
	sig::SimpleRandom<double> drand(-1.0, 1.0, false);

	for (int i=0; i<DIM; ++i){
		dot1.push_back(drand());
		dot2.push_back(drand());
	}

	sig::TimeWatch<std::chrono::high_resolution_clock> tw2;
	for (int loop = 0; loop < ITERATION; ++loop){
		sig::array<double, M> ar;

		for (int i = 0; i < M; ++i){
			ar[i] = sig::cosine_similarity(dot1, dot2);
		}
		tw2.save();
	}

	std::cout << "cosine_simirality time: " << tw2.get_total_time<std::chrono::milliseconds>() << std::endl;

	/*
	<VC++120>
	[debug]
	raw		2081, 1976, 1996 ms
	maybe	2077, 2110, 2071 ms

	[release]
	raw		95, 102, 96 ms
	maybe	97, 107, 98 ms
	*/
}

void ContainerTraitsEffectiveTest()
{
	using Vec = std::vector<double>;
	using US = std::unordered_set<double>;
	using UM = std::unordered_map<double, double>;

	const int L = 100;
	const int N = 50000;
	const int H = N;

#if !(SIG_ENABLE_BOOST && SIG_USE_OPTIONAL)
	using sig::operator*;
#endif

	Sleep(1000);

// vector
	{
		sig::TimeWatch<std::chrono::high_resolution_clock> tw1;
		for (int l = 0; l<L; ++l){
			Vec vec1;
			for (int i = 0; i < N; ++i){
				vec1.push_back(i);
			}
		}
		tw1.stop();
		tw1.save();

		sig::TimeWatch<std::chrono::high_resolution_clock> tw2;
		for (int l = 0; l<L; ++l){
			auto vec2 = sig::impl::container_traits<Vec>::make(N);
			for (int i = 0; i < N; ++i){
				vec2.push_back(i);
			}
		}
		tw2.stop();
		tw2.save();

		std::cout << "default vec time: " << tw1.get_total_time<std::chrono::microseconds>() / L << std::endl;
		std::cout << "reserve vec time: " << tw2.get_total_time<std::chrono::microseconds>() / L << std::endl;
	}

	Sleep(1000);

//u-set
	{
		sig::TimeWatch<std::chrono::high_resolution_clock> tw1;
		tw1.stop();
		sig::TimeWatch<std::chrono::high_resolution_clock> tw2;
		tw2.stop();

		for (int l=0; l<L; ++l){
			US us1;
			tw1.restart();
			for (int i = 0; i < N; ++i){
				us1.emplace(i);
			}
			tw1.stop();
			tw1.save();
			us1.clear();

			US us2 = sig::impl::container_traits<US>::make(H);
			tw2.restart();
			for (int i = 0; i < N; ++i){
				us2.emplace(i);
			}
			tw2.stop();
			tw2.save();
		}

		std::cout << "default u-set write time: " << tw1.get_total_time<std::chrono::milliseconds>() / L << std::endl;
		std::cout << "reserve u-set write time: " << tw2.get_total_time<std::chrono::milliseconds>() / L << std::endl;
		
		sig::Histgram<long, 20> hist1(0, tw1.get_total_time<std::chrono::microseconds>() * 2 / L);
		for (int i = 0; i<tw1.get_count(); ++i) hist1.count(*tw1.get_lap_time<std::chrono::microseconds>(i));
		hist1.print();

		sig::Histgram<long, 20> hist2(0, tw2.get_total_time<std::chrono::microseconds>() * 2 / L);
		for (int i = 0; i<tw2.get_count(); ++i) hist2.count(*tw2.get_lap_time<std::chrono::microseconds>(i));
		hist2.print();
	}

	Sleep(1000);

	{
		US us1;
		US us2 = sig::impl::container_traits<US>::make(H);
		for (int i = 0; i < N; ++i){
			us1.emplace(i);
			us2.emplace(i);
		}

		sig::TimeWatch<std::chrono::high_resolution_clock> tw1;
		tw1.stop();
		sig::TimeWatch<std::chrono::high_resolution_clock> tw2;
		tw2.stop();

		for (int l = 0; l<L; ++l){
			double sum1 = 0;
			double sum2 = 0;

			tw1.restart();
			for (int i = 0; i < N; ++i){
				sum1 += *us1.find(i);
			}
			tw1.stop();
			tw1.save();

			tw2.restart();
			for (int i = 0; i < N; ++i){
				sum2 += *us2.find(i);
			}
			tw2.stop();
			tw2.save();
		}

		std::cout << "default u-set read time: " << tw1.get_total_time<std::chrono::microseconds>() / L << std::endl;
		std::cout << "reserve u-set read time: " << tw2.get_total_time<std::chrono::microseconds>() / L << std::endl;


		sig::Histgram<long, 20> hist1(0, tw1.get_total_time<std::chrono::nanoseconds>() * 2 / L);
		for (int i = 0; i<tw1.get_count(); ++i) hist1.count(*tw1.get_lap_time<std::chrono::nanoseconds>(i));
		hist1.print();

		sig::Histgram<long, 20> hist2(0, tw2.get_total_time<std::chrono::nanoseconds>() * 2 / L);
		for (int i = 0; i<tw2.get_count(); ++i) hist2.count(*tw2.get_lap_time<std::chrono::nanoseconds>(i));
		hist2.print();
	}

	Sleep(1000);

// u-map
	{
		sig::TimeWatch<std::chrono::high_resolution_clock> tw1;
		for (int l = 0; l<L; ++l){
			UM um1;
			tw1.restart();
			for (int i = 0; i < N; ++i){
				um1.emplace(i, i);
			}
			tw1.stop();
			tw1.save();
		}
		tw1.stop();
		tw1.save();

		sig::TimeWatch<std::chrono::high_resolution_clock> tw2;
		for (int l = 0; l<L; ++l){
			auto um2 = sig::impl::container_traits<UM>::make(H);
			tw2.restart();
			for (int i = 0; i < N; ++i){
				um2.emplace(i, i);
			}
			tw2.stop();
			tw2.save();
		}
		tw2.stop();
		tw2.save();

		std::cout << "default u-map write time: " << tw1.get_total_time<std::chrono::milliseconds>() / L << std::endl;
		std::cout << "reserve u-map write time: " << tw2.get_total_time<std::chrono::milliseconds>() / L << std::endl;
	}

	Sleep(1000);

	{
		UM um1;
		auto um2 = sig::impl::container_traits<UM>::make(H);
		for (int i = 0; i < N; ++i){
			um1.emplace(i, i);
			um2.emplace(i, i);
		}

		double sum1 = 0;
		sig::TimeWatch<std::chrono::high_resolution_clock> tw1;
		for (int l = 0; l<L; ++l){
			for (int i = 0; i < N; ++i){
				sum1 += um1.find(i)->second;
			}
		}
		tw1.stop();
		tw1.save();


		double sum2 = 0;
		sig::TimeWatch<std::chrono::high_resolution_clock> tw2;
		for (int l = 0; l<L; ++l){
			for (int i = 0; i < N; ++i){
				sum2 += um2.find(i)->second;
			}
		}
		tw2.stop();
		tw2.save();

		std::cout << "default u-map read time: " << tw1.get_total_time<std::chrono::microseconds>() / L << std::endl;
		std::cout << "reserve u-map read time: " << tw2.get_total_time<std::chrono::microseconds>() / L << std::endl;
	}
}
