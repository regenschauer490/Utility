/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTILCONTAINER_TRAITS_H
#define SIG_UTILCONTAINER_TRAITS_H


#include "eval.hpp"
#include "../helper/container_traits.hpp"

namespace sig
{
extern void* enabler;

namespace impl{

template <class C>
auto begin(C&& c) ->std::move_iterator<decltype(std::begin(c))>
{
	return std::make_move_iterator(std::begin(c));
}

template <class C>
auto begin(C& c) ->decltype(c.begin())
{
	return std::begin(c);
}

template <class C>
auto begin(C const& c) ->decltype(c.begin())
{
	return std::begin(c);
}


template<class It>
void increment_iterator(It&& iter)
{
	++iter;
}
template<class It, class... Its>
void increment_iterator(It&& iter, Its&&... iterators)
{
	++iter;
	increment_iterator(iterators...);
}


template <class T>
struct remove_const_reference
{
	using type = typename std::remove_reference<typename std::remove_const<T>::type>::type;
};


// コンテナの型に対応した要素型を得る
// ex: vector<T> const& -> T const&,	list<T>&& -> T&&
template <class C>
struct actual_element
{
private:
	using R1 = typename std::remove_reference<C>::type;
	using R2 = typename std::remove_const<C>::type;
	using RR = typename std::remove_const<R1>::type;

	using CT = typename std::conditional<
		std::is_const<R1>::value,
		typename std::add_const<typename container_traits<RR>::value_type>::type,
		typename container_traits<RR>::value_type
	>::type;

	using CRT = typename std::conditional<
		std::is_lvalue_reference<R2>::value,
		typename std::add_lvalue_reference<CT>::type,
		typename std::add_rvalue_reference<CT>::type
		/*typename std::conditional<
			std::is_rvalue_reference<R2>::value,
			typename std::add_rvalue_reference<CT>::type,
			CT
		>::type*/
	>::type;

public:
	using type = CRT;
};


template<class It>
auto dereference_iterator(It&& iter) ->typename std::add_rvalue_reference<decltype(*iter)>::type
{
/*
	if(std::is_same<typename std::add_rvalue_reference<decltype(*iter)>::type, typename std::iterator_traits<It>::value_type&>{}) std::cout << "lval&";
	if(std::is_same<typename std::add_rvalue_reference<decltype(*iter)>::type, typename std::add_const<typename std::iterator_traits<It>::value_type>::type&>{}) std::cout << "const lval&";
	if(std::is_same<typename std::add_rvalue_reference<decltype(*iter)>::type, typename std::iterator_traits<It>::value_type&&>{}) std::cout << "rval&";
std::cout << std::endl;
*/
	return *iter;
}

}	//impl

// 複数のイテレータに対して、loop回数だけ繰り返しデリファレンス+関数適用して結果をdestに格納
template <class C, class F, class... Its>
void iterative_make(uint loop, C& dest, F&& func, Its... iterators)
{
	for (uint i = 0; i < loop; ++i, impl::increment_iterator(iterators...)){
		impl::container_traits<C>::add_element(dest, impl::eval(std::forward<F>(func), impl::dereference_iterator(iterators)...));
	}
}

// 複数のイテレータに対して、loop回数だけ繰り返しデリファレンス+関数適用して結果をdestに集約
template <class T, class F1, class F2, class... Its>
void iterative_fold(uint loop, T& dest, F1&& zip, F2&& fold, Its... iterators)
{
	for (uint i = 0; i < loop; ++i, impl::increment_iterator(iterators...)){
		dest = std::forward<F2>(fold)(dest, impl::eval(std::forward<F1>(zip), impl::dereference_iterator(iterators)...));
	}
}


// 複数のイテレータに対して、loop回数だけ繰り返しデリファレンス+関数適用(副作用あり)
template <class F, class... Its>
void iterative_assign(uint loop, F&& func, Its... iterators)
{
	for (uint i = 0; i < loop; ++i, impl::increment_iterator(iterators...)){
		std::forward<F>(func)(impl::dereference_iterator(iterators)...);
	}
}
template <class F, class... Its>
void iterative_assign(uint loop, int init, F&& func, Its... iterators)
{
	for (uint i = 0; i < loop; ++i, impl::increment_iterator(iterators...)){
		std::forward<F>(func)(i + init, impl::dereference_iterator(iterators)...);
	}
}


template <class T, class D = void> struct has_random_access_op{ static const bool value = false; };
template <class T> struct has_random_access_op<T, decltype(std::declval<typename T::iterator>()[0], void())>{ static const bool value = true; };

template <class C>
void erase(C& container, typename impl::sequence_container_traits<C>::value_type const& t)
{
	container.erase(std::remove(std::begin(container), std::end(container), t), std::end(container));
}
template <class C>
void erase(C& container, typename impl::associative_container_traits<C>::value_type const& t)
{
	container.erase(t);
}
template <class C>
void erase(C& container, typename impl::hash_container_traits<C>::value_type const& t)
{
	container.erase(t);
}

template <class C, class F, typename std::enable_if<impl::sequence_container_traits<C>::exist>::type*& = enabler>
void erase_if(C& container, F const& remove_pred)
{
	container.erase(std::remove_if(std::begin(container), std::end(container), remove_pred), std::end(container));
}
template <class C, class F, typename std::enable_if<impl::associative_container_traits<C>::exist>::type*& = enabler>
void erase_if(C& container, F const& remove_pred)
{
	for (auto it = std::begin(container), end = std::end(container); it != end;){
		if (remove_pred(*it)){
			it = container.erase(it);
		}
		else ++it;
	}
}
template <class C, class F, typename std::enable_if<impl::hash_container_traits<C>::exist>::type*& = enabler>
void erase_if(C& container, F const& remove_pred)
{
	for (auto it = std::begin(container), end = std::end(container); it != end;){
		if (remove_pred(*it)){
			it = container.erase(it);
		}
		else ++it;
	}
}

/// 別の種類のコンテナに要素をコピーする
/**
	\tparam RC コピー先のコンテナ型（\ref sig_container ）

	\param src コピー元のコンテナ（\ref sig_container ）

	\return コピーされたコンテナ
*/
template <class RC, class C>
auto copy(C const& src) ->RC
{
	RC dest;
	for (auto const& e : src) impl::container_traits<RC>::add_element(dest, e);
	return dest;
}
	
}

#endif
