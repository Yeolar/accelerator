/*
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

#include <stddef.h>
#include <algorithm>
#include <limits>
#include <type_traits>

namespace acc {

template <class Container>
typename std::enable_if<
  std::is_arithmetic<typename Container::value_type>::value,
  typename Container::value_type>::type
min(const Container& container) {
  typename Container::value_type maxValue =
    std::numeric_limits<typename Container::value_type>::max();
  for (auto& i : container) {
    if (maxValue > i) {
      maxValue = i;
    }
  }
  return maxValue;
}

template <class Container>
typename std::enable_if<
  std::is_arithmetic<typename Container::value_type>::value,
  typename Container::value_type>::type
max(const Container& container) {
  typename Container::value_type maxValue =
    std::numeric_limits<typename Container::value_type>::lowest();
  for (auto& i : container) {
    if (maxValue < i) {
      maxValue = i;
    }
  }
  return maxValue;
}

template <class Container>
typename std::enable_if<
  std::is_arithmetic<typename Container::value_type>::value,
  typename Container::value_type>::type
sum(const Container& container) {
  typename Container::value_type sumValue = 0;
  for (auto& i : container) {
    sumValue += i;
  }
  return sumValue;
}

// for sorted container
template <class Container>
typename std::enable_if<
  std::is_arithmetic<typename Container::value_type>::value,
  typename Container::value_type>::type
median(const Container& container) {
  size_t n = container.size();
  if (n > 0) {
    auto it = container.begin() + n / 2;
    if (n % 2 != 0) {
      return *it;
    } else {
      return (*it + *(it - 1)) / 2;
    }
  }
  return 0;
}

//////////////////////////////////////////////////////////////////////

template <class Container>
bool contain(const Container& container,
             const typename Container::value_type& value) {
  return std::find(container.begin(), container.end(), value)
    != container.end();
}

template <class Container>
bool containKey(const Container& container,
                const typename Container::key_type& key) {
  return container.find(key) != container.end();
}

template <class Container>
void remove(Container& container, const typename Container::value_type& value) {
  container.erase(std::remove(container.begin(), container.end(), value),
                  container.end());
}

template <class Container>
void subRange(Container& container, size_t begin, size_t end) {
  size_t b = std::min(begin, container.size());
  size_t e = std::min(end, container.size());
  container.erase(container.begin() + e, container.end());
  container.erase(container.begin(), container.begin() + b);
}

//////////////////////////////////////////////////////////////////////

template <class T>
inline bool noneOf(const T& value, const T& arg) {
  return value != arg;
}

template <class T, class... Args>
inline bool noneOf(const T& value, const T& arg, const Args&... args) {
  return value != arg && noneOf(value, args...);
}

template <class F, class T>
inline bool noneOf(const F& func, const T& arg) {
  return !func(arg);
}

template <class F, class T, class... Args>
inline bool noneOf(const F& func, const T& arg, const Args&... args) {
  return !func(arg) && noneOf(func, args...);
}

template <class T>
inline bool anyOf(const T& value, const T& arg) {
  return value == arg;
}

template <class T, class... Args>
inline bool anyOf(const T& value, const T& arg, const Args&... args) {
  return value == arg || anyOf(value, args...);
}

template <class F, class T>
inline bool anyOf(const F& func, const T& arg) {
  return func(arg);
}

template <class F, class T, class... Args>
inline bool anyOf(const F& func, const T& arg, const Args&... args) {
  return func(arg) || anyOf(func, args...);
}

} // namespace acc
