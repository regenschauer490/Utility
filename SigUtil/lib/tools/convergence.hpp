/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_ERROR_CONVERGENCE_HPP
#define SIG_UTIL_ERROR_CONVERGENCE_HPP

#include "../sigutil.hpp"
#include "../distance/norm.hpp"
#include "../helper/maybe.hpp"
#include "../helper/helper_modules.hpp"

/// \file convergence.hpp 反復処理の収束判定に関するユーティリティ

namespace sig
{
/// 誤差基準（絶対誤差）
/**
	ManageConvergenceクラスのテンプレート引数に指定する関数オブジェクト
*/
struct AbsoluteError
{
	template <class T, class F>
	double operator()(T const& value, T const& last_value, F const& norm_func) const
	{
		return norm_func(value, last_value);
	}
};

const AbsoluteError absolute_error;

/// 誤差基準（相対誤差）
/**
	ManageConvergenceクラスのテンプレート引数に指定する関数オブジェクト
*/
struct RelativeError
{
	template <class T, class F>
	double operator()(T const& value, T const& last_value, F const& norm_func) const
	{
		return norm_func(value, last_value) / norm_func(last_value);
	}
};

const RelativeError relative_error;


/// 収束判定の管理を行うクラス
/**
	反復処理において、前回の値との差が指定値未満になると収束と判定する

	\code
	const double eps = 0.01;

	double value = 100;
	ManageConvergenceSimple conv(eps);

	while (!conv.update(value /= 2)) ;
	\endcode
*/
class ManageConvergenceSimple
{
	const double epsilon_;
	Maybe<double> last_value_;
	bool conv_;

public:
	/// コンストラクタ
	/**
		\param epsilon 収束判定用の定数（前回の値との差がこの定数未満であれば収束と判定する）
	*/
	ManageConvergenceSimple(double epsilon) : epsilon_(epsilon), last_value_(Nothing(0)), conv_(false) {}

	/// 状態の更新と収束判定
	/**
		\param value 収束判定に用いる値（ex:評価関数の出力）

		\return true(収束), false(未収束)
	*/
	bool update(double value){
		conv_ = isJust(last_value_) && abs_delta(value, fromJust(last_value_)) < epsilon_;
		last_value_ <<= value;

		return conv_;
	}

	/// 前回の値の取得
	double get_value() const{ return isJust(last_value_) ? fromJust(last_value_) : -1;	}

	bool is_convergence() const{ return conv_; }
};


/// 収束判定の計算と管理を行うクラス
/**
	反復処理を行う毎にその結果を与え、ノルム計算を行って収束の判定と管理を行う

	\tparam T 収束判定を行う数値ベクトルのデータ型
	\tparam C 誤差基準
	\tparam F ノルム関数

	\code
	const double eps = 0.01;
	const double delta = 0.1;

	std::vector<double> data{ 0.2, 0.3, 0.5 };
	ManageConvergence<std::vector<double>, RelativeError> conv(eps, norm_L2);

	while (!conv.update(data)){
		data[0] += delta * (0.3 - data[0]);
		data[1] += delta * (0.3 - data[1]);
		data[2] += delta * (0.4 - data[2]);
	}
	\endcode
*/
template <class T, class Criteria = RelativeError, class F = Norm<2>>
class ManageConvergence
{
	const double epsilon_;
	const F norm_func_;
	const Criteria criteria_;
	Maybe<T> last_value_;
	bool conv_;

public:
	/// コンストラクタ
	/**
		\param epsilon 収束判定用の定数（前回のノルム値との差がこの定数未満であれば収束と判定する）
		\param norm_function ノルム関数（1引数と2引数のoperator()を定義した関数オブジェクト）
	*/
	ManageConvergence(double epsilon, F norm_func = norm_L2) : epsilon_(epsilon), norm_func_(norm_func), criteria_(Criteria()), last_value_(Nothing(T())), conv_(false) {}

	/// 状態の更新とノルム計算、および収束判定
	/**
		\param value ノルムを取りたい数値ベクトル

		\return true(収束), false(未収束)
	*/
	bool update(T value){
		conv_ = isJust(last_value_) && criteria_(value, fromJust(last_value_), norm_func_) < epsilon_;
		last_value_ <<= value;

		return conv_;
	}

	bool is_convergence() const{ return conv_; }
};

}
#endif
