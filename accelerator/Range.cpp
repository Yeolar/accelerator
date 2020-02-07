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

// @author Mark Rabkin (mrabkin@fb.com)
// @author Andrei Alexandrescu (andrei.alexandrescu@fb.com)

#include "accelerator/Range.h"

#include "accelerator/SparseByteSet.h"

namespace acc {
namespace detail {

size_t qfind_first_byte_of_byteset(
    const StringPiece haystack,
    const StringPiece needles) {
  SparseByteSet s;
  for (auto needle : needles) {
    s.add(uint8_t(needle));
  }
  for (size_t index = 0; index < haystack.size(); ++index) {
    if (s.contains(uint8_t(haystack[index]))) {
      return index;
    }
  }
  return std::string::npos;
}

inline size_t qfind_first_byte_of_std(
    const StringPiece haystack,
    const StringPiece needles) {
  auto ret = std::find_first_of(
      haystack.begin(),
      haystack.end(),
      needles.begin(),
      needles.end(),
      [](char a, char b) { return a == b; });
  return ret == haystack.end() ? std::string::npos : ret - haystack.begin();
}

size_t qfind_first_byte_of_nosse(
    const StringPiece haystack,
    const StringPiece needles) {
  if (UNLIKELY(needles.empty() || haystack.empty())) {
    return std::string::npos;
  }
  // The thresholds below were empirically determined by benchmarking.
  // This is not an exact science since it depends on the CPU, the size of
  // needles, and the size of haystack.
  if ((needles.size() >= 4 && haystack.size() <= 10) ||
      (needles.size() >= 16 && haystack.size() <= 64) || needles.size() >= 32) {
    return qfind_first_byte_of_byteset(haystack, needles);
  }
  return qfind_first_byte_of_std(haystack, needles);
}

} // namespace detail
} // namespace acc
