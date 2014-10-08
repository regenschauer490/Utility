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

namespace sig
{
	
// Maybe(boost.optional)の有効・無効に関係なくコードを統一的に記述するための処理
#if SIG_ENABLE_BOOST && SIG_USE_OPTIONAL
	template <class T> using Maybe = boost::optional<T>;

	/// 値コンストラクタ
	/**
		\code
		auto m = Just<int>(1);	// :: Maybe<int>
		\endcode
	*/
	template <class T> using Just = Maybe<T>;

	/// 値コンストラクタ
	/**
		引数はダミー（optional無効時にも統一的に記述できるようにするために存在）

		\code
		auto n = Nothing(0);	// :: Maybe<int>
		\endcode
	*/
	template <class T> auto Nothing(T const& dummy)-> decltype(boost::none){ return boost::none; }


	/// Justであるか調べる関数．Maybe a -> Bool
	template <class T> bool isJust(Maybe<T> const& m){ return m; }
	constexpr bool isJust(boost::none_t m){ return false; }


	/// Nothingであるか調べる関数．Maybe a -> Bool
	template <class T> bool isNothing(Maybe<T> const& m){ return !m; }
	constexpr bool isNothing(boost::none_t m){ return true; }


	/// Justから値を取り出す関数．Maybe a -> a
	/**
		Maybeの正体がNothingオブジェクトの場合、未定義動作

		\code
		auto m = Just<int>(1);
		int v = fromJust(m);	// v == 1
		\endcode
	*/
	template <class T> T& fromJust(Maybe<T>& m){ return *m; }
	template <class T> T const& fromJust(Maybe<T> const& m){ return *m; }
	template <class T> T&& fromJust(Maybe<T>&& m){ return std::move(*m); }


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


	/// bind演算子. Maybe m => m a -> (a -> m b) -> m b
	/**
		\param ma maybe<T>型オブジェクト
		\param f 適用する関数

		\return
	*/
	template <class F, class T>
	auto operator>>=(Maybe<T> const& ma, F const& f) ->Maybe<decltype(impl::eval(f, std::declval<T>()))>
	{
		if (ma) return f(*ma);
		else return boost::none;
	}


	/// Maybeオブジェクトへの再代入を行う演算子
	/**
		関数型言語では再代入は不可能な機能であるが、C++では無いと不便なのでこの関数を作成．\n
		Maybeオブジェクトへの再代入を統一的に記述することが目的

		\param m Maybeオブジェクト
		\param v 代入したい値

		\return mへの参照
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
	template <class I, class D = void> using Maybe = std::tuple<I, bool>;

	template <class I> Maybe<I> Just(I v){ return std::make_tuple(v, true); }

	template <class I> Maybe<I> Nothing(I&& dummy){ return std::make_tuple(dummy, false); }
	template <class I> Maybe<I> Nothing(I const& dummy){ return std::make_tuple(dummy, false); }

	template <class I> bool isJust(Maybe<I> const& m){ return std::get<1>(m) ? true : false; }
	//template <class P, typename std::enable_if<std::is_pod<P>::value>::type*& = enabler> bool isJust(P m){ return m; }
	//template <class C, typename std::enable_if<impl::container_traits<C>::exist>::type*& = enabler> bool isJust(C const& m){ return !m.empty(); }
	//template <class T, typename std::enable_if<!std::is_pod<T>::value && !impl::container_traits<T>::exist>::type*& = enabler> bool isJust(T const& m){ return T() == m; }


	template <class T> T const& fromJust(Maybe<T> const& m){
		if (!std::get<1>(m)) std::abort();
		return std::get<0>(m);
	}
	template <class T> T&& fromJust(Maybe<T>&& m){
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
