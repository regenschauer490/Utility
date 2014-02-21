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

#ifndef SIG_EVAL_H
#define SIG_EVAL_H

#include <type_traits>
#include <utility>

template<
    class F, class... Args,
    class = typename std::enable_if<!std::is_member_function_pointer<F>::value>::type,
    class = typename std::enable_if<!std::is_member_object_pointer<F>::value>::type
    >
auto eval(F&& f, Args&&... args) -> decltype(f(std::forward<Args>(args)...))
{
    return f(std::forward<Args>(args)...);
}

template<class R, class C, class... Args>
auto eval(R(C::*f)() const, const C& c, Args&&... args) -> R
{
    return (c.*f)(std::forward<Args>(args)...);
}

template<class R, class C, class... Args>
auto eval(R(C::*f)() const, C& c, Args&&... args) -> R
{
    return (c.*f)(std::forward<Args>(args)...);
}

template<class R, class C, class... Args>
auto eval(R(C::*f)(), C& c, Args&&... args) -> R
{
    return (c.*f)(std::forward<Args>(args)...);
}

template<class R, class C>
auto eval(R(C::*m), const C& c) -> const R&
{
    return c.*m;
}

template<class R, class C>
auto eval(R(C::*m), C& c) -> R&
{
    return c.*m;
}

template<class... Args>
using eval_result_type = decltype(eval(std::declval<Args>()...));

#endif