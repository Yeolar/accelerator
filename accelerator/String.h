/*
 * Copyright 2017 Facebook, Inc.
 * Copyright 2017-present Yeolar
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

#pragma once

#include <cstdarg>
#include <exception>
#include <string>
#include <vector>

#include "accelerator/Conv.h"
#include "accelerator/Demangle.h"
#include "accelerator/Range.h"
#include "accelerator/ScopeGuard.h"
#include "accelerator/Traits.h"

#define ACC_PRINTF_FORMAT_ATTR(format_param, dots_param) \
  __attribute__((__format__(__printf__, format_param, dots_param)))

namespace acc {

/**
 * C-Escape a string, making it suitable for representation as a C string
 * literal.  Appends the result to the output string.
 *
 * Backslashes all occurrences of backslash and double-quote:
 *   "  ->  \"
 *   \  ->  \\
 *
 * Replaces all non-printable ASCII characters with backslash-octal
 * representation:
 *   <ASCII 254> -> \376
 *
 * Note that we use backslash-octal instead of backslash-hex because the octal
 * representation is guaranteed to consume no more than 3 characters; "\3760"
 * represents two characters, one with value 254, and one with value 48 ('0'),
 * whereas "\xfe0" represents only one character (with value 4064, which leads
 * to implementation-defined behavior).
 */
void cEscape(StringPiece str, std::string& out);

/**
 * Similar to cEscape above, but returns the escaped string.
 */
inline std::string cEscape(StringPiece str) {
  std::string out;
  cEscape(str, out);
  return out;
}

/**
 * C-Unescape a string; the opposite of cEscape above.  Appends the result
 * to the output string.
 *
 * Recognizes the standard C escape sequences:
 *
 * \' \" \? \\ \a \b \f \n \r \t \v
 * \[0-7]+
 * \x[0-9a-fA-F]+
 *
 * In strict mode (default), throws std::invalid_argument if it encounters
 * an unrecognized escape sequence.  In non-strict mode, it leaves
 * the escape sequence unchanged.
 */
void cUnescape(StringPiece str, std::string& out, bool strict = true);

/**
 * Similar to cUnescape above, but returns the escaped string.
 */
inline std::string cUnescape(StringPiece str, bool strict = true) {
  std::string out;
  cUnescape(str, out, strict);
  return out;
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
std::string stringPrintf(const char* format, ...)
  ACC_PRINTF_FORMAT_ATTR(1, 2);

/* Similar to stringPrintf, with different signature. */
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
 * Backslashify a string, that is, replace non-printable characters
 * with C-style (but NOT C compliant) "\xHH" encoding.  If hex_style
 * is false, then shorthand notations like "\0" will be used instead
 * of "\x00" for the most common backslash cases.
 *
 * There are two forms, one returning the input string, and one
 * creating output in the specified output string.
 *
 * This is mainly intended for printing to a terminal, so it is not
 * particularly optimized.
 *
 * Do *not* use this in situations where you expect to be able to feed
 * the string to a C or C++ compiler, as there are nuances with how C
 * parses such strings that lead to failures.  This is for display
 * purposed only.  If you want a string you can embed for use in C or
 * C++, use cEscape instead.  This function is for display purposes
 * only.
 */
void backslashify(
    acc::StringPiece input,
    std::string& output,
    bool hex_style = false);

inline std::string backslashify(StringPiece input, bool hex_style = false) {
  std::string output;
  backslashify(input, output, hex_style);
  return output;
}

/**
 * Same functionality as Python's binascii.hexlify.  Returns true
 * on successful conversion.
 *
 * If append_output is true, append data to the output rather than
 * replace it.
 */
template <class InputString>
bool hexlify(const InputString& input, std::string& output,
             bool append=false);

inline std::string hexlify(ByteRange input) {
  std::string output;
  if (!hexlify(input, output)) {
    // hexlify() currently always returns true, so this can't really happen
    throw std::runtime_error("hexlify failed");
  }
  return output;
}

inline std::string hexlify(StringPiece input) {
  return hexlify(ByteRange{input});
}

/**
 * Same functionality as Python's binascii.unhexlify.  Returns true
 * on successful conversion.
 */
template <class InputString>
bool unhexlify(const InputString& input, std::string& output);

inline std::string unhexlify(StringPiece input) {
  std::string output;
  if (!unhexlify(input, output)) {
    // unhexlify() fails if the input has non-hexidecimal characters,
    // or if it doesn't consist of a whole number of bytes
    throw std::domain_error("unhexlify() called with non-hex input");
  }
  return output;
}

/**
 * Write a hex dump of size bytes starting at ptr to out.
 *
 * The hex dump is formatted as follows:
 *
 * for the string "abcdefghijklmnopqrstuvwxyz\x02"
00000000  61 62 63 64 65 66 67 68  69 6a 6b 6c 6d 6e 6f 70  |abcdefghijklmnop|
00000010  71 72 73 74 75 76 77 78  79 7a 02                 |qrstuvwxyz.     |
 *
 * that is, we write 16 bytes per line, both as hex bytes and as printable
 * characters.  Non-printable characters are replaced with '.'
 * Lines are written to out one by one (one StringPiece at a time) without
 * delimiters.
 */
template <class OutIt>
void hexDump(const void* ptr, size_t size, OutIt out);

/**
 * Return the hex dump of size bytes starting at ptr as a string.
 */
std::string hexDump(const void* ptr, size_t size);

/**
 * Return a std::string containing the description of the given errno value.
 * Takes care not to overwrite the actual system errno, so calling
 * errnoStr(errno) is valid.
 */
std::string errnoStr(int err);

/**
 * Debug string for an exception: include type and what(), if
 * defined.
 */
inline std::string exceptionStr(const std::exception& e) {
  return to<std::string>(demangle(typeid(e)), ": ", e.what());
}

template<typename E>
auto exceptionStr(const E& e) -> typename std::
    enable_if<!std::is_base_of<std::exception, E>::value, std::string>::type {
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
void split(const Delim& delimiter,
           const String& input,
           std::vector<OutputType>& out,
           const bool ignoreEmpty = false);

template <
    class OutputValueType,
    class Delim,
    class String,
    class OutputIterator>
void splitTo(const Delim& delimiter,
             const String& input,
             OutputIterator out,
             const bool ignoreEmpty = false);

/*
 * Split a string into a fixed number of string pieces and/or numeric types
 * by delimiter. Conversions are supported for any type which acc:to<> can
 * target, including all overloads of parseTo(). Returns 'true' if the fields
 * were all successfully populated.  Returns 'false' if there were too few
 * fields in the input, or too many fields if exact=true.  Casting exceptions
 * will not be caught.
 *
 * Examples:
 *
 *  acc::StringPiece name, key, value;
 *  if (acc::split('\t', line, name, key, value))
 *    ...
 *
 *  acc::StringPiece name;
 *  double value;
 *  int id;
 *  if (acc::split('\t', line, name, value, id))
 *    ...
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
 *    assert(x == "a" && y == "b:c");
 *
 * Note that this will likely not work if the last field's target is of numeric
 * type, in which case acc::to<> will throw an exception.
 */
namespace detail {
template <typename Void, typename OutputType>
struct IsConvertible : std::false_type {};

template <typename OutputType>
struct IsConvertible<
    void_t<decltype(parseTo(StringPiece{}, std::declval<OutputType&>()))>,
    OutputType> : std::true_type {};
} // namespace detail
template <typename OutputType>
struct IsConvertible : detail::IsConvertible<void, OutputType> {};

template <bool exact = true, class Delim, class... OutputTypes>
typename std::enable_if<
    StrictConjunction<IsConvertible<OutputTypes>...>::value &&
        sizeof...(OutputTypes) >= 1,
    bool>::type
split(const Delim& delimiter, StringPiece input, OutputTypes&... outputs);

/*
 * Join list of tokens.
 *
 * Stores a string representation of tokens in the same order with
 * deliminer between each element.
 */

template <class Delim, class Iterator>
void join(const Delim& delimiter,
          Iterator begin,
          Iterator end,
          std::string& output);

template <class Delim, class Container>
void join(const Delim& delimiter,
          const Container& container,
          std::string& output) {
  join(delimiter, container.begin(), container.end(), output);
}

template <class Delim, class Value>
void join(const Delim& delimiter,
          const std::initializer_list<Value>& values,
          std::string& output) {
  join(delimiter, values.begin(), values.end(), output);
}

template <class Delim, class Container>
std::string join(const Delim& delimiter,
                 const Container& container) {
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

template <
    class Delim,
    class Iterator,
    typename std::enable_if<std::is_base_of<
        std::forward_iterator_tag,
        typename std::iterator_traits<Iterator>::iterator_category>::value>::
        type* = nullptr>
std::string join(const Delim& delimiter, Iterator begin, Iterator end) {
  std::string output;
  join(delimiter, begin, end, output);
  return output;
}

template <class Delim, class Iterator>
void joinMap(const Delim& delimiter,
             const Delim& pairDelimiter,
             Iterator begin,
             Iterator end,
             std::string& output);

template <class Delim, class Container>
std::string joinMap(const Delim& delimiter,
                    const Delim& pairDelimiter,
                    const Container& container) {
  std::string output;
  joinMap(delimiter, pairDelimiter, container.begin(), container.end(), output);
  return output;
}

template <class Delim, class Iterator>
std::string joinMap(const Delim& delimiter,
                    const Delim& pairDelimiter,
                    Iterator begin,
                    Iterator end) {
  std::string output;
  joinMap(delimiter, pairDelimiter, begin, end, output);
  return output;
}

/**
 * Returns a subpiece with all whitespace removed from the front of @sp.
 * Whitespace means any of [' ', '\n', '\r', '\t'].
 */
StringPiece ltrimWhitespace(StringPiece sp);

/**
 * Returns a subpiece with all whitespace removed from the back of @sp.
 * Whitespace means any of [' ', '\n', '\r', '\t'].
 */
StringPiece rtrimWhitespace(StringPiece sp);

/**
 * Returns a subpiece with all whitespace removed from the back and front of @sp.
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
 * @param length Length of str, in bytes
 */
void toLowerAscii(char* str, size_t length);

inline void toLowerAscii(MutableStringPiece str) {
  toLowerAscii(str.begin(), str.size());
}

inline void toLowerAscii(std::string& str) {
  // str[0] is legal also if the string is empty.
  toLowerAscii(&str[0], str.size());
}

} // namespace acc

#include "String-inl.h"
