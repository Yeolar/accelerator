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

#include <cstdarg>
#include <exception>
#include <string>
#include <vector>

#include "accelerator/Conv.h"
#include "accelerator/Demangle.h"
#include "accelerator/FBString.h"
#include "accelerator/Range.h"

#define ACC_PRINTF_FORMAT_ATTR(format_param, dots_param) \
  __attribute__((__format__(__printf__, format_param, dots_param)))

namespace acc {

inline
std::string toStdString(const fbstring& s) {
  return std::string(s.data(), s.size());
}

inline
const std::string& toStdString(const std::string& s) {
  return s;
}

// If called with a temporary, the compiler will select this overload instead
// of the above, so we don't return a (lvalue) reference to a temporary.
inline
std::string&& toStdString(std::string&& s) {
  return std::move(s);
}

/**
 * URI-escape a string.  Appends the result to the output string.
 *
 * Alphanumeric characters and other characters marked as "unreserved" in RFC
 * 3986 ( -_.~ ) are left unchanged.  In PATH mode, the forward slash (/) is
 * also left unchanged.  In QUERY mode, spaces are replaced by '+'.  All other
 * characters are percent-encoded.
 */
enum class UriEscapeMode : unsigned char {
  // The values are meaningful, see generate_escape_tables.py
  ALL = 0,
  QUERY = 1,
  PATH = 2
};

void uriEscape(StringPiece str,
               std::string& out,
               UriEscapeMode mode = UriEscapeMode::ALL);

/**
 * Similar to uriEscape above, but returns the escaped string.
 */
inline std::string uriEscape(StringPiece str,
                             UriEscapeMode mode = UriEscapeMode::ALL) {
  std::string out;
  uriEscape(str, out, mode);
  return out;
}

/**
 * URI-unescape a string.  Appends the result to the output string.
 *
 * In QUERY mode, '+' are replaced by space.  %XX sequences are decoded if
 * XX is a valid hex sequence, otherwise we throw invalid_argument.
 */
void uriUnescape(StringPiece str,
                 std::string& out,
                 UriEscapeMode mode = UriEscapeMode::ALL);

/**
 * Similar to uriUnescape above, but returns the unescaped string.
 */
inline std::string uriUnescape(StringPiece str,
                               UriEscapeMode mode = UriEscapeMode::ALL) {
  std::string out;
  uriUnescape(str, out, mode);
  return out;
}

/**
 * stringPrintf is much like printf but deposits its result into a
 * string. Two signatures are supported: the first simply returns the
 * resulting string, and the second appends the produced characters to
 * the specified string and returns a reference to it.
 */
std::string stringPrintf(const char* format, ...) ACC_PRINTF_FORMAT_ATTR(1, 2);

void stringPrintf(std::string* out, const char* fmt, ...)
  ACC_PRINTF_FORMAT_ATTR(2, 3);

std::string& stringAppendf(std::string* output, const char* format, ...)
  ACC_PRINTF_FORMAT_ATTR(2, 3);

/**
 * Similar to stringPrintf, but accepts a va_list argument.
 *
 * As with vsnprintf() itself, the value of ap is undefined after the call.
 * These functions do not call va_end() on ap.
 */
std::string stringVPrintf(const char* format, va_list ap);
void stringVPrintf(std::string* out, const char* format, va_list ap);
std::string& stringVAppendf(std::string* out, const char* format, va_list ap);

/**
 * Same functionality as Python's binascii.hexlify.
 * Returns true on successful conversion.
 *
 * If append_output is true, append data to the output rather than
 * replace it.
 */
template <class InputString>
bool hexlify(const InputString& input, std::string& output,
             bool append = false);

/**
 * Same functionality as Python's binascii.unhexlify.
 * Returns true on successful conversion.
 */
template <class InputString>
bool unhexlify(const InputString& input, std::string& output);

/**
 * Return a fbstring containing the description of the given errno value.
 * Takes care not to overwrite the actual system errno, so calling
 * errnoStr(errno) is valid.
 */
fbstring errnoStr(int err);

/**
 * Debug string for an exception: include type and what(), if
 * defined.
 */
inline fbstring exceptionStr(const std::exception& e) {
  return to<fbstring>(demangle(typeid(e)), ": ", e.what());
}

template<typename E>
auto exceptionStr(const E& e) -> typename std::
    enable_if<!std::is_base_of<std::exception, E>::value, fbstring>::type {
  return demangle(typeid(e));
}

/**
 * Split a string into a list of tokens by delimiter.
 *
 * The split interface here supports different output types, selected
 * at compile time: StringPiece or std::string.  If you are
 * using a vector to hold the output, it detects the type based on
 * what your vector contains.  If the output vector is not empty, split
 * will append to the end of the vector.
 *
 * You can also use splitTo() to write the output to an arbitrary
 * OutputIterator (e.g. std::inserter() on a std::set<>), in which
 * case you have to tell the function the type.  (Rationale:
 * OutputIterators don't have a value_type, so we can't detect the
 * type in splitTo without being told.)
 *
 * Examples:
 *
 *   std::vector<acc::StringPiece> v;
 *   acc::split(":", "asd:bsd", v);
 *
 *   std::set<StringPiece> s;
 *   acc::splitTo<StringPiece>(":", "asd:bsd:asd:csd",
 *     std::inserter(s, s.begin()));
 *
 * Split also takes a flag (ignoreEmpty) that indicates whether adjacent
 * delimiters should be treated as one single separator (ignoring empty tokens)
 * or not (generating empty tokens).
 */

template <class Delim, class String, class OutputType>
void split(const Delim& delimiter, const String& input,
           std::vector<OutputType>& out,
           bool ignoreEmpty = false);

template <class OutputValueType, class Delim, class String,
          class OutputIterator>
void splitTo(const Delim& delimiter, const String& input,
             OutputIterator out,
             bool ignoreEmpty = false);

/*
 * Split a string into a fixed number of string pieces and/or numeric types
 * by delimiter. Any numeric type that acc::to<> can convert to from a
 * string piece is supported as a target. Returns 'true' if the fields were
 * all successfully populated.  Returns 'false' if there were too few fields
 * in the input, or too many fields if exact=true.  Casting exceptions will
 * not be caught.
 *
 * Examples:
 *
 *  acc::StringPiece name, key, value;
 *  if (acc::split('\t', line, name, key, value))
 *  ...
 *
 *  acc::StringPiece name;
 *  double value;
 *  int id;
 *  if (acc::split('\t', line, name, value, id))
 *  ...
 *
 * The 'exact' template parameter specifies how the function behaves when too
 * many fields are present in the input string. When 'exact' is set to its
 * default value of 'true', a call to split will fail if the number of fields in
 * the input string does not exactly match the number of output parameters
 * passed. If 'exact' is overridden to 'false', all remaining fields will be
 * stored, unsplit, in the last field, as shown below:
 *
 *  acc::StringPiece x, y.
 *  if (acc::split<false>(':', "a:b:c", x, y))
 *  assert(x == "a" && y == "b:c");
 *
 * Note that this will likely not work if the last field's target is of numeric
 * type, in which case acc::to<> will throw an exception.
 */
template <class T>
struct IsSplitTargetType {
  enum {
    value = std::is_arithmetic<T>::value ||
        std::is_same<T, StringPiece>::value ||
        std::is_same<T, std::string>::value
  };
};

template <bool exact = true, class Delim, class OutputType,
          class... OutputTypes>
typename std::enable_if<IsSplitTargetType<OutputType>::value, bool>::type
split(const Delim& delimiter, StringPiece input,
      OutputType& outHead, OutputTypes&... outTail);

/*
 * Similar to split(), but split for any char matching of delimiters.
 */
template <class Delim, class String, class OutputType>
void splitAny(const Delim& delimiters, const String& input,
              std::vector<OutputType>& out,
              bool ignoreEmpty = false);

template <class OutputValueType, class Delim, class String,
          class OutputIterator>
void splitAnyTo(const Delim& delimiters, const String& input,
                OutputIterator out,
                bool ignoreEmpty = false);

/**
 * Join list of tokens.
 *
 * Stores a string representation of tokens in the same order with
 * deliminer between each element.
 */

template <class Delim, class Iterator>
void join(const Delim& delimiter, Iterator begin, Iterator end,
          std::string& output);

template <class Delim, class Container>
void join(const Delim& delimiter, const Container& container,
          std::string& output) {
  join(delimiter, container.begin(), container.end(), output);
}

template <class Delim, class Value>
void join(const Delim& delimiter, const std::initializer_list<Value>& values,
          std::string& output) {
  join(delimiter, values.begin(), values.end(), output);
}

template <class Delim, class Container>
std::string join(const Delim& delimiter, const Container& container) {
  std::string output;
  join(delimiter, container.begin(), container.end(), output);
  return output;
}

template <class Delim, class Value>
std::string join(const Delim& delimiter,
                 const std::initializer_list<Value>& values) {
  std::string output;
  join(delimiter, values.begin(), values.end(), output);
  return output;
}

template <class Delim, class Iterator>
std::string join(const Delim& delimiter, Iterator begin, Iterator end) {
  std::string output;
  join(delimiter, begin, end, output);
  return output;
}

template <class Delim, class Iterator>
void joinMap(const Delim& delimiter, const Delim& pairDelimiter,
             Iterator begin, Iterator end,
             std::string& output);

template <class Delim, class Container>
std::string joinMap(const Delim& delimiter, const Delim& pairDelimiter,
                    const Container& container) {
  std::string output;
  joinMap(delimiter, pairDelimiter, container.begin(), container.end(), output);
  return output;
}

template <class Delim, class Iterator>
std::string joinMap(const Delim& delimiter, const Delim& pairDelimiter,
                    Iterator begin, Iterator end) {
  std::string output;
  joinMap(delimiter, pairDelimiter, begin, end, output);
  return output;
}

/**
 * Returns a subpiece with all whitespace removed from the front.
 * Whitespace means any of [' ', '\n', '\r', '\t'].
 */
StringPiece ltrimWhitespace(StringPiece sp);

/**
 * Returns a subpiece with all whitespace removed from the back.
 * Whitespace means any of [' ', '\n', '\r', '\t'].
 */
StringPiece rtrimWhitespace(StringPiece sp);

/**
 * Returns a subpiece with all whitespace removed from the back and front.
 * Whitespace means any of [' ', '\n', '\r', '\t'].
 */
inline StringPiece trimWhitespace(StringPiece sp) {
  return ltrimWhitespace(rtrimWhitespace(sp));
}

/**
 * Fast, in-place lowercasing of ASCII alphabetic characters in strings.
 * Leaves all other characters unchanged, including those with the 0x80
 * bit set.
 * @param str String to convert
 * @param len Length of str, in bytes
 */
void toLowerAscii(char* str, size_t length);

template <class String>
void toLowerAscii(String& s) {
  for (auto& c : s) {
    c = tolower(c);
  }
}

inline std::string toLowerAscii(StringPiece sp) {
  std::string out = sp.str();
  toLowerAscii(out);
  return out;
}

} // namespace acc

#include "String-inl.h"
