/*
 * Copyright (c) 2013, Daniel Park
 * All rights reserved.
 *
 * Permission to modify and redistribute this software is granted to
 * anyone provided the above copyright notice, this condition and the
 * following disclaimer are retained.
 *
 * This software is provided "as is", without and express or implied
 * warranty. In no event shall the author be liable for damages arising
 * from the use of this software.
 */

/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_CONTAINER_TRAITS_H
#define SIG_CONTAINER_TRAITS_H

#include <vector>
#include <deque>
#include <list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <initializer_list>

#include "../container/array.hpp"


/** \page sig_container 対応コンテナ
\verbatim
	　[ STL ]
	<static_container>
	・array

	<sequence_container>
	・vector
	・deque
	・list

	<associative_container>
	・set
	・multiset
	・map
	・multimap

	<hash_container>
	・unordered_set
	・unordered_multiset
	・unordered_map
	・unordered_multimap

	　[ SigUtil ]
	<static_container>
	・array

	※その他コンテナもクラスtemplateの特殊化を定義することで利用可能
\endverbatim
*/

namespace sig
{
namespace impl
{

template <class C>
struct container_traits
{
	static const bool exist = false;
};


/// static_container_traits

template<class C>
struct static_container_traits
{
	static const bool exist = false;
};

template<template<class, size_t> class C, class T, size_t N>
struct static_container_traits<C<T, N>>
{
	static const bool exist = true;

	using value_type = T;

	static const size_t size = N;	// only static_container

	template<class U>
	using rebind = C<U, N>;

	template<class U, size_t M>
	using rebind_t = C<U, M>;		// only static_container

	template<size_t M>
	using rebind_n = C<T, M>;		// only static_container


	static C<T, N> make(size_t n){ return C<T, N>(); }

	static void add_element(C<T, N>& c, T const& t)
	{
		c.push_back(t);
	}
	static void add_element(C<T, N>& c, T&& t)
	{
		c.push_back(std::move(t));
	}
};

template<class T, size_t N>
struct container_traits<std::array<T, N>> : public static_container_traits<std::array<T, N>>
{};

template<class T, size_t N>
struct container_traits<sig::array<T, N>> : public static_container_traits<sig::array<T, N>>
{};

/*
/// temp_container_traits

template<class C>
struct temp_container_traits;

template<template<class> class C, class T>
struct temp_container_traits<C<T>>
{
	static const bool exist = true;
	
	using value_type = T;

	template<class U>
	using rebind = C<U>;
};

template<class T>
struct container_traits<std::initializer_list<T>> : public temp_container_traits<std::initializer_list<T>>
{};
*/

/// sequence_container_traits

template<class C>
struct sequence_container_traits;

template<template<class, class> class C, class T, class A>
struct sequence_container_traits<C<T, A>>
{
	static const bool exist = true;

	using value_type = T;

	template<class U>
	using rebind = C<U, typename A::template rebind<U>::other>;


	static C<T, A> make(size_t n){ return C<T, A>(); }	// default implementation

	static void add_element(C<T, A>& c, T const& t)
	{
		c.push_back(t);
	}
	static void add_element(C<T, A>& c, T&& t)
	{
		c.push_back(std::move(t));
	}

	static void concat(C<T, A>& lhs, C<T, A> const& rhs)
	{
		lhs.insert(lhs.end(), rhs.begin(), rhs.end());
	}
	static void concat(C<T, A>& lhs, C<T, A>&& rhs)
	{
		lhs.insert(lhs.end(), std::make_move_iterator(rhs.begin()), std::make_move_iterator(rhs.end()));
	}
};

template<class... Args>
struct container_traits<std::vector<Args...>> : public sequence_container_traits<std::vector<Args...>>
{
	static std::vector<Args...> make(size_t n){ std::vector<Args...> tmp; tmp.reserve(n); return tmp; }
};

template<class... Args>
struct container_traits<std::deque<Args...>> : public sequence_container_traits<std::deque<Args...>>
{};

template<class... Args>
struct container_traits<std::list<Args...>> : public sequence_container_traits<std::list<Args...>>
{};


/// associative_container_traits

template<class C>
struct associative_container_traits;

// for set
template<template<class, class, class> class C, class T, template<class> class O, class A>
struct associative_container_traits<C<T, O<T>, A>>
{
	static const bool exist = true;

	using value_type = T;

	template<class U>
	using rebind = C<U, O<U>, typename A::template rebind<U>::other>;


	static C<T, O<T>, A> make(size_t n){ return C<T, O<T>, A>(); }	// default implementation

	static void add_element(C<T, O<T>, A>& c, T const& t)
	{
		c.insert(t);
	}
	static void add_element(C<T, O<T>, A>& c, T&& t)
	{
		c.insert(std::move(t));
	}

	static void concat(C<T, O<T>, A>& lhs, C<T, O<T>, A> const& rhs)
	{
		lhs.insert(rhs.begin(), rhs.end());
	}
	static void concat(C<T, O<T>, A>& lhs, C<T, O<T>, A>&& rhs)
	{
		lhs.insert(std::make_move_iterator(rhs.begin()), std::make_move_iterator(rhs.end()));
	}
};

// for map
template<template<class, class, class, class> class C, class K, class T, template<class> class O, class A>
struct associative_container_traits<C<K, T, O<K>, A>>
{
	static const bool exist = true;

	using value_type = std::pair<const K, T>;

	template<class U, class V>
	using rebind = C<U, V, O<U>, typename A::template rebind<std::pair<U, V>>::other>;


	static C<K, T, O<K>, A> make(size_t n){ return C<K, T, O<K>, A>(); }	// default implementation

	template<class P>
	static void add_element(C<K, T, O<K>, A>& c, P&& pair)
	{
		c.insert(std::forward<P>(pair));
	}

	static void concat(C<K, T, O<K>, A>& lhs, C<K, T, O<K>, A> const& rhs)
	{
		lhs.insert(rhs.begin(), rhs.end());
	}
	static void concat(C<K, T, O<K>, A>& lhs, C<K, T, O<K>, A>&& rhs)
	{
		lhs.insert(std::make_move_iterator(rhs.begin()), std::make_move_iterator(rhs.end()));
	}
};

template<class... Args>
struct container_traits<std::multiset<Args...>> : public associative_container_traits<std::multiset<Args...>>
{};

template<class... Args>
struct container_traits<std::set<Args...>> : public associative_container_traits<std::set<Args...>>
{};

#if SIG_MSVC_ENV
template<class K, class T, class... Args>
struct container_traits<std::map<K, T, Args...>> : public associative_container_traits<std::map<K, T, Args...>>
{};

template<class K, class T, class... Args>
struct container_traits<std::multimap<K, T, Args...>> : public associative_container_traits<std::multimap<K, T, Args...>>
{};

#else
template<class... Args>
struct container_traits<std::map<Args...>> : public associative_container_traits<std::map<Args...>>
{};

template<class... Args>
struct container_traits<std::multimap<Args...>> : public associative_container_traits<std::multimap<Args...>>
{};
#endif


/// hash_container_traits

template<class C>
struct hash_container_traits;

// for set
template<template<class, class, class, class> class C, class T, template<class> class H, template<class> class O, class A>
struct hash_container_traits<C<T, H<T>, O<T>, A>>
{
	static const bool exist = true;

	using value_type = T;

	template<class U>
	using rebind = C<U, H<U>, O<U>, typename A::template rebind<U>::other>;


	static C<T, H<T>, O<T>, A> make(size_t n){
		//C<T, H<T>, O<T>, A> tmp; tmp.reserve(n); return tmp;	// performace varies by value condition (added value distribution)
		return C<T, H<T>, O<T>, A>();
	}

	static void add_element(C<T, H<T>, O<T>, A>& c, T const& t)
	{
		c.insert(t);
	}
	static void add_element(C<T, H<T>, O<T>, A>& c, T&& t)
	{
		c.insert(std::move(t));
	}

	static void concat(C<T, H<T>, O<T>, A>& lhs, C<T, H<T>, O<T>, A> const& rhs)
	{
		lhs.insert(rhs.begin(), rhs.end());
	}
	static void concat(C<T, H<T>, O<T>, A>& lhs, C<T, H<T>, O<T>, A>&& rhs)
	{
		lhs.insert(std::make_move_iterator(rhs.begin()), std::make_move_iterator(rhs.end()));
	}
};

template<template<class, class, class, class, class> class C, class K, class T, template<class> class H, template<class> class O, class A>
struct hash_container_traits<C<K, T, H<K>, O<K>, A>>
{
	static const bool exist = true;

	using value_type = std::pair<const K, T>;

	template<class U, class V>
	using rebind = C<U, V, H<U>, O<U>, typename A::template rebind<std::pair<U, V>>::other>;


	static C<K, T, H<K>, O<K>, A> make(size_t n){
		//C<K, T, H<K>, O<K>, A> tmp; tmp.reserve(n); return tmp;	// performace varies by value condition (added value distribution)
		return C<K, T, H<K>, O<K>, A>();
	}

	template<class P>
	static void add_element(C<K, T, H<K>, O<K>, A>& c, P&& pair)
	{
		c.insert(std::forward<P>(pair));
	}

	static void concat(C<K, T, H<K>, O<K>, A>& lhs, C<K, T, H<K>, O<K>, A> const& rhs)
	{
		lhs.insert(rhs.begin(), rhs.end());
	}
	static void concat(C<K, T, H<K>, O<K>, A>& lhs, C<K, T, H<K>, O<K>, A>&& rhs)
	{
		lhs.insert(std::make_move_iterator(rhs.begin()), std::make_move_iterator(rhs.end()));
	}
};

template<class... Args>
struct container_traits<std::unordered_set<Args...>> : public hash_container_traits<std::unordered_set<Args...>>
{};

template<class... Args>
struct container_traits<std::unordered_multiset<Args...>> : public hash_container_traits<std::unordered_multiset<Args...>>
{};

#if SIG_MSVC_ENV
template<class K, class T, class... Args>
struct container_traits<std::unordered_map<K, T, Args...>> : public hash_container_traits<std::unordered_map<K, T, Args...>>
{};

template<class K, class T, class... Args>
struct container_traits<std::unordered_multimap<K, T, Args...>> : public hash_container_traits<std::unordered_multimap<K, T, Args...>>
{};

#else
template<class... Args>
struct container_traits<std::unordered_map<Args...>> : public hash_container_traits<std::unordered_map<Args...>>
{};

template<class... Args>
struct container_traits<std::unordered_multimap<Args...>> : public hash_container_traits<std::unordered_multimap<Args...>>
{};
#endif


/*
// basic_string
template<class T, template<class> class K, class A>
struct container_traits<std::basic_string<T,K<T>,A>>
{
	static const bool exist = true;
	
    using value_type = T;

    template<class U>
    using rebind = std::basic_string<U,K<U>,typename A::template rebind<U>::other>;

    static void add_element(std::basic_string<T,K<T>,A>& c, const T& t)
    {
        c.push_back(t);
    }

    static void concat(std::basic_string<T,K<T>,A>& lhs, const std::basic_string<T,K<T>,A>& rhs)
    {
        lhs+=rhs;
    }
};
*/

}
}
#endif
