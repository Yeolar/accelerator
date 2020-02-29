/*
 * Copyright 2011-present Facebook, Inc.
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

#include <limits>
#include <type_traits>

#define ACC_CREATE_HAS_MEMBER_TYPE_TRAITS(classname, type_name)                \
  template <typename TTheClass_>                                               \
  struct classname##__acc_traits_impl__ {                                      \
    template <typename UTheClass_>                                             \
    static constexpr bool test(typename UTheClass_::type_name*) {              \
      return true;                                                             \
    }                                                                          \
    template <typename>                                                        \
    static constexpr bool test(...) {                                          \
      return false;                                                            \
    }                                                                          \
  };                                                                           \
  template <typename TTheClass_>                                               \
  using classname = typename std::conditional<                                 \
      classname##__acc_traits_impl__<TTheClass_>::template test<TTheClass_>(   \
          nullptr),                                                            \
      std::true_type,                                                          \
      std::false_type>::type

#define ACC_CREATE_HAS_MEMBER_FN_TRAITS_IMPL(classname, func_name, cv_qual)   \
  template <typename TTheClass_, typename RTheReturn_, typename... TTheArgs_> \
  struct classname##__acc_traits_impl__<                                      \
      TTheClass_,                                                             \
      RTheReturn_(TTheArgs_...) cv_qual> {                                    \
    template <                                                                \
        typename UTheClass_,                                                  \
        RTheReturn_ (UTheClass_::*)(TTheArgs_...) cv_qual>                    \
    struct sfinae {};                                                         \
    template <typename UTheClass_>                                            \
    static std::true_type test(sfinae<UTheClass_, &UTheClass_::func_name>*);  \
    template <typename>                                                       \
    static std::false_type test(...);                                         \
  }

/*
 * The ACC_CREATE_HAS_MEMBER_FN_TRAITS is used to create traits
 * classes that check for the existence of a member function with
 * a given name and signature. It currently does not support
 * checking for inherited members.
 *
 * Such classes receive two template parameters: the class to be checked
 * and the signature of the member function. A static boolean field
 * named `value` (which is also constexpr) tells whether such member
 * function exists.
 *
 * Each traits class created is bound only to the member name, not to
 * its signature nor to the type of the class containing it.
 *
 * Say you need to know if a given class has a member function named
 * `test` with the following signature:
 *
 *    int test() const;
 *
 * You'd need this macro to create a traits class to check for a member
 * named `test`, and then use this traits class to check for the signature:
 *
 * namespace {
 *
 * ACC_CREATE_HAS_MEMBER_FN_TRAITS(has_test_traits, test);
 *
 * } // unnamed-namespace
 *
 * void some_func() {
 *   cout << "Does class Foo have a member int test() const? "
 *     << boolalpha << has_test_traits<Foo, int() const>::value;
 * }
 *
 * You can use the same traits class to test for a completely different
 * signature, on a completely different class, as long as the member name
 * is the same:
 *
 * void some_func() {
 *   cout << "Does class Foo have a member int test()? "
 *     << boolalpha << has_test_traits<Foo, int()>::value;
 *   cout << "Does class Foo have a member int test() const? "
 *     << boolalpha << has_test_traits<Foo, int() const>::value;
 *   cout << "Does class Bar have a member double test(const string&, long)? "
 *     << boolalpha << has_test_traits<Bar, double(const string&, long)>::value;
 * }
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
#define ACC_CREATE_HAS_MEMBER_FN_TRAITS(classname, func_name)                 \
  template <typename, typename>                                               \
  struct classname##__acc_traits_impl__;                                      \
  ACC_CREATE_HAS_MEMBER_FN_TRAITS_IMPL(classname, func_name, );               \
  ACC_CREATE_HAS_MEMBER_FN_TRAITS_IMPL(classname, func_name, const);          \
  ACC_CREATE_HAS_MEMBER_FN_TRAITS_IMPL(                                       \
      classname, func_name, /* nolint */ volatile);                           \
  ACC_CREATE_HAS_MEMBER_FN_TRAITS_IMPL(                                       \
      classname, func_name, /* nolint */ volatile const);                     \
  template <typename TTheClass_, typename TTheSignature_>                     \
  using classname =                                                           \
      decltype(classname##__acc_traits_impl__<TTheClass_, TTheSignature_>::   \
                   template test<TTheClass_>(nullptr))

namespace acc {

//  mimic: std::bool_constant, C++17
template <bool B>
using bool_constant = std::integral_constant<bool, B>;

template <std::size_t I>
using index_constant = std::integral_constant<std::size_t, I>;

template <typename T>
using _t = typename T::type;

namespace traits_detail {
template <class T, class...>
struct type_t_ {
  using type = T;
};
} // namespace traits_detail

template <class T, class... Ts>
using type_t = typename traits_detail::type_t_<T, Ts...>::type;
template <class... Ts>
using void_t = type_t<void, Ts...>;

// Older versions of libstdc++ do not provide std::is_trivially_copyable
#if defined(__clang__) && !defined(_LIBCPP_VERSION)
template <class T>
struct is_trivially_copyable : bool_constant<__is_trivially_copyable(T)> {};
#elif defined(__GNUC__) && !defined(__clang__) && __GNUC__ < 5
template <class T>
struct is_trivially_copyable : std::is_trivial<T> {};
#else
template <class T>
using is_trivially_copyable = std::is_trivially_copyable<T>;
#endif

/**
 * IsRelocatable<T>::value describes the ability of moving around
 * memory a value of type T by using memcpy (as opposed to the
 * conservative approach of calling the copy constructor and then
 * destroying the old temporary. Essentially for a relocatable type,
 * the following two sequences of code should be semantically
 * equivalent:
 *
 * void move1(T * from, T * to) {
 *   new(to) T(from);
 *   (*from).~T();
 * }
 *
 * void move2(T * from, T * to) {
 *   memcpy(to, from, sizeof(T));
 * }
 *
 * Most C++ types are relocatable; the ones that aren't would include
 * internal pointers or (very rarely) would need to update remote
 * pointers to pointers tracking them. All C++ primitive types and
 * type constructors are relocatable.
 *
 * This property can be used in a variety of optimizations. Currently
 * fbvector uses this property intensively.
 *
 * The default conservatively assumes the type is not
 * relocatable. Several specializations are defined for known
 * types. You may want to add your own specializations. Do so in
 * namespace acc and make sure you keep the specialization of
 * IsRelocatable<SomeStruct> in the same header as SomeStruct.
 *
 * You may also declare a type to be relocatable by including
 *    `typedef std::true_type IsRelocatable;`
 * in the class header.
 *
 * It may be unset in a base class by overriding the typedef to false_type.
 */
/*
 * IsZeroInitializable describes the property that default construction is the
 * same as memset(dst, 0, sizeof(T)).
 */

namespace traits_detail {

#define ACC_HAS_TRUE_XXX(name)                                               \
  ACC_CREATE_HAS_MEMBER_TYPE_TRAITS(has_##name, name);                       \
  template <class T>                                                         \
  struct name##_is_true : std::is_same<typename T::name, std::true_type> {}; \
  template <class T>                                                         \
  struct has_true_##name : std::conditional<                                 \
                               has_##name<T>::value,                         \
                               name##_is_true<T>,                            \
                               std::false_type>::type {}

ACC_HAS_TRUE_XXX(IsRelocatable);
ACC_HAS_TRUE_XXX(IsZeroInitializable);

#undef ACC_HAS_TRUE_XXX

} // namespace traits_detail

struct Ignore {
  Ignore() = default;
  template <class T>
  constexpr /* implicit */ Ignore(const T&) {}
  template <class T>
  const Ignore& operator=(T const&) const { return *this; }
};

template <class...>
using Ignored = Ignore;

namespace traits_detail_IsEqualityComparable {
Ignore operator==(Ignore, Ignore);

template <class T, class U = T>
struct IsEqualityComparable
    : std::is_convertible<
          decltype(std::declval<T>() == std::declval<U>()),
          bool
      > {};
} // namespace traits_detail_IsEqualityComparable

/* using override */ using traits_detail_IsEqualityComparable::
    IsEqualityComparable;

namespace traits_detail_IsLessThanComparable {
Ignore operator<(Ignore, Ignore);

template <class T, class U = T>
struct IsLessThanComparable
    : std::is_convertible<
          decltype(std::declval<T>() < std::declval<U>()),
          bool
      > {};
} // namespace traits_detail_IsLessThanComparable

/* using override */ using traits_detail_IsLessThanComparable::
    IsLessThanComparable;

namespace traits_detail_IsNothrowSwappable {
/* using override */ using std::swap;

template <class T>
struct IsNothrowSwappable
    : bool_constant<std::is_nothrow_move_constructible<T>::value&& noexcept(
          swap(std::declval<T&>(), std::declval<T&>()))> {};
} // namespace traits_detail_IsNothrowSwappable

/* using override */ using traits_detail_IsNothrowSwappable::IsNothrowSwappable;

template <class T>
struct IsRelocatable : std::conditional<
                           traits_detail::has_IsRelocatable<T>::value,
                           traits_detail::has_true_IsRelocatable<T>,
                           // TODO add this line (and some tests for it) when we
                           // upgrade to gcc 4.7
                           // std::is_trivially_move_constructible<T>::value ||
                           is_trivially_copyable<T>>::type {};

template <class T>
struct IsZeroInitializable
    : std::conditional<
          traits_detail::has_IsZeroInitializable<T>::value,
          traits_detail::has_true_IsZeroInitializable<T>,
          bool_constant<!std::is_class<T>::value>>::type {};

template <typename...>
struct Conjunction : std::true_type {};
template <typename T>
struct Conjunction<T> : T {};
template <typename T, typename... TList>
struct Conjunction<T, TList...>
    : std::conditional<T::value, Conjunction<TList...>, T>::type {};

template <typename...>
struct Disjunction : std::false_type {};
template <typename T>
struct Disjunction<T> : T {};
template <typename T, typename... TList>
struct Disjunction<T, TList...>
    : std::conditional<T::value, T, Disjunction<TList...>>::type {};

template <typename T>
struct Negation : bool_constant<!T::value> {};

template <bool... Bs>
struct Bools {
  using valid_type = bool;
  static constexpr std::size_t size() {
    return sizeof...(Bs);
  }
};

// Lighter-weight than Conjunction, but evaluates all sub-conditions eagerly.
template <class... Ts>
struct StrictConjunction
    : std::is_same<Bools<Ts::value...>, Bools<(Ts::value || true)...>> {};

template <class... Ts>
struct StrictDisjunction
  : Negation<
      std::is_same<Bools<Ts::value...>, Bools<(Ts::value && false)...>>
    > {};

// Is T one of T1, T2, ..., Tn?
template <typename T, typename... Ts>
using IsOneOf = StrictDisjunction<std::is_same<T, Ts>...>;

namespace detail {

template <typename T, bool>
struct is_negative_impl {
  constexpr static bool check(T x) { return x < 0; }
};

template <typename T>
struct is_negative_impl<T, false> {
  constexpr static bool check(T) { return false; }
};

template <typename RHS, RHS rhs, typename LHS>
bool less_than_impl(LHS const lhs) {
  return
    rhs > std::numeric_limits<LHS>::max() ? true :
    rhs <= std::numeric_limits<LHS>::min() ? false :
    lhs < rhs;
}

template <typename RHS, RHS rhs, typename LHS>
bool greater_than_impl(LHS const lhs) {
  return
    rhs > std::numeric_limits<LHS>::max() ? false :
    rhs < std::numeric_limits<LHS>::min() ? true :
    lhs > rhs;
}

}  // namespace detail

// same as `x < 0`
template <typename T>
constexpr bool is_negative(T x) {
  return acc::detail::is_negative_impl<T, std::is_signed<T>::value>::check(x);
}

// same as `x <= 0`
template <typename T>
constexpr bool is_non_positive(T x) { return !x || acc::is_negative(x); }

// same as `x > 0`
template <typename T>
constexpr bool is_positive(T x) { return !is_non_positive(x); }

// same as `x >= 0`
template <typename T>
constexpr bool is_non_negative(T x) {
  return !x || is_positive(x);
}

template <typename RHS, RHS rhs, typename LHS>
bool less_than(LHS const lhs) {
  return detail::less_than_impl<
    RHS, rhs, typename std::remove_reference<LHS>::type
  >(lhs);
}

template <typename RHS, RHS rhs, typename LHS>
bool greater_than(LHS const lhs) {
  return detail::greater_than_impl<
    RHS, rhs, typename std::remove_reference<LHS>::type
  >(lhs);
}

/**
 * Check if is a specialization. See:
 * https://stackoverflow.com/questions/16337610/how-to-know-if-a-type-is-a-specialization-of-stdvector
 */
template<typename Test, template<typename...> class Ref>
struct IsSpecialization
  : std::false_type {};

template<template<typename...> class Ref, typename... Args>
struct IsSpecialization<Ref<Args...>, Ref>
  : std::true_type {};

/**
 * decltype of function parameter and result.
 * https://stackoverflow.com/questions/43526647/decltype-of-function-parameter
 */

template <std::size_t N, typename T0, typename... Ts>
struct TypeN {
  using type = typename TypeN<N - 1U, Ts...>::type;
};
template <typename T0, typename... Ts>
struct TypeN<0U, T0, Ts...> {
  using type = T0;
};

template <std::size_t, typename>
struct ArgType;
template <std::size_t N, typename R, typename... As>
struct ArgType<N, R(As...)> {
  using type = typename TypeN<N, As...>::type;
};

// or use: std::result_of
template <typename>
struct ReturnType;
template <typename R, typename... As>
struct ReturnType<R(As...)> {
  using type = R;
};

} // namespace acc
