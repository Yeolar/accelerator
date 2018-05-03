/*
 * Copyright 2017 Yeolar
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <boost/type_traits.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/has_xxx.hpp>
#include <boost/mpl/not.hpp>

namespace acc {

namespace traits_detail {

#define ACC_HAS_TRUE_XXX(name)                              \
  BOOST_MPL_HAS_XXX_TRAIT_DEF(name);                        \
  template <class T> struct name ## _is_true                \
    : std::is_same<typename T::name, std::true_type> {};    \
  template <class T> struct has_true_ ## name               \
    : std::conditional<                                     \
        has_ ## name<T>::value,                             \
        name ## _is_true<T>,                                \
        std::false_type                                     \
      >::type {};

ACC_HAS_TRUE_XXX(IsRelocatable)
ACC_HAS_TRUE_XXX(IsTriviallyCopyable)
ACC_HAS_TRUE_XXX(IsZeroInitializable)

#undef ACC_HAS_TRUE_XXX

} // namespace traits_detail

#define ACC_ASSUME_RELOCATABLE(...) \
  struct IsRelocatable<  __VA_ARGS__ > : std::true_type {};

template <class T> struct IsTriviallyCopyable
  : std::integral_constant<bool,
      !std::is_class<T>::value ||
      traits_detail::has_true_IsTriviallyCopyable<T>::value
    > {};

template <class T> struct IsRelocatable
  : std::integral_constant<bool,
      !std::is_class<T>::value ||
      IsTriviallyCopyable<T>::value ||
      traits_detail::has_true_IsRelocatable<T>::value
    > {};

template <class T> struct IsZeroInitializable
  : std::integral_constant<bool,
      !std::is_class<T>::value ||
      traits_detail::has_true_IsZeroInitializable<T>::value
    > {};

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

} // namespace acc

#define ACC_CREATE_HAS_MEMBER_TYPE_TRAITS(classname, type_name) \
  template <typename T> \
  class classname { \
    typedef char Y_[1]; \
    typedef char N_[2]; \
    template <typename C> \
    static Y_& test(typename C::type_name*); \
    template <typename> \
    static N_& test(...); \
   public: \
    const static bool value = (sizeof(test<T>(nullptr)) == sizeof(Y_)); \
  }

#define ACC_CREATE_HAS_MEMBER_FN_TRAITS_IMPL(classname, func_name, cv_qual) \
  template <typename TTheClass_, typename RTheReturn_, typename... TTheArgs_> \
  class classname<TTheClass_, RTheReturn_(TTheArgs_...) cv_qual> { \
    typedef char Y_[1]; \
    typedef char N_[2]; \
    template < \
      typename UTheClass_, RTheReturn_ (UTheClass_::*)(TTheArgs_...) cv_qual> \
      struct sfinae {}; \
    template <typename UTheClass_> \
    static Y_& test(sfinae<UTheClass_, &UTheClass_::func_name>*); \
    template <typename> \
    static N_& test(...); \
   public: \
    const static bool value = (sizeof(test<TTheClass_>(nullptr)) == sizeof(Y_)); \
  }

#define ACC_CREATE_HAS_MEMBER_FN_TRAITS(classname, func_name) \
  template <typename, typename> class classname; \
  ACC_CREATE_HAS_MEMBER_FN_TRAITS_IMPL(classname, func_name, ); \
  ACC_CREATE_HAS_MEMBER_FN_TRAITS_IMPL(classname, func_name, const); \
  ACC_CREATE_HAS_MEMBER_FN_TRAITS_IMPL( \
      classname, func_name, /* nolint */ volatile); \
  ACC_CREATE_HAS_MEMBER_FN_TRAITS_IMPL( \
      classname, func_name, /* nolint */ volatile const)

