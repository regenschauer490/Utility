/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_HISTGRAM_HPP
#define SIG_UTIL_HISTGRAM_HPP

#include "../sigutil.hpp"
#include <fstream>
#include <array>


/// \file histgram.hpp ヒストグラム（データ型指定、ビン数指定、値域指定、出力・保存機能）

namespace sig
{

/// ヒストグラム
/**
	\tparam T 要素の型
	\tparam Bin 度数

	\code
	std::vector<int> data{-100, -10, -6, -5, -1, 0, 3, 5, 5, 6, 6, 6, 7, 9, 10};

	sig::Histgram<int, 10> hist(-10, 10);	//int型、ビン数10、[-10～10）の範囲の数値を集計

	hist.count(data);
	hist.print();
	\endcode

	\code
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
	\endcode
*/
template <class T, size_t Bin>
class Histgram
{
	T const min_;
	T const max_;
	double const delta_;
	std::array<uint, Bin + 2> count_;	//[0]: x < min, [Bin-1]: max <= x
	uint num_;

private:
	void print_base_(std::ostream& ofs) const
	{
		//auto IsPlus = [](double v){ return v < 0 ? false : true; };

		auto IntDigit = [](double v){ return static_cast<int>(std::log10(v) + 1); };

		auto FirstZero = [](double v){
			uint keta = 0;
			if (equal(v, 0)) return keta;
			while (static_cast<int>(v * std::pow(10, keta)) == 0) ++keta;
			return keta;
		};

		auto Space = [](int num){
			std::string space;
			for (int i = 0; i < num; ++i) space.append(" ");
			return space;
		};

		auto Precision = [](double v)
		{
			uint keta = 0;
			double dv = v - int(v);

			while (true){
				if (equal(dv, 0) || keta >= 15) break;
				v *= 10;
				dv = v - int(v);
				++keta;
			}

			return keta;
		};

		int const rketa = IntDigit(max_);
		int const disp_precision = typename std::conditional<std::is_integral<T>::value, std::true_type, std::false_type>::type{}
			? 0
			: IntDigit(delta_) > 1
				? 0
				: FirstZero(delta_) +1;
		int const keta = std::max(rketa, std::min((int) Precision(delta_), disp_precision) + 2);
		int const ctketa = log10(*std::max_element(count_.begin(), count_.end())) + 1;
		T const dbar = num_ < 100 ? 1.0 : num_*0.01;

		/*
		std::string offset1, offset2;
		if (keta < 3) offset1.append(2 - keta, ' ');
		else offset2.append(keta - 3, ' ');*/

		ofs << "\n-- Histgram --\n";
		for (int i = 0; i < Bin + 2; ++i){
			auto low = delta_*(i - 1) + min_;
			auto high = delta_*i + min_;

			if (i == 0) ofs << std::fixed << std::setprecision(disp_precision) << "\n[-∞" << Space(keta - 2) << "," << std::setw(keta + 1) << high << ")" << "：" << std::setw(ctketa) << count_[i] << " ";
			else if (i == Bin + 1) ofs << std::fixed << std::setprecision(disp_precision) << "\n[" << std::setw(keta + 1) << low << ",+∞" << Space(keta - 2) << ")" << "：" << std::setw(ctketa) << count_[i] << " ";
			else ofs << std::fixed << std::setprecision(disp_precision) << "\n[" << std::setw(keta + 1) << low << "," << std::setw(keta + 1) << high << ")" << "：" << std::setw(ctketa) << count_[i] << " ";

			for (uint j = 1; dbar*j <= count_[i]; ++j) ofs << "|";
		}
		ofs << std::resetiosflags(std::ios_base::floatfield) << "\n\n";
	}

public:
	/// コンストラクタ 
	/**
		\param min ヒストグラムの範囲の最小値
		\param max ヒストグラムの範囲の最大値
	*/
	Histgram(T min, T max) : min_(min), max_(max), delta_(((double) max - min) / Bin), num_(0)
	{
		if (!(delta_ > 0)){
			std::cout << "error: max - min = " << delta_ << std::endl;
			std::terminate();
		}
		for (auto& ct : count_) ct = 0;
	}

	/// 要素をbinに振り分けてカウント
	void count(T value)
	{
		for (uint i = 0; i < Bin + 1; ++i){
			if (value < delta_*i + min_){
				++num_;
				++count_[i];
				return;
			}
		}
		++count_[Bin + 1];
	}

	template <class C, typename std::enable_if<impl::container_traits<C>::exist>::type*& = enabler>
	void count(C const& values){
		for (auto const& e : values) count(e);
	}

	/// bin外の要素が存在したか
	bool is_over_range() const{	return count_[0] || count_[Bin + 1]; }

	//double GetAverage() const{ return std::accumulate(count_.begin(), count_.end(), 0, [](T total, T next){ return total + next; }) / static_cast<double>(num_); }

	/// 頻度を取得
	auto get_count() const -> std::array<uint, Bin>
	{
		std::array<uint, Bin> tmp;
		for (uint i = 0; i < Bin; ++i) tmp[i] = count_[i + 1];
		return tmp;
	}

	/// bin番目(0 ～ Bin-1)の頻度を取得
	/**
		\param bin ビン番号

		\return tuple<頻度, 範囲最小値(以上), 範囲最大値(未満)>
	*/
	auto get_count(uint bin) const ->Maybe<std::tuple<uint, int, int>>
	{
		return bin < Bin
			? Just<std::tuple<uint, int, int>>(std::make_tuple(count_[bin + 1], delta_*bin + min_, delta_*(bin + 1) + min_))
			: Nothing(std::make_tuple(static_cast<uint>(0u), 0, 0));
	}
		
	/// 標準出力へ出力
	void print() const{ print_base_(std::cout); }

	/// ファイルへ出力
	void print(FilepassString const& file_pass) const
	{
		std::ofstream ofs(file_pass);
		print_base_(ofs);
	}
};

}
#endif