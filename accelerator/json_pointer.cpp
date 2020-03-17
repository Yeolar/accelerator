/*
 * Copyright 2011-present Facebook, Inc.
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

#include "accelerator/json_pointer.h"

#include "accelerator/String.h"

namespace acc {

// static, public
Expected<json_pointer, json_pointer::parse_error> json_pointer::try_parse(
    StringPiece const str) {
  // pointer describes complete document
  if (str.empty()) {
    return json_pointer{};
  }

  if (str.at(0) != '/') {
    return makeUnexpected(parse_error::INVALID_FIRST_CHARACTER);
  }

  std::vector<std::string> tokens;
  splitTo<std::string>("/", str, std::inserter(tokens, tokens.begin()));
  tokens.erase(tokens.begin());

  for (auto& token : tokens) {
    if (!unescape(token)) {
      return makeUnexpected(parse_error::INVALID_ESCAPE_SEQUENCE);
    }
  }

  return json_pointer(std::move(tokens));
}

// static, public
json_pointer json_pointer::parse(StringPiece const str) {
  auto res = try_parse(str);
  if (res.hasValue()) {
    return std::move(res.value());
  }
  switch (res.error()) {
    case parse_error::INVALID_FIRST_CHARACTER:
      throw json_pointer::parse_exception(
          "non-empty JSON pointer string does not start with '/'");
    case parse_error::INVALID_ESCAPE_SEQUENCE:
      throw json_pointer::parse_exception(
          "Invalid escape sequence in JSON pointer string");
  }
}

std::vector<std::string> const& json_pointer::tokens() const {
  return tokens_;
}

// private
json_pointer::json_pointer(std::vector<std::string> tokens) noexcept
    : tokens_{std::move(tokens)} {}

// private, static
bool json_pointer::unescape(std::string& str) {
  char const* end = &str[str.size()];
  char* out = &str.front();
  char const* decode = out;
  while (decode < end) {
    if (*decode != '~') {
      *out++ = *decode++;
      continue;
    }
    if (decode + 1 == end) {
      return false;
    }
    switch (decode[1]) {
      case '1':
        *out++ = '/';
        break;
      case '0':
        *out++ = '~';
        break;
      default:
        return false;
    }
    decode += 2;
  }
  str.resize(out - &str.front());
  return true;
}

} // namespace acc