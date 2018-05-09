/*
 * Copyright 2018 Yeolar
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

#include <gtest/gtest.h>

namespace acc {
namespace test {

template <class T>
typename std::enable_if<std::is_same<T, std::nullptr_t>::value>::type
expectEq(const T&, const T&) {
  EXPECT_TRUE(true);
}

template <class T>
typename std::enable_if<std::is_integral<T>::value>::type
expectEq(const T& a, const T& b) {
  EXPECT_EQ(a, b);
}

template <class T>
typename std::enable_if<std::is_same<T, float>::value>::type
expectEq(const T& a, const T& b) {
  EXPECT_FLOAT_EQ(a, b);
}

template <class T>
typename std::enable_if<std::is_same<T, double>::value>::type
expectEq(const T& a, const T& b) {
  EXPECT_DOUBLE_EQ(a, b);
}

template <class T>
typename std::enable_if<std::is_convertible<T, StringPiece>::value>::type
expectEq(const T& a, const T& b) {
  EXPECT_STREQ(a.data(), b.data());
}

template <class T>
typename std::enable_if<
  has_first_type<T>::value &&
  has_second_type<T>::value>::type
expectEq(const T& a, const T& b) {
  expectEq(a.first, b.first);
  expectEq(a.second, b.second);
}

template <class T>
typename std::enable_if<
  has_iterator<T>::value &&
  !std::is_convertible<T, StringPiece>::value>::type
expectEq(const T& a, const T& b) {
  expectEq(a.size(), b.size());
  for (auto i = a.begin(), j = b.begin(); i != a.end(); i++, j++) {
    expectEq(*i, *j);
  }
}

} // namespace test
} // namespace acc
