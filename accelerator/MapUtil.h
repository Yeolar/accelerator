/*
 * Copyright 2017 Facebook, Inc.
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

#include <stdexcept>

namespace acc {

template <class Map>
typename Map::mapped_type get_default(
    const Map& map, const typename Map::key_type& key,
    const typename Map::mapped_type& dflt = typename Map::mapped_type()) {
  auto pos = map.find(key);
  return (pos != map.end() ? pos->second : dflt);
}

template <class Map>
typename Map::mapped_type get_or_throw(
    const Map& map, const typename Map::key_type& key) {
  auto pos = map.find(key);
  if (pos != map.end()) {
    return pos->second;
  }
  throw std::out_of_range(key);
}

template <class Map>
const typename Map::mapped_type& get_ref_default(
    const Map& map, const typename Map::key_type& key,
    const typename Map::mapped_type& dflt) {
  auto pos = map.find(key);
  return (pos != map.end() ? pos->second : dflt);
}

template <class Map>
const typename Map::mapped_type& get_ref_or_throw(
    const Map& map, const typename Map::key_type& key) {
  auto pos = map.find(key);
  if (pos != map.end()) {
    return pos->second;
  }
  throw std::out_of_range(key);
}

template <class Map>
const typename Map::mapped_type* get_ptr(
    const Map& map, const typename Map::key_type& key) {
  auto pos = map.find(key);
  return (pos != map.end() ? &pos->second : nullptr);
}

template <class Map>
typename Map::mapped_type* get_ptr(
    Map& map, const typename Map::key_type& key) {
  auto pos = map.find(key);
  return (pos != map.end() ? &pos->second : nullptr);
}

template <class Map>
const typename Map::mapped_type::element_type* get_deref_smart_ptr(
    const Map& map, const typename Map::key_type& key) {
  auto pos = map.find(key);
  return (pos != map.end() ? pos->second.get() : nullptr);
}

template <class Map>
typename Map::mapped_type::element_type* get_deref_smart_ptr(
    Map& map, const typename Map::key_type& key) {
  auto pos = map.find(key);
  return (pos != map.end() ? pos->second.get() : nullptr);
}

template <class Container, class Map>
typename std::enable_if<
  std::is_same<
    typename Container::value_type,
    typename Map::mapped_type>::value, Container>::type
get_all(Map& map, const typename Map::key_type& key) {
  Container result;
  auto rng = map.equal_range(key);
  for (auto it = rng.first; it != rng.second; ++it) {
    result.push_back(it->second);
  }
  return result;
}

} // namespace acc
