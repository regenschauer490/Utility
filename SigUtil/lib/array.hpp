/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_ARRAY_HPP
#define SIG_UTIL_ARRAY_HPP

#include "sigutil.hpp"
#include <array>
#include <initializer_list>

namespace sig
{
/// STLライクな静的配列
/**
	std::arrayの利便性を拡張し、固定長なstd::vectorの感覚で使用できる．\n
	基本的に初期化時に全要素に値を入れる使い方を想定しているが、逐次的に末尾に代入していくことも可能．\n
	メンバ関数begin()からend()で得られる範囲は、先頭から値が代入されている末尾までとなる([0]～[N-1]までではない)

	\tparam T 要素型（必要：デフォルトコンストラクタ、コピーコンストラクタ、コピー代入演算子）
	\tparam N 最大容量
*/
template <class T, size_t N>
class array
{
public:
	using value_type = T;
	using reference = T&;
	using const_reference = T const&;
	using pointer = T*;
	using const_pointer = T const*;
	using iterator = typename std::array<T, N>::iterator;
	using const_iterator = typename std::array<T, N>::const_iterator;
	using reverse_iterator = typename std::array<T, N>::reverse_iterator;
	using const_reverse_iterator = typename std::array<T, N>::const_reverse_iterator;
	using size_type = size_t;
	using difference_type = typename std::array<T, N>::difference_type;
#if SIG_ENABLE_BOOST
	using result_type = typename boost::call_traits<T>::value_type;
	using param_type = typename boost::call_traits<T>::param_type;
#else
	using result_type = value_type;
	using param_type = typename std::conditional<std::is_class<T>::value, T const&, T>::type;
#endif

private:
	std::array<T, N> array_;
	size_type tail_;				// 現在使用している添字末尾

private:
	template <class OP, class C1, class C2>
	void compound_assignment_(OP const& assign_op, C1& dest, C2 const& src)
	{
		auto it1 = std::begin(dest), end1 = std::end(dest);
		auto it2 = std::begin(src), end2 = std::end(src);

		while (it1 != end1 && it2 != end2){
			assign_op(*it1, *it2);
			++it1;
			++it2;
		}
	}

	void range_check_(size_type index)
	{
		if (index >= tail_){
			throw std::out_of_range("invalid sig::array<T, N> subscript");
		}
	}

	// pos以降の要素を1つ後へずらす
	iterator back_shift_(const_iterator pos)
	{
		auto it = rbegin();
		for (auto pit = it; it.base() != pos; pit = it++){
			*pit = std::move(*it);
		}
		return it.base();
	}

	// pos以降の要素を1つ前へずらす
	iterator front_shift_(iterator pos)
	{
		iterator post = pos;
		for (auto pit = pos, end = this->end(); pos != end; pit = pos++){
			*pit = std::move(*pos);
		}
		return post;
	}

public:
	/// デフォルトコンストラクタ
	array() : tail_(0) {}

	/// 値をコピーしてコンストラクト
	/**
		val を size 個コピーする．以降、このオブジェクトには[0]～[size-1]までアクセス可能

		\param size 生成する要素数（N以下）
		\param val 初期化に使用する値
	*/
	array(size_type size, T val) : tail_(0){ for(size_type i=0; i<size; ++i) push_back(val); }

	/// initializer_list から値をムーブしてコンストラクト
	/**
		init の先頭から順番に値をムーブする

		\param init 初期化に使用する値リスト（サイズはN以下）
	*/
	array(std::initializer_list<T> init) : tail_(0)
	{
		assert(init.size() <= N);
		for(auto it = init.begin(), end = init.end(); it != end; ++it){
			array_[tail_] = std::move(*it);
			++tail_;
		}
	}

	/// std::array から値をコピーしてコンストラクト
	/**
		\param src 初期化に使用する値リスト（サイズはN以下）
	*/
	template <size_type M>
	explicit array(std::array<T,M> const& src) : tail_(0)
	{
		static_assert(M <= N, "M must be less than N");
		compound_assignment_([this](T& dest, ParamType<T> src){ dest = src; ++tail_; }, array_, src);
	}

	/// sig::array(サイズが異なる)から値をコピーしてコンストラクト
	/**
		\param src 初期化に使用する値リスト（サイズはN以下）
	*/
	template <size_type M>
	explicit array(sig::array<T,M> const& src) : tail_(src.size())
	{
		static_assert(M <= N, "M must be less than N");
		compound_assignment_([this](T& dest, ParamType<T> src){ dest = src; }, array_, src.std_array());
	}

	/// コピーコンストラクタ
	/**
		\param src コピー元
	*/
	array(array const& src) : tail_(src.tail_)
	{
		compound_assignment_([this](T& dest, ParamType<T> src){ dest = src; }, array_, src.array_);
	}

	/// ムーブコンストラクタ
	/**
		\param src ムーブ元

		\post src の現サイズは0になる
	*/
	array(array&& src) : tail_(0)
	{
		for (auto it = src.begin(), end = src.end(); it != end; ++it){
			array_[tail_] = std::move(*it);
			++tail_;
		}
		src.tail_ = 0;
	}

	/// コピー代入演算子
	/**
		\param src コピー元

		\return このオブジェクトへの参照
	*/
	array& operator=(array const& src)
	{
		auto tmp = array(src);
		swap(tmp);
		return *this;
	}

	/// ムーブ代入演算子
	/**
		\param src ムーブ元

		\return このオブジェクトへの参照

		\post src の現サイズは0になる
	*/
	array& operator=(array&& src)
	{
		swap(src);
		src.tail_ = 0;
		return *this;
	}

	/// std::array から値をコピーして代入
	/**
		\param src 代入される値リスト（サイズはN以下）

		\return このオブジェクトへの参照
	*/
	template <size_type M>
	array& operator=(std::array<T,M> const& src)
	{
		static_assert(M <= N, "M must be less than N");
		array copy(src);
		swap(copy);
		return *this;
	}

	/// sig::array(サイズが異なる)から値をコピーして代入
	/**
		\param src 代入される値リスト（サイズはN以下）

		\return このオブジェクトへの参照
	*/
	template <size_type M>
	array& operator=(sig::array<T, M> const& src)
	{
		static_assert(M <= N, "M must be less than N");
		array copy(src);
		swap(copy);
		return *this;
	}

	/// 等値比較演算子
	bool operator==(array const& obj){
		if (tail_ != obj.tail_) return false;
		bool same = true;
		for (size_type i = 0; i < tail_; ++i) same = same && (array_[i] == obj.array_[i]);
		return same;
	}

	/// 非等値比較演算子
	bool operator!=(array const& obj){
		return !(*this == obj);
	}

	/// 先頭要素を指すイテレータを返す
	/**
		\return 先頭要素を指すイテレータ
	*/
	iterator begin() noexcept{ return array_.begin(); }
	const_iterator begin() const noexcept{ return array_.begin(); }
	const_iterator cbegin() const noexcept{ return array_.cbegin(); }

	/// 末尾要素の後ろを指すイテレータを返す
	/**
		末尾の添字は最大容量N-1でなく、現在のサイズ(size()が返す値)-1であることに注意．\n
		このイテレータへのアクセスは未定義動作

		\return 末尾要素の後ろを指すイテレータ
	*/
	iterator end() noexcept{ return array_.begin() + tail_; }
	const_iterator end() const noexcept{ return array_.begin() + tail_; }
	const_iterator cend() const noexcept{ return array_.cbegin() + tail_; }

	/// 末尾要素を指す逆イテレータを返す
	/**
		\return 末尾要素を指す逆イテレータ
	*/
	reverse_iterator rbegin() noexcept{ return array_.rend() - tail_; }
	const_reverse_iterator rbegin() const noexcept{ return array_.rend() - tail_; }
	const_reverse_iterator crbegin() const noexcept{ return array_.crend() - tail_; }

	/// 先頭要素の前を指す逆イテレータを返す
	/**
		このイテレータへのアクセスは未定義動作

		\return 先頭要素の前を指す逆イテレータ
	*/
	reverse_iterator rend() noexcept{ return array_.rend(); }
	const_reverse_iterator rend() const noexcept{ return array_.rend(); }
	const_reverse_iterator crend() const noexcept{ return array_.crend(); }

	/// 添字による要素へのアクセス
	/**
		\param pos 添字

		\return 指定要素への参照

		\exception std::out_of_range （if pos >= size()）
	*/
	reference at(size_type pos){ range_check_(pos); return array_.at(pos); }
	const_reference at(size_type pos) const{ range_check_(pos); return array_.at(pos); }

	/// 添字による要素へのアクセス
	/**
		現在のサイズ(size()が返す値)以降の添字アクセスは未定義動作

		\param pos 添字

		\return 指定要素への参照
	*/
	reference operator [](size_type pos){ return array_[pos]; }
	const_reference operator [](size_type pos) const{ return array_[pos]; }

	/// 先頭要素へのアクセス
	/**
		\return 先頭要素への参照
	*/
	reference front(){ return array_.front(); }
	const_reference front() const{ return array_.front(); }

	/// 末尾要素へのアクセス
	/**
		\return 末尾要素への参照
	*/
	reference back(){ return tail_ ? array_[tail_-1] : array_.front(); }
	const_reference back() const{ return tail_ ? array_[tail_ - 1] : array_.front(); }

	/// コンテナが空であるかを確認
	/**
		\return コンテナが空ならtrue, そうでなければfalse
	*/
	bool empty() const noexcept{ return tail_ == 0; }

	/// コンテナに格納された要素数を返す
	/**
		std::distance(begin(), end())と等価．
		最大サイズとは異なることに注意

		\return コンテナに格納された要素数
	*/
	size_type size() const noexcept{ return tail_; }

	/// コンテナの最大容量を返す
	/**
		\return Nの値を返す
	*/
	size_type max_size() const noexcept{ return N; }

	/// 内部で保有する生配列へのポインタを返す
	/**
		\return 先頭要素を指すポインタ
	*/
	value_type* data() noexcept{ return array_.data(); }
	value_type const* data() const noexcept{ return array_.data(); }

	/// 内部で保有するstd::arrayへの参照を返す
	/**
		\return 内部で保有するstd::arrayへの参照
	*/
	auto std_array() noexcept ->std::array<T, N>&{ return array_; }
	auto std_array() const noexcept ->std::array<T, N> const&{ return array_; }

	/// コンテナの全要素（最大容量まで）を指定した値でコピーする
	/**
		\val コピーしたい値
	*/
	void fill(value_type const& val){ array_.fill(val); tail_ = N; }

	/// 他の最大容量が同じsig::arrayと保持メンバを入れ替える
	/**
		\param other swapしたいsig::array
	*/
	void swap(array& other) //noexcept(noexcept(swap(std::declval<value_type&>(), std::declval<value_type&>())))
	{
		std::swap(array_, other.array_); 
		std::swap(tail_, other.tail_);
	}

	/// コンテナの末尾に指定した値を追加する
	/**
		現在のサイズ(size()が返す値)が最大容量未満であれば、コンテナの末尾に新しく要素を追加する

		\param val 追加したい値（コピー または ムーブ）

		\exception std::length_error （if size() >= N）
	*/
	void push_back(value_type const& val)
	{
		if (tail_ >= N) throw std::length_error("this container reached upper maxsize limit. so, can't hold more elements.");
		array_[tail_++] = val;
	}
	void push_back(value_type&& val)
	{
		if (tail_ >= N) throw std::length_error("this container reached upper maxsize limit. so, can't hold more elements.");
		array_[tail_++] = std::forward<value_type>(val); 
	}

	/// コンテナの末尾から要素を取り除く
	void pop_back() noexcept{ if(tail_ > 0) --tail_; }

	/// 指定位置の後ろに指定した値を挿入する
	/**
		現在のサイズ(size()が返す値)が最大容量未満であれば、コンテナの指定位置に新しい要素を挿入する

		\param position 挿入位置を指すイテレータ（この位置の後ろに挿入）
		\param val 追加したい値（コピー または ムーブ）

		\return 挿入位置（追加した要素）を指すイテレータ

		\exception std::length_error （if size() >= N）
	*/
	iterator insert(const_iterator position, const value_type& val)
	{
		if (tail_ >= N) throw std::length_error("this container reached upper maxsize limit. so, can't hold more elements.");
		++tail_;
		auto p = back_shift_(position);
		*p = val;
		return p;
	}
	iterator insert(const_iterator position, value_type&& val)
	{
		if (tail_ >= N) throw std::length_error("this container reached upper maxsize limit. so, can't hold more elements.");
		++tail_;
		auto p = back_shift_(position);
		*p = std::move(val);
		return p;
	}

	/// 指定位置の要素を取り除く
	/**
		取り除かれた後、それ以降の要素は前に詰められる

		\param pos 取り除く要素を指すイテレータ

		\return 取り除かれた要素の後ろの要素を指すイテレータ
	*/
	iterator erase(iterator position)
	{
		auto p = front_shift_(position);
		--tail_;
		return p;
	}

	/// コンテナを空にする
	void clear() noexcept{ tail_ = 0; }
};

}
#endif
