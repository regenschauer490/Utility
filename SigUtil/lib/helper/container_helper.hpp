/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTILCONTAINER_TRAITS_H
#define SIG_UTILCONTAINER_TRAITS_H


#include "eval.hpp"
#include "container_traits.hpp"
#include "type_convert.hpp"

namespace sig
{
extern void* enabler;

namespace impl{

#if SIG_ENABLE_MOVEITERATOR
template <class C,
	class RC = typename impl::remove_const_reference<C>::type,
	typename std::enable_if<std::is_same<C, RC>::value>::type *& = enabler,	// C is not const and not lvalue-reference
	typename std::enable_if<!std::is_same<typename RC::iterator, typename RC::const_iterator>::value>::type*& = enabler	// not to return const-iterator
>
auto begin(C&& c) ->std::move_iterator<typename RC::iterator>
{
	return std::make_move_iterator(std::begin(c));
}

template <class C,
	class RC = typename impl::remove_const_reference<C>::type,
	typename std::enable_if<(!std::is_same<C, RC>::value) || (std::is_same<typename RC::iterator, typename RC::const_iterator>::value)>::type *& = enabler
>
auto begin(C&& c) ->decltype(std::begin(c))
{
	return std::begin(c);
}

template <class C,
	class RC = typename impl::remove_const_reference<C>::type,
	typename std::enable_if<std::is_same<C, RC>::value>::type *& = enabler,	// C is not const and not lvalue-reference
	typename std::enable_if<!std::is_same<typename RC::iterator, typename RC::const_iterator>::value>::type*& = enabler	// not to return const-iterator
>
auto end(C&& c) ->std::move_iterator<typename RC::iterator>
{
	return std::make_move_iterator(std::end(c));
}

template <class C,
	class RC = typename impl::remove_const_reference<C>::type,
	typename std::enable_if<(!std::is_same<C, RC>::value) || (std::is_same<typename RC::iterator, typename RC::const_iterator>::value)>::type *& = enabler
>
auto end(C&& c) ->decltype(std::end(c))
{
	return std::end(c);
}

#else
template <class C>
auto begin(C&& c) ->decltype(std::begin(c))
{
	return std::begin(c);
}

template <class C>
auto end(C&& c) ->decltype(std::end(c))
{
	return std::end(c);
}
#endif

#if SIG_MSVC_ENV || (SIG_GCC_ENV && SIG_GCC_GT5_0_0) || (SIG_CLANG_ENV)
template <class C,
	class RC = typename impl::remove_const_reference<C>::type,
	typename std::enable_if<std::is_same<C, RC>::value>::type *& = enabler,
	typename std::enable_if<!std::is_same<typename RC::reverse_iterator, typename RC::const_reverse_iterator>::value>::type*& = enabler
>
auto rbegin(C&& c) ->std::move_iterator<typename RC::reverse_iterator>
{
	return std::make_move_iterator(std::rbegin(c));
}

template <class C,
	class RC = typename impl::remove_const_reference<C>::type,
	typename std::enable_if<(!std::is_same<C, RC>::value) || (std::is_same<typename RC::reverse_iterator, typename RC::const_reverse_iterator>::value)>::type *& = enabler
>
auto rbegin(C&& c) ->decltype(std::rbegin(c))
{
	return std::rbegin(c);
}


template <class C,
	class RC = typename impl::remove_const_reference<C>::type,
	typename std::enable_if<std::is_same<C, RC>::value>::type *& = enabler,	// C is not const and not lvalue-reference
	typename std::enable_if<!std::is_same<typename RC::reverse_iterator, typename RC::const_reverse_iterator>::value>::type*& = enabler	// not to return const-iterator
>
auto rend(C&& c) ->std::move_iterator<typename RC::reverse_iterator>
{
	return std::make_move_iterator(std::rend(c));
}

template <class C,
class RC = typename impl::remove_const_reference<C>::type,
	typename std::enable_if<(!std::is_same<C, RC>::value) || (std::is_same<typename RC::reverse_iterator, typename RC::const_reverse_iterator>::value)>::type *& = enabler
>
auto rend(C&& c) ->decltype(std::rend(c))
{
	return std::rend(c);
}
#endif


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


// コンテナの型に対応した要素型を得る
// ex: vector<T> const& -> T const&,	list<T>&& -> T&&
template <class C, typename std::enable_if<container_traits<typename remove_const_reference<C>::type>::exist>::type*& = enabler>
struct forward_element
{
private:
	using R1 = typename std::remove_reference<C>::type;	// remove reference
	using RR = typename std::remove_const<R1>::type;	// remove const and reference
	using ET = typename container_traits<RR>::value_type;

	using CT = typename std::conditional<
		std::is_const<R1>::value,
		typename std::add_const<ET>::type,
		ET
	>::type;	// add const

	using CRT = typename std::conditional<
		std::is_lvalue_reference<C>::value,
		typename std::add_lvalue_reference<CT>::type,
		typename std::add_rvalue_reference<CT>::type
		/*typename std::conditional<
			std::is_rvalue_reference<R2>::value,
			typename std::add_rvalue_reference<CT>::type,
			CT
		>::type*/
	>::type;	// add reference

public:
	using type = CRT;
};


template<class It>
auto dereference_iterator(It&& iter) ->decltype(*std::forward<It>(iter))
{
/*
	if(std::is_same<typename std::add_rvalue_reference<decltype(*iter)>::type, typename std::iterator_traits<It>::value_type&>{}) std::cout << "lval&";
	if(std::is_same<typename std::add_rvalue_reference<decltype(*iter)>::type, typename std::add_const<typename std::iterator_traits<It>::value_type>::type&>{}) std::cout << "const lval&";
	if(std::is_same<typename std::add_rvalue_reference<decltype(*iter)>::type, typename std::iterator_traits<It>::value_type&&>{}) std::cout << "rval&";
std::cout << std::endl;
*/
	return *std::forward<It>(iter);
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
auto copy(C&& src) ->RC
{
	RC dest = impl::container_traits<RC>::make(src.size());

	for (auto it = impl::begin(std::forward<C>(src)), end = impl::end(std::forward<C>(src)); it != end; ++it){
		impl::container_traits<RC>::add_element(dest, *it);
	}

	return dest;
}
	
}

#endif
