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
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <initializer_list>

#include "eval.hpp"
#include "../array.hpp"


namespace sig{

template <class C>
struct container_traits
{
	static const bool exist = false;
	// Type value_type
	// Type rebind<U>
	// void add_element(C&,T)
	// void concat(C&,C)
};


/// static_container_traits

template<class C>
struct static_container_traits;

template<template<class, size_t> class C, class T, size_t N>
struct static_container_traits<C<T, N>>
{
	static const bool exist = true;
	
	using value_type = T;

	template<class U>
	using rebind = C<U, N>;

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
struct container_traits<const std::array<T, N>> : public static_container_traits<std::array<T, N>>
{};

template<class T, size_t N>
struct container_traits<sig::array<T, N>> : public static_container_traits<sig::array<T, N>>
{};
template<class T, size_t N>
struct container_traits<const sig::array<T, N>> : public static_container_traits<sig::array<T, N>>
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

template<template<class,class> class C, class T, class A>
struct sequence_container_traits<C<T,A>>
{
	static const bool exist = true;
	
	using value_type = T;

	template<class U>
	using rebind = C<U,typename A::template rebind<U>::other>;

	static void add_element(C<T,A>& c, T const& t)
	{
		c.push_back(t);
	}
	static void add_element(C<T, A>& c, T&& t)
	{
		c.push_back(std::move(t));
	}

	static void concat(C<T,A>& lhs, C<T,A> const& rhs)
	{
		lhs.insert(lhs.end(),rhs.begin(),rhs.end());
	}
	static void concat(C<T, A>& lhs, C<T, A>&& rhs)
	{
		lhs.insert(lhs.end(), std::make_move_iterator(rhs.begin()), std::make_move_iterator(rhs.end()));
	}
};

template<class... Args>
struct container_traits<std::vector<Args...>> : public sequence_container_traits<std::vector<Args...>>
{};
template<class... Args>
struct container_traits<const std::vector<Args...>> : public sequence_container_traits<std::vector<Args...>>
{};

template<class... Args>
struct container_traits<std::deque<Args...>> : public sequence_container_traits<std::deque<Args...>>
{};
template<class... Args>
struct container_traits<const std::deque<Args...>> : public sequence_container_traits<std::deque<Args...>>
{};

template<class... Args>
struct container_traits<std::list<Args...>> : public sequence_container_traits<std::list<Args...>>
{};
template<class... Args>
struct container_traits<const std::list<Args...>> : public sequence_container_traits<std::list<Args...>>
{};


/// associative_container_traits

template<class C>
struct associative_container_traits;

template<template<class,class,class> class C, class T, template<class> class O, class A>
struct associative_container_traits<C<T,O<T>,A>>
{
	static const bool exist = true;
	
	using value_type = T;

	template<class U>
	using rebind = C<U,O<U>,typename A::template rebind<U>::other>;

	static void add_element(C<T,O<T>,A>& c, T const& t)
	{
		c.insert(t);
	}
	static void add_element(C<T, O<T>, A>& c, T&& t)
	{
		c.insert(std::move(t));
	}


	static void concat(C<T,O<T>,A>& lhs, C<T,O<T>,A> const& rhs)
	{
		lhs.insert(rhs.begin(),rhs.end());
	}
	static void concat(C<T, O<T>, A>& lhs, C<T, O<T>, A>&& rhs)
	{
		lhs.insert(std::make_move_iterator(rhs.begin()), std::make_move_iterator(rhs.end()));
	}
};

template<template<class,class,class,class> class C, class K, class T, template<class> class O, class A>
struct associative_container_traits<C<K,T,O<K>,A>>
{
	static const bool exist = true;
	
	using value_type = std::pair<const K,T>;
};

template<class... Args>
struct container_traits<std::multiset<Args...>> : public associative_container_traits<std::multiset<Args...>>
{};
template<class... Args>
struct container_traits<const std::multiset<Args...>> : public associative_container_traits<std::multiset<Args...>>
{};

template<class... Args>
struct container_traits<std::set<Args...>> : public associative_container_traits<std::set<Args...>>
{};
template<class... Args>
struct container_traits<const std::set<Args...>> : public associative_container_traits<std::set<Args...>>
{};

#if SIG_MSVC_ENV
template<class K, class T, class... Args>
struct container_traits<std::map<K, T, Args...>> : public associative_container_traits<std::map<K, T, Args...>>
{};	// only ::value_type
template<class K, class T, class... Args>
struct container_traits<const std::map<K, T, Args...>> : public associative_container_traits<std::map<K, T, Args...>>
{};	// only ::value_type

template<class K, class T, class... Args>
struct container_traits<std::multimap<K, T, Args...>> : public associative_container_traits<std::multimap<K, T, Args...>>
{};	// only ::value_type
template<class K, class T, class... Args>
struct container_traits<const std::multimap<K, T, Args...>> : public associative_container_traits<std::multimap<K, T, Args...>>
{};	// only ::value_type

#else
template<class... Args>
struct container_traits<std::map<Args...>> : public associative_container_traits<std::map<Args...>>
{};	// only ::value_type
template<class... Args>
struct container_traits<const std::map<Args...>> : public associative_container_traits<std::map<Args...>>
{};	// only ::value_type

template<class... Args>
struct container_traits<std::multimap<Args...>> : public associative_container_traits<std::multimap<Args...>>
{};	// only ::value_type
template<class... Args>
struct container_traits<const std::multimap<Args...>> : public associative_container_traits<std::multimap<Args...>>
{};	// only ::value_type
#endif


/// hash_container_traits

template<class C>
struct hash_container_traits;

template<template<class,class,class,class> class C, class T, template<class> class H, template<class> class O, class A>
struct hash_container_traits<C<T,H<T>,O<T>,A>>
{
	static const bool exist = true;
	
	using value_type = T;

	template<class U>
	using rebind = C<U,H<U>,O<U>,typename A::template rebind<U>::other>;

	static void add_element(C<T,H<T>,O<T>,A>& c, T const& t)
	{
		c.insert(t);
 	}
	static void add_element(C<T, H<T>, O<T>, A>& c, T&& t)
	{
		c.insert(std::move(t));
	}

	static void concat(C<T,H<T>,O<T>,A>& lhs, C<T,H<T>,O<T>,A> const& rhs)
	{
		lhs.insert(rhs.begin(),rhs.end());
	}
	static void concat(C<T, H<T>, O<T>, A>& lhs, C<T, H<T>, O<T>, A>&& rhs)
	{
		lhs.insert(std::make_move_iterator(rhs.begin()), std::make_move_iterator(rhs.end()));
	}
};

template<template<class,class,class,class,class> class C, class K, class T, template<class> class H, template<class> class O, class A>
struct hash_container_traits<C<K,T,H<K>,O<K>,A>>
{
	static const bool exist = true;
	
	using value_type = std::pair<const K,T>;
};

template<class... Args>
struct container_traits<std::unordered_multiset<Args...>> : public hash_container_traits<std::unordered_multiset<Args...>>
{};
template<class... Args>
struct container_traits<const std::unordered_multiset<Args...>> : public hash_container_traits<std::unordered_multiset<Args...>>
{};

template<class... Args>
struct container_traits<std::unordered_set<Args...>> : public hash_container_traits<std::unordered_set<Args...>>
{};
template<class... Args>
struct container_traits<const std::unordered_set<Args...>> : public hash_container_traits<std::unordered_set<Args...>>
{};

#if SIG_MSVC_ENV
template<class K, class T, class... Args>
struct container_traits<std::unordered_map<K, T, Args...>> : public hash_container_traits<std::unordered_map<K, T, Args...>>
{};	// only ::value_type
template<class K, class T, class... Args>
struct container_traits<const std::unordered_map<K, T, Args...>> : public hash_container_traits<std::unordered_map<K, T, Args...>>
{};	// only ::value_type

template<class K, class T, class... Args>
struct container_traits<std::unordered_multimap<K, T, Args...>> : public hash_container_traits<std::unordered_multimap<K, T, Args...>>
{};	// only ::value_type
template<class K, class T, class... Args>
struct container_traits<const std::unordered_multimap<K, T, Args...>> : public hash_container_traits<std::unordered_multimap<K, T, Args...>>
{};	// only ::value_type

#else
template<class... Args>
struct container_traits<std::unordered_map<Args...>> : public hash_container_traits<std::unordered_map<Args...>>
{};	// only ::value_type
template<class... Args>
struct container_traits<const std::unordered_map<Args...>> : public hash_container_traits<std::unordered_map<Args...>>
{};	// only ::value_type

template<class... Args>
struct container_traits<std::unordered_multimap<Args...>> : public hash_container_traits<std::unordered_multimap<Args...>>
{};	// only ::value_type
template<class... Args>
struct container_traits<const std::unordered_multimap<Args...>> : public hash_container_traits<std::unordered_multimap<Args...>>
{};	// only ::value_type
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

/*
namespace ct
{
	template <class T>
	struct is_valid : public std::false_type
	{};

	template <class T>
	struct is_valid<container_traits<T>> : public std::true_type
	{};

	template <class T>
	struct is_sequence : public std::false_type
	{};

	template <class T>
	struct is_sequence<static_container_traits<T>> : public std::true_type
	{};

	template <class T>
	struct is_sequence<sequence_container_traits<T>> : public std::true_type
	{};


} //ct
*/

}
#endif
