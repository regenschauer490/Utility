/*
The MIT License(MIT)

Copyright(c) 2014 Akihiro Nishimura

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files(the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and / or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef __SIG_C_TRAITS_H__
#define __SIG_C_TRAITS_H__


#include "../external/container traits/container_traits.h"

namespace sig
{
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
	auto DereferenceIterator(It& iter) ->decltype(*iter)
	{
		return *iter;
	}

	template <class C>
	void Erase(C& container, typename sequence_container_traits<C>::value_type const& t)
	{
		container.erase(std::remove(std::begin(container), std::end(container), t), std::end(container));
	}
	template <class C>
	void EraseIf(C& container, std::function<typename std::common_type<bool>::type(typename sequence_container_traits<C>::value_type)> const& remove_pred)
	{
		container.erase(std::remove_if(std::begin(container), std::end(container), remove_pred), std::end(container));
	}

	template <class C>
	void Erase(C& container, typename associative_container_traits<C>::value_type const& t)
	{
		container.erase(t);
	}
	template <class C>
	void EraseIf(C& container, std::function<typename std::common_type<bool>::type(typename associative_container_traits<C>::value_type)> const& remove_pred)
	{
		for (auto it = std::begin(container), end = std::end(container); it != end;){
			if (remove_pred(*it)){
				it = container.erase(it);
			}
			else ++it;
		}
	}

	template <class C>
	void Erase(C& container, typename hash_container_traits<C>::value_type const& t)
	{
		container.erase(t);
	}
	template <class C>
	void EraseIf(C& container, std::function<typename std::common_type<bool>::type(typename hash_container_traits<C>::value_type)> const& remove_pred)
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