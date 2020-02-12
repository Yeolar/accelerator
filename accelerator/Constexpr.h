/*
 * Copyright 2017-present Facebook, Inc.
 * Copyright 2020 Yeolar
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

#include <cstdint>
#include <cstring>
#include <limits>
#include <type_traits>

#include "accelerator/CPortability.h"

namespace acc {

// TLDR: Prefer using operator< for ordering. And when
// a and b are equivalent objects, we return b to make
// sorting stable.
// See http://stepanovpapers.com/notes.pdf for details.
template <typename T>
constexpr T constexpr_max(T a) {
  return a;
}
template <typename T, typename... Ts>
constexpr T constexpr_max(T a, T b, Ts... ts) {
  return b < a ? constexpr_max(a, ts...) : constexpr_max(b, ts...);
}

// When a and b are equivalent objects, we return a to
// make sorting stable.
template <typename T>
constexpr T constexpr_min(T a) {
  return a;
}
template <typename T, typename... Ts>
constexpr T constexpr_min(T a, T b, Ts... ts) {
  return b < a ? constexpr_min(b, ts...) : constexpr_min(a, ts...);
}

namespace detail {
template <typename T>
constexpr T constexpr_log2_(T a, T e) {
  return e == T(1) ? a : constexpr_log2_(a + T(1), e / T(2));
}
} // namespace detail

template <typename T>
constexpr T constexpr_log2(T t) {
  return detail::constexpr_log2_(T(0), t);
}

namespace detail {

template <typename Char>
constexpr size_t constexpr_strlen_internal(const Char* s, size_t len) {
  return *s == Char(0) ? len : constexpr_strlen_internal(s + 1, len + 1);
}
static_assert(
    constexpr_strlen_internal("123456789", 0) == 9,
    "Someone appears to have broken constexpr_strlen...");

template <typename Char>
constexpr int constexpr_strcmp_internal(const Char* s1, const Char* s2) {
  return (*s1 == '\0' || *s1 != *s2)
      ? (static_cast<int>(*s1 - *s2))
      : constexpr_strcmp_internal(s1 + 1, s2 + 1);
}
} // namespace detail

template <typename Char>
constexpr size_t constexpr_strlen(const Char* s) {
  return detail::constexpr_strlen_internal(s, 0);
}

template <>
constexpr size_t constexpr_strlen(const char* s) {
#if ACC_HAS_FEATURE(cxx_constexpr_string_builtins)
  // clang provides a constexpr builtin
  return __builtin_strlen(s);
#elif defined(__GNUC__) && !defined(__clang__)
  // strlen() happens to already be constexpr under gcc
  return std::strlen(s);
#else
  return detail::constexpr_strlen_internal(s, 0);
#endif
}

} // namespace acc
