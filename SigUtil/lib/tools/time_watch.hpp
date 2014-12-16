/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_TIMEWATCH_HPP
#define SIG_UTIL_TIMEWATCH_HPP

#include "../sigutil.hpp"
#include "../helper/maybe.hpp"
#include <chrono>


/// \file time_watch.hpp 高機能タイム(ストップ)ウォッチ (ラップ、スプリット、一時停止、複数保存)

namespace sig
{
/// タイムウォッチ
/**
	\template Clock 時間の精度（defaultはstd::chrono::system_clock. 高精度はstd::chrono::high_resolution_clock）

	\code
	TimeWatch tw;		// 計測開始

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

	assert( equal_tolerant(*tw.get_lap_time(0)), 100, moe);		//100 ± moe (ms)
	assert( equal_tolerant(*tw.get_lap_time(1)), 200, moe);		//200 ± moe (ms)
	assert( equal_tolerant(*tw.get_lap_time(2)), 300, moe);		//300 ± moe (ms)
	assert( equal_tolerant(*tw.get_lap_time(3)), 400, moe);		//400 ± moe (ms)

	assert(! sig::isJust(tw.get_lap_time(4)));	// no data

	assert( equal_tolerant(*tw.get_split_time(0)), 100, moe);	//100 ± moe (ms)
	assert( equal_tolerant(*tw.get_split_time(1)), 300, moe);	//300 ± moe (ms)
	assert( equal_tolerant(*tw.get_split_time(2)), 600, moe);	//600 ± moe (ms)
	assert( equal_tolerant(*tw.get_split_time(3)), 1000, moe);	//1000 ± moe (ms)

	assert( equal_tolerant( tw.get_total_time(), 1000, 4 * moe));				//1000 ± 4 * moe (ms)
	assert( equal( tw.get_total_time<std::chrono::seconds>(), 1));		//1 (s)
	assert( equal_tolerant( tw.get_total_time<std::chrono::microseconds>(), 1000000, 10000));	//1000000  ± 10000 (μs)
	\endcode
*/
template <class Clock = std::chrono::system_clock>
class TimeWatch
{
	using Time = typename Clock::time_point;
	using Duration = decltype(std::declval<Time>() - std::declval<Time>());

	Time st;
	std::vector<Duration> laps;
	std::vector<Duration> cache;
	bool is_run;

private:
	void init()
	{
		st = Clock::now();
		laps.clear();
		cache.clear();
	}

	Duration d_accumulate(std::vector<Duration> const& ds, uint end) const
	{
		return std::accumulate(ds.begin(), ds.begin() + end, Duration(), [&](Duration const& sum, Duration const& v){
			return sum + v;
		});
	}

public:
	TimeWatch()
	{
		init();
		is_run = true;
	}

	/// 初期化して停止
	void reset()
	{
		init();
		is_run = false;
	}

	/// 停止
	void stop()
	{
		if (is_run)	cache.push_back(Clock::now() - st);
		is_run = false;
	}
		
	/// 停止解除
	void restart()
	{
		st = Clock::now();
		is_run = true;
	}

	/// 時間保存
	void save()
	{
		if (is_run){
			auto now = Clock::now();
			cache.push_back(now - st);
			st = std::move(now);
		}

		laps.push_back(d_accumulate(cache, cache.size()));
		cache.clear();
	}


	/// 記録したデータ数を取得
	uint get_count() const{ return laps.size(); }


	///　全区間のトータルの時間を取得
	/**
		\tparam TimeUnit 取得する時間の単位を指定
	*/
	template<class TimeUnit = std::chrono::milliseconds>
	long get_total_time()
	{
		return std::chrono::duration_cast<TimeUnit>(d_accumulate(laps, laps.size())).count();
	}

	///　指定した区間までのトータル時間(スプリットタイム)を取得
	/**
		\tparam TimeUnit 取得する時間の単位を指定
	*/
	template<class TimeUnit = std::chrono::milliseconds>
	auto get_split_time(uint index) ->Maybe<long>
	{
		return index < laps.size()
			? Just<long>(std::chrono::duration_cast<TimeUnit>(d_accumulate(laps, index+1)).count())
			: Nothing(-1l);
	}

	///　指定した区間の時間(ラップタイム)を取得
	/**
		\tparam TimeUnit 取得する時間の単位を指定
	*/
	template<class TimeUnit = std::chrono::milliseconds>
	auto get_lap_time(uint index) ->Maybe<long>
	{
		return index < laps.size()
			? Just<long>(std::chrono::duration_cast<TimeUnit>(laps[index]).count())
			: Nothing(-1l);
	}
};

}

#endif