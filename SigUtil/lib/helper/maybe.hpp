/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef _SIG_UTIL_MAYBE_HPP
#define _SIG_UTIL_MAYBE_HPP

#include "../sigutil.hpp"
#include "eval.hpp"
#include "container_traits.hpp"

#if SIG_ENABLE_BOOST && SIG_USE_OPTIONAL
#include <boost/optional.hpp>
#endif

/// \file maybe.hpp HaskellのData.Maybeの機能を模倣した関数と、boost.optionalのためのユーティリティ関数

/** \page sig_maybe Maybe
	　SigUtilでは，値の取得失敗や無効値が生じる可能性がある関数の返り値として，Maybe型のオブジェクトを返すことで成否を判定できるようにしている．\n
	すなわち，処理が成功していれば返り値から値を取り出すことができ，失敗していれば値を取り出す前に失敗したことを知ることができる．\n
	以下にサンプルコードを示す．

	\code
	if(auto result = kl_divergence(dist1, dist2)))
	{
		double kl_value = *result;	// or fromJust(result);
	}
	\endcode
	
	　SigUtilのMaybeに関する機能は，boost.optionalと補助的な非メンバ関数から構成されており，HaskellのData.Maybeの機能を再現することに加え，C++で扱い易い様に工夫を加えている．\n
	boost.optionalの使用経験がある人は今まで通りの感覚で使用でき，加えてbind演算（\ref maybe_bindop1 , \ref maybe_bindop2 ）や簡潔な値の代入（\ref maybe_assignop ）などを行えるようになる．\n\n

	諸々の事情でboost.optionalを使うことができない場合，Maybeの実体は std::tuple<T, bool>となる（ただし，今後変更の可能性あり）．
*/

namespace sig
{
	
// Maybe(boost.optional)の有効・無効に関係なくコードを統一的に記述するための処理
#if SIG_ENABLE_BOOST && SIG_USE_OPTIONAL
	template <class T> using Maybe = boost::optional<T>;

	/// 値コンストラクタ
	/**
		\code
		auto m1 = Just(1);			// :: Maybe<int>
		auto m2 = Just<double>(1);	// :: Maybe<double>
		\endcode
	*/
	template <class T> Maybe<T> Just(T&& v){ return Maybe<T>(std::forward<T>(v)); }
	template <class T> Maybe<T> Just(T const& v){ return Maybe<T>(v); }		// T is given explicitly

	/// 値コンストラクタ
	/**
		\code
		auto n = Nothing();	// :: Maybe<int>
		\endcode
	*/
	inline auto Nothing()-> decltype(boost::none){ return boost::none; }

	/// 値コンストラクタ
	/**
		引数はダミー（optional無効時にも統一的に記述できるようにするために存在）

		\code
		auto n = Nothing(0);	// :: Maybe<int>
		\endcode
	*/
	template <class T> auto Nothing(T const& dummy)-> decltype(boost::none){ return boost::none; }


	/// Justであるか調べる関数．Maybe a -> Bool
	template <class T> bool isJust(Maybe<T> const& m){ return static_cast<bool>(m); }
	constexpr bool isJust(boost::none_t m){ return false; }
	

	/// Nothingであるか調べる関数．Maybe a -> Bool
	template <class T> bool isNothing(Maybe<T> const& m){ return !static_cast<bool>(m); }
	constexpr bool isNothing(boost::none_t m){ return true; }


	/// Justから値を取り出す関数．Maybe a -> a
	/**
		Maybeの正体がNothingオブジェクトの場合、未定義動作

		\sa operator*(Maybe<T>& m)

		\code
		auto m = Just<int>(1);
		const auto cm = Just<int>(2);

		int v1 = fromJust(m);		// v == 1

		auto& v2 = fromJust(m);		// int&
		rv = 5;
		assert(*m == 5);

		auto& v3 = sig::fromJust(cm);	// const int&
		auto v4 = sig::fromJust(cm);	// int
		\endcode
	*/
	template <class T> T& fromJust(Maybe<T>& m){ return *m; }
	template <class T> T const& fromJust(Maybe<T> const& m){ return *m; }
	template <class T> T&& fromJust(Maybe<T>&& m){ return std::move(*m); }


	/// fromJust演算子
	/**
		fromJust関数の演算子版．\n
		Maybeの正体がNothingオブジェクトの場合、未定義動作

		\sa fromJust(Maybe<T>& m)

		\code
		auto m = Just<int>(1);
		int v = *m;		// v == 1
		\endcode
	*/
	template <class T> T& operator*(Maybe<T>& m){ return *m; }
	template <class T> T const& operator*(Maybe<T> const& m){ return *m; }
	template <class T> T&& operator*(Maybe<T>&& m){ return *std::move(m); }


	/// Maybeから値を取り出す関数．もしNothingなら引数のデフォルト値を返す．a -> Maybe a -> a
	/**
		\code
		auto m = Just<int>(1);
		auto n = Nothing(0);

		int v = fromMaybe(2, m);	// v == 1
		int d = fromMaybe(2, n);	// d == 2
		\endcode
	*/
	template <class T> T& fromMaybe(T& def, Maybe<T>& m){ return m ? *m : def; }
	template <class T> T const& fromMaybe(T const& def, Maybe<T> const& m){ return m ? *m : def; }
	template <class T> T&& fromMaybe(T&& def, Maybe<T>&& m){ return m ? std::move(*m) : std::move(def); }
	template <class T> T&& fromMaybe(T&& def, boost::none_t m){ return std::move(def); }


	/// Maybeを返す関数を指定するfilter関数．(a -> Maybe b) -> [a] -> [b]
	/**
		コンテナの各要素にMaybeを返す関数を適用し、結果がJustであれば値を保存し、Nothingであれば無視する

		\param f Maybeを返す関数
		\param list \ref sig_container

		\return 結果を格納したlistと同じ種類のコンテナ

		\code
		std::vector<int> vec{ 1, 2, 3 };
		auto vecd = mapMaybe([](int v){ return v != 2 ? Just<double>(v *1.5) : Nothing(0); }, vec);
		vecd[0];		// 1.5
		vecd[1];		// 4.5
		vecf[2];		// out of range. undefined behavior
		\endcode
	*/
	template <class F, class C> auto mapMaybe(F const& f, C const& list)
		->typename impl::container_traits<C>::template rebind<
			typename std::remove_reference<decltype(*(impl::eval(
				f, 
				std::declval<typename impl::container_traits<C>::value_type>()
			)))>::type
		>
	{
		using R = typename impl::container_traits<C>::template rebind<typename std::remove_reference<decltype(*(impl::eval(f, std::declval<typename impl::container_traits<C>::value_type>())))>::type>;

		R result;

		for (auto const& e : list){
			auto r = f(e);
			if (r) impl::container_traits<R>::add_element(result, *r);
		}

		return result;
	}


	/// Haskell風のbind演算子. Maybe m => m a -> (a -> m b) -> m b	\anchor maybe_bindop1
	/**
		C++では operator>>= は右結合であるため、連鎖させる際には明示的に()を使う必要あり（以下のサンプルコード参照）
		
		\param ma Maybeオブジェクト
		\param f 適用する関数

		\return 関数適用結果が格納されたMaybeオブジェクト

		\sa operator<<=(F const& f, Maybe<T> const& m)

		\code
		// using sig::operator>>=;	// required in global namespace

		auto m = Just<int>(1);
		auto f = [&](int v){ return sig::Just<double>(v * 1.5); };	// (a -> m b)

		auto mm = (m >>= f) >>= [&](double v){ return sig::Just<bool>(v < 10); };	// Haskell Style
		assert(*mm);	// mm == true
		\endcode
	*/
	template <class T, class F>
	auto operator>>=(Maybe<T> const& m, F const& f) ->decltype(impl::eval(f, std::declval<T>()))
	{
		if (m) return f(*m);
		else return boost::none;
	}

	/// C++に合わせたbind演算子. Maybe m => (a -> m b) -> m a -> m b	\anchor maybe_bindop2
	/**
		C++では operator>>= と operator<<= は右結合であるため、処理が右端から順番に流れていく方が自然に記述できる（以下のサンプルコード参照）
		
		\param f 適用する関数
		\param ma Maybeオブジェクト

		\return 関数適用結果が格納されたMaybeオブジェクト

		\sa operator>>=(Maybe<T> const& m, F const& f)

		\code
		// using sig::operator<<=;	// required in global namespace

		auto m = Just<int>(1);
		auto f = [&](int v){ return sig::Just<double>(v * 1.5); };	// (a -> m b)

		auto mm = [&](double v){ return sig::Just<bool>(v < 10); } <<= f <<= m;		// suitable style for C++
		assert(*mm);	// mm == true
		\endcode
	*/
	template <class F, class T>
	auto operator<<=(F const& f, Maybe<T> const& m) ->decltype(impl::eval(f, std::declval<T>()))
	{
		if (m) return f(*m);
		else return boost::none;
	}


	/// Maybeオブジェクトへの再代入を行う演算子	\anchor maybe_assignop
	/**
		関数型言語では再代入は不可能な機能であるが、C++では無いと不便なのでこの関数を作成．\n
		Maybeオブジェクトへの再代入をJust, Nothingに関係なく統一的に記述することが目的


		\param m Maybeオブジェクト
		\param v 代入したい値

		\return mへの参照

		\code
		// using sig::operator<<=;	// required in global namespace

		auto m = Just<int>(1);
		m <<= 2;

		int v = *m;	// v == 2
		\endcode
	*/
	template <class T1, class T2>
	auto operator<<=(Maybe<T1>& m, T2 const& v) ->Maybe<T1>&
	{
		if (m) *m = v;
		else m = Maybe<T1>(static_cast<T1>(v));
		return m;
	}
	
#else
	// optionalが使えない場合の代替処理
	// 当ライブラリコードを統一的に記述するために用いているため、ユーザコードでの使用は非推奨

	template <class I, class D = void> using Maybe = std::tuple<I, bool>;

	template <class I> Maybe<I> Just(I v){ return std::make_tuple(v, true); }

	template <class I> Maybe<I> Nothing(I&& dummy){ return std::make_tuple(dummy, false); }
	template <class I> Maybe<I> Nothing(I const& dummy){ return std::make_tuple(dummy, false); }

	template <class I> bool isJust(Maybe<I> const& m){ return std::get<1>(m); }

	template <class I> bool isNothing(Maybe<I> const& m){ return !std::get<1>(m); }

	template <class T> T& fromJust(Maybe<T>& m){ return *m; }
	template <class T> T const& fromJust(Maybe<T> const& m){ return *m; }
	template <class T> T&& fromJust(Maybe<T>&& m){ return *std::move(m); }

	template <class T> T& operator*(Maybe<T>& m){
		if (!std::get<1>(m)) std::abort();
		return std::get<0>(m);
	}
	template <class T> T const& operator*(Maybe<T> const& m){
		if (!std::get<1>(m)) std::abort();
		return std::get<0>(m);
	}
	template <class T> T&& operator*(Maybe<T>&& m){
		if (!std::get<1>(m)) std::abort();
		return std::move(std::get<0>(m));
	}


	template <class T1, class T2>
	auto operator<<=(Maybe<T1>& m, T2 const& v) ->Maybe<T1>&
	{
		std::get<0>(m) = v;
		std::get<1>(m) = true;
		return m;
	}
	
#endif


}	// sig
#endif
