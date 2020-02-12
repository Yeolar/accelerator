/*
 * Copyright 2013-present Facebook, Inc.
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

#include <functional>
#include <tuple>

#include "accelerator/Hash.h"

namespace acc {

namespace uri_detail {

using UriTuple = std::tuple<
    const std::string&,
    const std::string&,
    const std::string&,
    const std::string&,
    uint16_t,
    const std::string&,
    const std::string&,
    const std::string&>;

inline UriTuple as_tuple(const acc::Uri& k) {
  return UriTuple(
      k.scheme(),
      k.username(),
      k.password(),
      k.host(),
      k.port(),
      k.path(),
      k.query(),
      k.fragment());
}

} // namespace uri_detail

} // namespace acc

namespace std {

template <>
struct hash<acc::Uri> {
  std::size_t operator()(const acc::Uri& k) const {
    return std::hash<acc::uri_detail::UriTuple>{}(
        acc::uri_detail::as_tuple(k));
  }
};

template <>
struct equal_to<acc::Uri> {
  bool operator()(const acc::Uri& a, const acc::Uri& b) const {
    return acc::uri_detail::as_tuple(a) == acc::uri_detail::as_tuple(b);
  }
};

} // namespace std
