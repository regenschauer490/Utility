/*
Copyright(c) 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTILCONTAINER_TRAITS_H
#define SIG_UTILCONTAINER_TRAITS_H


#include "traits/container_traits.hpp"

namespace sig
{
extern void* enabler;

namespace impl{
template<class It>
void increment_iterator(It& iter)
{
	++iter;
}
template<class It, class... Its>
void increment_iterator(It& iter, Its&... iterators)
{
	++iter;
	increment_iterator(iterators...);
}

template<class It>
auto dereference_iterator(It& iter) ->typename std::add_rvalue_reference<decltype(*iter)>::type
{
/*
	if(std::is_same<typename std::add_rvalue_reference<decltype(*iter)>::type, typename std::iterator_traits<It>::value_type&>{}) std::cout << "lval&";
	if(std::is_same<typename std::add_rvalue_reference<decltype(*iter)>::type, typename std::add_const<typename std::iterator_traits<It>::value_type>::type&>{}) std::cout << "const lval&";
	if(std::is_same<typename std::add_rvalue_reference<decltype(*iter)>::type, typename std::iterator_traits<It>::value_type&&>{}) std::cout << "rval&";
std::cout << std::endl;
*/
	return *iter;
}
}

// 複数のイテレータに対して、loop回数だけ繰り返しデリファレンス+関数適用して結果をdestに格納
template <class C, class F, class... Its>
void iterative_make(std::size_t loop, C& dest, F const& func, Its... iterators)
{
	for (std::size_t i = 0; i < loop; ++i, impl::increment_iterator(iterators...)){
		container_traits<C>::add_element(dest, eval(func, impl::dereference_iterator(iterators)...));
	}
}

// 複数のイテレータに対して、loop回数だけ繰り返しデリファレンス+関数適用(副作用あり)
template <class F, class... Its>
void iterative_assign(std::size_t loop, F const& func, Its... iterators)
{
	for (std::size_t i = 0; i < loop; ++i, impl::increment_iterator(iterators...)){
		func(impl::dereference_iterator(iterators)...);
	}
}
template <class F, class... Its>
void iterative_assign(std::size_t loop, int init, F const& func, Its... iterators)
{
	for (std::size_t i = 0; i < loop; ++i, impl::increment_iterator(iterators...)){
		func(i + init, impl::dereference_iterator(iterators)...);
	}
}


template <class T, class D = void> struct has_random_access_op{ static const bool value = false; };
template <class T> struct has_random_access_op<T, decltype(std::declval<typename T::iterator>()[0], void())>{ static const bool value = true; };

template <class C>
void erase(C& container, typename sequence_container_traits<C>::value_type const& t)
{
	container.erase(std::remove(std::begin(container), std::end(container), t), std::end(container));
}
template <class C>
void erase(C& container, typename associative_container_traits<C>::value_type const& t)
{
	container.erase(t);
}
template <class C>
void erase(C& container, typename hash_container_traits<C>::value_type const& t)
{
	container.erase(t);
}

template <class C, class F, typename std::enable_if<sequence_container_traits<C>::exist>::type*& = enabler>
void erase_if(C& container, F const& remove_pred)
{
	container.erase(std::remove_if(std::begin(container), std::end(container), remove_pred), std::end(container));
}
template <class C, class F, typename std::enable_if<associative_container_traits<C>::exist>::type*& = enabler>
void erase_if(C& container, F const& remove_pred)
{
	for (auto it = std::begin(container), end = std::end(container); it != end;){
		if (remove_pred(*it)){
			it = container.erase(it);
		}
		else ++it;
	}
}
template <class C, class F, typename std::enable_if<hash_container_traits<C>::exist>::type*& = enabler>
void erase_if(C& container, F const& remove_pred)
{
	for (auto it = std::begin(container), end = std::end(container); it != end;){
		if (remove_pred(*it)){
			it = container.erase(it);
		}
		else ++it;
	}
}

//別のコンテナに要素をコピーする
template <class RC, class C>
auto copy(C const& src) ->RC
{
	RC dest;
	for (auto const& e : src) container_traits<RC>::add_element(dest, e);
	return dest;
}
	
}

#endif
