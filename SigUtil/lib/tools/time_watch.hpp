/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_TIMEWATCH_HPP
#define SIG_UTIL_TIMEWATCH_HPP

#include "../sigutil.hpp"
#include <chrono>


/* 高機能タイム(ストップ)ウォッチ (ラップ、スプリット、一時停止、複数保存) */

namespace sig
{
// タイムウォッチ
class TimeWatch
{
	typedef std::chrono::system_clock::time_point Time;
	typedef decltype(std::declval<Time>() - std::declval<Time>()) Duration;

	Time st;
	std::vector<Duration> laps;
	std::vector<Duration> cache;
	bool is_run;

private:
	void init()
	{
		st = std::chrono::system_clock::now();
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

	// 初期化して停止
	void reset()
	{
		init();
		is_run = false;
	}

	// 停止
	void stop()
	{
		if (is_run)	cache.push_back(std::chrono::system_clock::now() - st);
		is_run = false;
	}
		
	// 停止解除
	void restart()
	{
		st = std::chrono::system_clock::now();
		is_run = true;
	}

	void save()
	{
		if (is_run){
			auto now = std::chrono::system_clock::now();
			cache.push_back(now - st);
			st = std::move(now);
		}

		laps.push_back(d_accumulate(cache, cache.size()));
		cache.clear();
	}

	//　トータルの時間を取得
	//　template引数で時間の単位を指定
	template<class TimeUnit = std::chrono::milliseconds>
	long get_total_time()
	{
		return std::chrono::duration_cast<TimeUnit>(d_accumulate(laps, laps.size())).count();
	}

	//　指定した区間までのトータル時間(スプリットタイム)を取得
	//　template引数で時間の単位を指定
	template<class TimeUnit = std::chrono::milliseconds>
	auto get_split_time(uint index) ->Just<long>
	{
		return index < laps.size()
			? Just<long>(std::chrono::duration_cast<TimeUnit>(d_accumulate(laps, index+1)).count())
			: Nothing(-1);
	}

	//　指定した区間の時間(ラップタイム)を取得
	//　template引数で時間の単位を指定
	template<class TimeUnit = std::chrono::milliseconds>
	auto get_lap_time(uint index) ->Just<long>
	{
		return index < laps.size() ? Just<long>(std::chrono::duration_cast<TimeUnit>(laps[index]).count()) : Nothing(-1);
	}
};

}

#endif