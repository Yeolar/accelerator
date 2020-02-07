/*
 * Copyright 2017 Facebook, Inc.
 * Copyright 2017-present Yeolar
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

#include <tuple>

#include "accelerator/backport/Functional.h"
#include "accelerator/backport/Utility.h"

namespace acc {

/**
 * Helper to generate an index sequence from a tuple like type
 */
template <typename Tuple>
using index_sequence_for_tuple =
    make_index_sequence<std::tuple_size<Tuple>::value>;

namespace detail {
namespace apply_tuple {
namespace adl {
using std::get;

struct ApplyInvoke {
  template <typename T>
  using seq = index_sequence_for_tuple<std::remove_reference_t<T>>;

  template <typename F, typename T, std::size_t... I>
  static constexpr auto invoke_(F&& f, T&& t, index_sequence<I...>) noexcept(
      is_nothrow_invocable<F&&, decltype(get<I>(std::declval<T>()))...>::value)
      -> invoke_result_t<F&&, decltype(get<I>(std::declval<T>()))...> {
    return invoke(static_cast<F&&>(f), get<I>(static_cast<T&&>(t))...);
  }
};

} // namespace adl
} // namespace apply_tuple
} // namespace detail

struct ApplyInvoke : private detail::apply_tuple::adl::ApplyInvoke {
 public:
  template <typename F, typename T>
  constexpr auto operator()(F&& f, T&& t) const noexcept(
      noexcept(invoke_(static_cast<F&&>(f), static_cast<T&&>(t), seq<T>{})))
      -> decltype(invoke_(static_cast<F&&>(f), static_cast<T&&>(t), seq<T>{})) {
    return invoke_(static_cast<F&&>(f), static_cast<T&&>(t), seq<T>{});
  }
};

//////////////////////////////////////////////////////////////////////

//  mimic: std::apply, C++17
template <typename F, typename Tuple>
constexpr decltype(auto) apply(F&& func, Tuple&& tuple) {
  return ApplyInvoke{}(static_cast<F&&>(func), static_cast<Tuple&&>(tuple));
}

} // namespace acc
