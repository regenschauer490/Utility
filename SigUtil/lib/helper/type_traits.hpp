/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef _SIG_UTIL_TYPE_TRAITS_HPP
#define _SIG_UTIL_TYPE_TRAITS_HPP

#include "../sigutil.hpp"
#include <iterator>
#include <type_traits>

namespace sig
{
namespace impl
{

#define CREATE_MEMBER_DETECTOR(X)                                                   \
template<typename T> class Detect_##X {                                             \
    struct Fallback { int X; };                                                     \
    struct Derived : T, Fallback { };                                               \
                                                                                    \
    template<typename U, U> struct Check;                                           \
                                                                                    \
    typedef char ArrayOfOne[1];                                                     \
    typedef char ArrayOfTwo[2];                                                     \
                                                                                    \
    template<typename U> static ArrayOfOne & func(Check<int Fallback::*, &U::X> *); \
    template<typename U> static ArrayOfTwo & func(...);                             \
  public:                                                                           \
    typedef Detect_##X type;                                                        \
    enum { value = sizeof(func<Derived>(0)) == 2 };                                 \
};

template <class T>
struct is_const
{
	static const bool value = std::is_const<typename std::remove_reference<T>::type>::value;
};


template <class C>
struct has_iterator
{
	CREATE_MEMBER_DETECTOR(iterator);
	static const bool value = Detect_iterator<C>::value;
};

#if !(SIG_MSVC_VER < 120)
template <class C>
struct has_random_access_iter
{
	template<class D, bool b> struct Sel{ using type = void; };
	template<class D> struct Sel<D, true>{ using type = typename std::iterator_traits<typename D::iterator>::iterator_category;  };

	template <class D>
	struct impl_{
		static const bool v = std::is_same<D, std::random_access_iterator_tag>::value;
	};

	static const bool value = impl_<typename Sel<C, has_iterator<C>::value>::type>::v;
};
#endif

//template <class T, class D = void> struct has_random_access_iter{ static const bool value = false; };
//template <class T> struct has_random_access_iter<T, decltype(std::declval<typename T::iterator>()[0], void())>{ static const bool value = true; };


}	// impl

}	// sig
#endif
