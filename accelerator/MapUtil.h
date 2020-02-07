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

#include "accelerator/Conv.h"
#include "accelerator/backport/Functional.h"

namespace acc {

/**
 * Given a map and a key, return the value corresponding to the key in the map,
 * or a given default value if the key doesn't exist in the map.
 */
template <class Map, typename Key = typename Map::key_type>
typename Map::mapped_type get_default(const Map& map, const Key& key) {
  auto pos = map.find(key);
  return (pos != map.end()) ? (pos->second) : (typename Map::mapped_type{});
}
template <
    class Map,
    typename Key = typename Map::key_type,
    typename Value = typename Map::mapped_type,
    typename std::enable_if<!is_invocable<Value>::value>::type* = nullptr>
typename Map::mapped_type
get_default(const Map& map, const Key& key, Value&& dflt) {
  using M = typename Map::mapped_type;
  auto pos = map.find(key);
  return (pos != map.end()) ? (pos->second) : M(std::forward<Value>(dflt));
}

/**
 * Give a map and a key, return the value corresponding to the key in the map,
 * or a given default value if the key doesn't exist in the map.
 */
template <
    class Map,
    typename Key = typename Map::key_type,
    typename Func,
    typename = typename std::enable_if<
        is_invocable_r<typename Map::mapped_type, Func>::value>::type>
typename Map::mapped_type
get_default(const Map& map, const Key& key, Func&& dflt) {
  auto pos = map.find(key);
  return pos != map.end() ? pos->second : dflt();
}

/**
 * Given a map and a key, return the value corresponding to the key in the map,
 * or throw an exception of the specified type.
 */
template <
    class E = std::out_of_range,
    class Map,
    typename Key = typename Map::key_type>
const typename Map::mapped_type& get_or_throw(
    const Map& map,
    const Key& key,
    const std::string& exceptionStrPrefix = std::string()) {
  auto pos = map.find(key);
  if (pos != map.end()) {
    return pos->second;
  }
  throw E(acc::to<std::string>(exceptionStrPrefix, key));
}

template <
    class E = std::out_of_range,
    class Map,
    typename Key = typename Map::key_type>
typename Map::mapped_type& get_or_throw(
    Map& map,
    const Key& key,
    const std::string& exceptionStrPrefix = std::string()) {
  auto pos = map.find(key);
  if (pos != map.end()) {
    return pos->second;
  }
  throw E(acc::to<std::string>(exceptionStrPrefix, key));
}

/**
 * Given a map and a key, return a reference to the value corresponding to the
 * key in the map, or the given default reference if the key doesn't exist in
 * the map.
 */
template <class Map, typename Key = typename Map::key_type>
const typename Map::mapped_type& get_ref_default(
    const Map& map,
    const Key& key,
    const typename Map::mapped_type& dflt) {
  auto pos = map.find(key);
  return (pos != map.end() ? pos->second : dflt);
}

/**
 * Passing a temporary default value returns a dangling reference when it is
 * returned. Lifetime extension is broken by the indirection.
 * The caller must ensure that the default value outlives the reference returned
 * by get_ref_default().
 */
template <class Map, typename Key = typename Map::key_type>
const typename Map::mapped_type& get_ref_default(
    const Map& map,
    const Key& key,
    typename Map::mapped_type&& dflt) = delete;

template <class Map, typename Key = typename Map::key_type>
const typename Map::mapped_type& get_ref_default(
    const Map& map,
    const Key& key,
    const typename Map::mapped_type&& dflt) = delete;

/**
 * Given a map and a key, return a reference to the value corresponding to the
 * key in the map, or the given default reference if the key doesn't exist in
 * the map.
 */
template <
    class Map,
    typename Key = typename Map::key_type,
    typename Func,
    typename = typename std::enable_if<
        is_invocable_r<const typename Map::mapped_type&, Func>::value>::type,
    typename = typename std::enable_if<
        std::is_reference<invoke_result_t<Func>>::value>::type>
const typename Map::mapped_type&
get_ref_default(const Map& map, const Key& key, Func&& dflt) {
  auto pos = map.find(key);
  return (pos != map.end() ? pos->second : dflt());
}

/**
 * Given a map and a key, return a pointer to the value corresponding to the
 * key in the map, or nullptr if the key doesn't exist in the map.
 */
template <class Map, typename Key = typename Map::key_type>
const typename Map::mapped_type* get_ptr(const Map& map, const Key& key) {
  auto pos = map.find(key);
  return (pos != map.end() ? &pos->second : nullptr);
}

template <class Map, typename Key = typename Map::key_type>
typename Map::mapped_type* get_ptr(Map& map, const Key& key) {
  auto pos = map.find(key);
  return (pos != map.end() ? &pos->second : nullptr);
}

/**
 * Given a map and a key, return the pointer in the (smart pointer) value
 * corresponding to the key in the map, or nullptr if the key doesn't exist
 * in the map.
 */
template <class Map, typename Key = typename Map::key_type>
const typename Map::mapped_type::element_type* get_deref_smart_ptr(
    const Map& map, const Key& key) {
  auto pos = map.find(key);
  return (pos != map.end() ? pos->second.get() : nullptr);
}

template <class Map, typename Key = typename Map::key_type>
typename Map::mapped_type::element_type* get_deref_smart_ptr(
    Map& map, const Key& key) {
  auto pos = map.find(key);
  return (pos != map.end() ? pos->second.get() : nullptr);
}

/**
 * Given a map and a key, return a container of all values corresponding
 * to the key in the map.
 */
template <class Map, typename Key = typename Map::key_type>
std::vector<typename Map::mapped_type> get_all(const Map& map, const Key& key) {
  std::vector<typename Map::mapped_type> result;
  auto rng = map.equal_range(key);
  for (auto it = rng.first; it != rng.second; ++it) {
    result.push_back(it->second);
  }
  return result;
}

} // namespace acc
