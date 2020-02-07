/*
 * Copyright 2017-present Facebook, Inc.
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
#include <limits>
#include <type_traits>

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

} // namespace acc
