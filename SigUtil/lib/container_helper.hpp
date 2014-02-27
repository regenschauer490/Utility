/*
Copyright(c) 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTILCONTAINER_TRAITS_H
#define SIG_UTILCONTAINER_TRAITS_H


#include "external/container traits/container_traits.h"

namespace sig
{
	extern void* enabler;

	template<class It>
	void IncrementIterator(It& iter)
	{
		++iter;
	}
	template<class It, class... Its>
	void IncrementIterator(It& iter, Its&... iterators)
	{
		++iter;
		IncrementIterator(iterators...);
	}

	template<class It>
	auto DereferenceIterator(It& iter)
	{
		return *iter;
	}

	//variadic templatesで受け取った複数のイテレータに対して、loop回数だけ繰り返しデリファレンス+関数適用する
	template <class C, class F, class... Its>
	void Iterate(std::size_t loop, C& dest, F const& func, Its... iterators)
	{
		for (std::size_t i = 0; i < loop; ++i, IncrementIterator(iterators...)){
			container_traits<C>::add_element(dest, eval(func, DereferenceIterator(iterators)...));
		}
	}

	template <class T, class D = void> struct HasRandomIter{ static const bool value = false; };
	template <class T> struct HasRandomIter<T, decltype(std::declval<typename T::iterator>()[0], void())>{ static const bool value = true; };

	template <class C>
	void Erase(C& container, typename sequence_container_traits<C>::value_type const& t)
	{
		container.erase(std::remove(std::begin(container), std::end(container), t), std::end(container));
	}
	template <class C>
	void Erase(C& container, typename associative_container_traits<C>::value_type const& t)
	{
		container.erase(t);
	}
	template <class C>
	void Erase(C& container, typename hash_container_traits<C>::value_type const& t)
	{
		container.erase(t);
	}

	template <class C, class F, typename std::enable_if<sequence_container_traits<C>::exist>::type*& = enabler>
	void EraseIf(C& container, F const& remove_pred)
	{
		container.erase(std::remove_if(std::begin(container), std::end(container), remove_pred), std::end(container));
	}
	template <class C, class F, typename std::enable_if<associative_container_traits<C>::exist>::type*& = enabler>
	void EraseIf(C& container, F const& remove_pred)
	{
		for (auto it = std::begin(container), end = std::end(container); it != end;){
			if (remove_pred(*it)){
				it = container.erase(it);
			}
			else ++it;
		}
	}
	template <class C, class F, typename std::enable_if<hash_container_traits<C>::exist>::type*& = enabler>
	void EraseIf(C& container, F const& remove_pred)
	{
		for (auto it = std::begin(container), end = std::end(container); it != end;){
			if (remove_pred(*it)){
				it = container.erase(it);
			}
			else ++it;
		}
	}
	
}

#endif