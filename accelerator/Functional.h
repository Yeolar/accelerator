/*
 * Copyright 2017 Facebook, Inc.
 * Copyright 2017-present Yeolar
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <functional>

#include "accelerator/Traits.h"

namespace acc {

/**
 *  include or backport:
 *  * std::invoke
 *  * std::invoke_result
 *  * std::invoke_result_t
 *  * std::is_invocable
 *  * std::is_invocable_r
 *  * std::is_nothrow_invocable
 *  * std::is_nothrow_invocable_r
 */

//  mimic: std::invoke, C++17
template <typename F, typename... Args>
constexpr auto invoke(F&& f, Args&&... args) noexcept(
    noexcept(static_cast<F&&>(f)(static_cast<Args&&>(args)...)))
    -> decltype(static_cast<F&&>(f)(static_cast<Args&&>(args)...)) {
  return static_cast<F&&>(f)(static_cast<Args&&>(args)...);
}
template <typename M, typename C, typename... Args>
constexpr auto invoke(M(C::*d), Args&&... args)
    -> decltype(std::mem_fn(d)(static_cast<Args&&>(args)...)) {
  return std::mem_fn(d)(static_cast<Args&&>(args)...);
}

namespace invoke_detail {

template <typename F, typename... Args>
using invoke_result_ =
    decltype(invoke(std::declval<F>(), std::declval<Args>()...));

template <typename F, typename... Args>
struct invoke_nothrow_
    : bool_constant<noexcept(
          invoke(std::declval<F>(), std::declval<Args>()...))> {};

//  from: http://en.cppreference.com/w/cpp/types/result_of, CC-BY-SA

template <typename Void, typename F, typename... Args>
struct invoke_result {};

template <typename F, typename... Args>
struct invoke_result<void_t<invoke_result_<F, Args...>>, F, Args...> {
  using type = invoke_result_<F, Args...>;
};

template <typename Void, typename F, typename... Args>
struct is_invocable : std::false_type {};

template <typename F, typename... Args>
struct is_invocable<void_t<invoke_result_<F, Args...>>, F, Args...>
    : std::true_type {};

template <typename Void, typename R, typename F, typename... Args>
struct is_invocable_r : std::false_type {};

template <typename R, typename F, typename... Args>
struct is_invocable_r<void_t<invoke_result_<F, Args...>>, R, F, Args...>
    : std::is_convertible<invoke_result_<F, Args...>, R> {};

template <typename Void, typename F, typename... Args>
struct is_nothrow_invocable : std::false_type {};

template <typename F, typename... Args>
struct is_nothrow_invocable<void_t<invoke_result_<F, Args...>>, F, Args...>
    : invoke_nothrow_<F, Args...> {};

template <typename Void, typename R, typename F, typename... Args>
struct is_nothrow_invocable_r : std::false_type {};

template <typename R, typename F, typename... Args>
struct is_nothrow_invocable_r<void_t<invoke_result_<F, Args...>>, R, F, Args...>
    : StrictConjunction<
        std::is_convertible<invoke_result_<F, Args...>, R>,
        invoke_nothrow_<F, Args...>> {};

} // namespace detail

//  mimic: std::invoke_result, C++17
template <typename F, typename... Args>
struct invoke_result : invoke_detail::invoke_result<void, F, Args...> {};

//  mimic: std::invoke_result_t, C++17
template <typename F, typename... Args>
using invoke_result_t = typename invoke_result<F, Args...>::type;

//  mimic: std::is_invocable, C++17
template <typename F, typename... Args>
struct is_invocable : invoke_detail::is_invocable<void, F, Args...> {};

//  mimic: std::is_invocable_r, C++17
template <typename R, typename F, typename... Args>
struct is_invocable_r : invoke_detail::is_invocable_r<void, R, F, Args...> {};

//  mimic: std::is_nothrow_invocable, C++17
template <typename F, typename... Args>
struct is_nothrow_invocable
    : invoke_detail::is_nothrow_invocable<void, F, Args...> {};

//  mimic: std::is_nothrow_invocable_r, C++17
template <typename R, typename F, typename... Args>
struct is_nothrow_invocable_r
    : invoke_detail::is_nothrow_invocable_r<void, R, F, Args...> {};

} // namespace acc

namespace acc {
namespace detail {

template <typename Invoke>
struct member_invoke_proxy {
 public:
  template <typename O, typename... Args>
  struct invoke_result : acc::invoke_result<Invoke, O, Args...> {};
  template <typename O, typename... Args>
  using invoke_result_t = acc::invoke_result_t<Invoke, O, Args...>;
  template <typename O, typename... Args>
  struct is_invocable : acc::is_invocable<Invoke, O, Args...> {};
  template <typename R, typename O, typename... Args>
  struct is_invocable_r : acc::is_invocable_r<R, Invoke, O, Args...> {};
  template <typename O, typename... Args>
  struct is_nothrow_invocable
      : acc::is_nothrow_invocable<Invoke, O, Args...> {};
  template <typename R, typename O, typename... Args>
  struct is_nothrow_invocable_r
      : acc::is_nothrow_invocable_r<R, Invoke, O, Args...> {};

  template <typename O, typename... Args>
  static constexpr auto invoke(O&& o, Args&&... args) noexcept(
      noexcept(Invoke{}(static_cast<O&&>(o), static_cast<Args&&>(args)...)))
      -> decltype(Invoke{}(static_cast<O&&>(o), static_cast<Args&&>(args)...)) {
    return Invoke{}(static_cast<O&&>(o), static_cast<Args&&>(args)...);
  }

  using invoke_type = Invoke;
};

} // namespace detail
} // namespace acc

/***
 *  ACC_CREATE_MEMBER_INVOKE_TRAITS
 *
 *  Used to create traits container, bound to a specific member-invocable name,
 *  with the following member traits types and aliases:
 *
 *  * invoke_result
 *  * invoke_result_t
 *  * is_invocable
 *  * is_invocable_r
 *  * is_nothrow_invocable
 *  * is_nothrow_invocable_r
 *
 *  The container also has a static member function:
 *
 *  * invoke
 *
 *  And a member type alias:
 *
 *  * invoke_type
 *
 *  These members have behavior matching the behavior of C++17's corresponding
 *  invocation traits types, aliases, and functions, but substituting canonical
 *  invocation with member invocation.
 *
 *  Example:
 *
 *    ACC_CREATE_MEMBER_INVOKE_TRAITS(foo_invoke_traits, foo);
 *
 *  The traits container type `foo_invoke_traits` is generated in the current
 *  namespace and has the listed member types and aliases. They may be used as
 *  follows:
 *
 *    struct CanFoo {
 *      int foo(Bar&) { return 1; }
 *      int foo(Car&&) noexcept { return 2; }
 *    };
 *
 *    using traits = foo_invoke_traits;
 *
 *    traits::invoke(CanFoo{}, Car{}) // 2
 *
 *    traits::invoke_result<CanFoo, Bar&> // has member
 *    traits::invoke_result_t<CanFoo, Bar&> // int
 *    traits::invoke_result<CanFoo, Bar&&> // empty
 *    traits::invoke_result_t<CanFoo, Bar&&> // error
 *
 *    traits::is_invocable<CanFoo, Bar&>::value // true
 *    traits::is_invocable<CanFoo, Bar&&>::value // false
 *
 *    traits::is_invocable_r<int, CanFoo, Bar&>::value // true
 *    traits::is_invocable_r<char*, CanFoo, Bar&>::value // false
 *
 *    traits::is_nothrow_invocable<CanFoo, Bar&>::value // false
 *    traits::is_nothrow_invocable<CanFoo, Car&&>::value // true
 *
 *    traits::is_nothrow_invocable<int, CanFoo, Bar&>::value // false
 *    traits::is_nothrow_invocable<char*, CanFoo, Bar&>::value // false
 *    traits::is_nothrow_invocable<int, CanFoo, Car&&>::value // true
 *    traits::is_nothrow_invocable<char*, CanFoo, Car&&>::value // false
 */
#define ACC_CREATE_MEMBER_INVOKE_TRAITS(classname, membername)                \
  struct classname##__acc_detail_member_invoke {                              \
    template <typename O, typename... Args>                                   \
    constexpr auto operator()(O&& o, Args&&... args) const noexcept(noexcept( \
        static_cast<O&&>(o).membername(static_cast<Args&&>(args)...)))        \
        -> decltype(                                                          \
            static_cast<O&&>(o).membername(static_cast<Args&&>(args)...)) {   \
      return static_cast<O&&>(o).membername(static_cast<Args&&>(args)...);    \
    }                                                                         \
  };                                                                          \
  struct classname : ::acc::detail::member_invoke_proxy<                      \
                         classname##__acc_detail_member_invoke> {}
