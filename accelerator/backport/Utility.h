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

#include <utility>

namespace acc {

namespace utility_detail {
template <typename...>
struct make_seq_cat;
template <
    template <typename T, T...> class S,
    typename T,
    T... Ta,
    T... Tb,
    T... Tc>
struct make_seq_cat<S<T, Ta...>, S<T, Tb...>, S<T, Tc...>> {
  using type =
      S<T,
        Ta...,
        (sizeof...(Ta) + Tb)...,
        (sizeof...(Ta) + sizeof...(Tb) + Tc)...>;
};

// Not parameterizing by `template <typename T, T...> class, typename` because
// clang precisely v4.0 fails to compile that. Note that clang v3.9 and v5.0
// handle that code correctly.
//
// For this to work, `S0` is required to be `Sequence<T>` and `S1` is required
// to be `Sequence<T, 0>`.

template <std::size_t Size>
struct make_seq {
  template <typename S0, typename S1>
  using apply = typename make_seq_cat<
      typename make_seq<Size / 2>::template apply<S0, S1>,
      typename make_seq<Size / 2>::template apply<S0, S1>,
      typename make_seq<Size % 2>::template apply<S0, S1>>::type;
};
template <>
struct make_seq<1> {
  template <typename S0, typename S1>
  using apply = S1;
};
template <>
struct make_seq<0> {
  template <typename S0, typename S1>
  using apply = S0;
};
} // namespace utility_detail

/* using override */ using std::integer_sequence;
/* using override */ using std::index_sequence;

template <typename T, std::size_t Size>
using make_integer_sequence = typename utility_detail::make_seq<
    Size>::template apply<integer_sequence<T>, integer_sequence<T, 0>>;

template <std::size_t Size>
using make_index_sequence = make_integer_sequence<std::size_t, Size>;
template <class... T>
using index_sequence_for = make_index_sequence<sizeof...(T)>;

/**
 *  Backports from C++17 of:
 *    std::in_place_t
 *    std::in_place_type_t
 *    std::in_place_index_t
 *    std::in_place
 *    std::in_place_type
 *    std::in_place_index
 */

struct in_place_tag {};
template <class>
struct in_place_type_tag {};
template <std::size_t>
struct in_place_index_tag {};

using in_place_t = in_place_tag (&)(in_place_tag);
template <class T>
using in_place_type_t = in_place_type_tag<T> (&)(in_place_type_tag<T>);
template <std::size_t I>
using in_place_index_t = in_place_index_tag<I> (&)(in_place_index_tag<I>);

inline in_place_tag in_place(in_place_tag = {}) {
  return {};
}
template <class T>
inline in_place_type_tag<T> in_place_type(in_place_type_tag<T> = {}) {
  return {};
}
template <std::size_t I>
inline in_place_index_tag<I> in_place_index(in_place_index_tag<I> = {}) {
  return {};
}

} // namespace acc
