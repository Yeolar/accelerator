/*
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

/**
 * Conf specialized json-like object notation.
 */

#pragma once

#include <iosfwd>
#include <string>

#include "accelerator/dynamic.h"
#include "accelerator/Range.h"

namespace acc {

//////////////////////////////////////////////////////////////////////

namespace cson {

struct serialization_opts {
  explicit serialization_opts()
      : pretty_formatting(false),
        encode_non_ascii(false),
        allow_trailing_comma(false),
        sort_keys(true),
        skip_invalid_utf8(false),
        double_mode(double_conversion::DoubleToStringConverter::SHORTEST),
        double_num_digits(0), // ignored when mode is SHORTEST
        recursion_limit(100) {}

  // If true, the serialized cson will contain space and newlines to
  // try to be minimally "pretty".
  bool pretty_formatting;

  // If true, non-ASCII utf8 characters would be encoded as \uXXXX.
  bool encode_non_ascii;

  // Allow trailing comma in lists of values / items
  bool allow_trailing_comma;

  // Sort keys of all objects before printing out (potentially slow)
  // using dynamic::operator<.
  bool sort_keys;

  // Replace invalid utf8 characters with U+FFFD and continue
  bool skip_invalid_utf8;

  // Options for how to print floating point values.  See Conv.h
  // toAppend implementation for floating point for more info
  double_conversion::DoubleToStringConverter::DtoaMode double_mode;
  unsigned int double_num_digits;

  // Recursion limit when parsing.
  unsigned int recursion_limit;
};

std::string serialize(dynamic const&, serialization_opts const&);

void escapeString(
    StringPiece input,
    std::string& out,
    const serialization_opts& opts);

} // namespace cson

//////////////////////////////////////////////////////////////////////

dynamic parseCson(StringPiece, cson::serialization_opts const&);
dynamic parseCson(StringPiece);

std::string toCson(dynamic const&);

std::string toPrettyCson(dynamic const&);

//////////////////////////////////////////////////////////////////////

} // namespace acc
