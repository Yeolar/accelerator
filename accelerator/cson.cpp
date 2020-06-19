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

#include "accelerator/cson.h"

#include <boost/algorithm/string.hpp>

#include "accelerator/Bits.h"
#include "accelerator/Conv.h"
#include "accelerator/Range.h"
#include "accelerator/String.h"
#include "accelerator/Unicode.h"

namespace acc {

//////////////////////////////////////////////////////////////////////

namespace cson {
namespace {

struct Printer {
  explicit Printer(
      std::string& out,
      unsigned* indentLevel,
      serialization_opts const* opts)
      : out_(out), indentLevel_(indentLevel), opts_(*opts) {}

  void operator()(dynamic const& v) const {
    switch (v.type()) {
    case dynamic::DOUBLE:
      toAppend(v.asDouble(), &out_, opts_.double_mode, opts_.double_num_digits);
      break;
    case dynamic::INT64: {
      toAppend(v.asInt(), &out_);
      break;
    }
    case dynamic::BOOL:
      out_ += v.asBool() ? "true" : "false";
      break;
    case dynamic::NULLT:
      out_ += "null";
      break;
    case dynamic::STRING:
      escapeString(v.asString(), out_, opts_);
      break;
    case dynamic::OBJECT:
      printObject(v);
      break;
    case dynamic::ARRAY:
      printArray(v);
      break;
    default:
      ACCCHECK(0) << "Bad type " << v.type();
    }
  }

 private:
  void printKV(const std::pair<const dynamic, dynamic>& p) const {
    if (!p.first.isString()) {
      throw std::runtime_error("acc::toCson: CSON object key was not a "
        "string");
    }
    (*this)(p.first);
    out_ += '=';
    (*this)(p.second);
  }

  template <typename Iterator>
  void printKVPairs(Iterator begin, Iterator end) const {
    printKV(*begin);
    for (++begin; begin != end; ++begin) {
      out_ += ',';
      newline();
      printKV(*begin);
    }
  }

  void printObject(dynamic const& o) const {
    if (o.empty()) {
      out_ += "{}";
      return;
    }

    out_ += '{';
    indent();
    newline();
    if (opts_.sort_keys) {
      std::vector<std::pair<dynamic, dynamic>> items(
        o.items().begin(), o.items().end());
      std::sort(items.begin(), items.end());
      printKVPairs(items.begin(), items.end());
    } else {
      printKVPairs(o.items().begin(), o.items().end());
    }
    outdent();
    newline();
    out_ += '}';
  }

  void printArray(dynamic const& a) const {
    if (a.empty()) {
      out_ += "[]";
      return;
    }

    out_ += '[';
    indent();
    newline();
    (*this)(a[0]);
    for (auto& val : range(std::next(a.begin()), a.end())) {
      out_ += ',';
      newline();
      (*this)(val);
    }
    outdent();
    newline();
    out_ += ']';
  }

 private:
  void outdent() const {
    if (indentLevel_) {
      --*indentLevel_;
    }
  }

  void indent() const {
    if (indentLevel_) {
      ++*indentLevel_;
    }
  }

  void newline() const {
    if (indentLevel_) {
      out_ += to<std::string>('\n', std::string(*indentLevel_ * 2, ' '));
    }
  }

 private:
  std::string& out_;
  unsigned* const indentLevel_;
  serialization_opts const& opts_;
};

//////////////////////////////////////////////////////////////////////

struct ACC_EXPORT ParseError : std::runtime_error {
  explicit ParseError(
      unsigned int line,
      std::string const& context,
      std::string const& expected)
      : std::runtime_error(to<std::string>(
            "cson parse error on line ",
            line,
            !context.empty() ? to<std::string>(" near `", context, '\'') : "",
            ": ",
            expected)) {}
};

// Wraps our input buffer with some helper functions.
struct Input {
  explicit Input(StringPiece range, cson::serialization_opts const* opts)
      : range_(range)
      , opts_(*opts)
      , lineNum_(0)
  {
    storeCurrent();
  }

  Input(Input const&) = delete;
  Input& operator=(Input const&) = delete;

  char const* begin() const { return range_.begin(); }

  // Parse ahead for as long as the supplied predicate is satisfied,
  // returning a range of what was skipped.
  template <class Predicate>
  StringPiece skipWhile(const Predicate& p) {
    std::size_t skipped = 0;
    for (; skipped < range_.size(); ++skipped) {
      if (!p(range_[skipped])) {
        break;
      }
      if (range_[skipped] == '\n') {
        ++lineNum_;
      }
    }
    auto ret = range_.subpiece(0, skipped);
    range_.advance(skipped);
    storeCurrent();
    return ret;
  }

  void skipWhitespace() {
    range_ = ltrimWhitespace(range_);
    storeCurrent();
  }

  void expect(char c) {
    if (**this != c) {
      throw ParseError(lineNum_, context(),
        to<std::string>("expected '", c, '\''));
    }
    ++*this;
  }

  std::size_t size() const {
    return range_.size();
  }

  int operator*() const {
    return current_;
  }

  void operator++() {
    range_.pop_front();
    storeCurrent();
  }

  bool consume(StringPiece str) {
    if (boost::starts_with(range_, str)) {
      range_.advance(str.size());
      storeCurrent();
      return true;
    }
    return false;
  }

  std::string context() const {
    return range_.subpiece(0, 16 /* arbitrary */).toString();
  }

  dynamic error(char const* what) const {
    throw ParseError(lineNum_, context(), what);
  }

  cson::serialization_opts const& getOpts() {
    return opts_;
  }

  void incrementRecursionLevel() {
    if (currentRecursionLevel_ > opts_.recursion_limit) {
      error("recursion limit exceeded");
    }
    currentRecursionLevel_++;
  }

  void decrementRecursionLevel() {
    currentRecursionLevel_--;
  }

 private:
  void storeCurrent() {
    current_ = range_.empty() ? EOF : range_.front();
  }

 private:
  StringPiece range_;
  cson::serialization_opts const& opts_;
  unsigned lineNum_;
  int current_;
  unsigned int currentRecursionLevel_{0};
};

class RecursionGuard {
 public:
  explicit RecursionGuard(Input& in) : in_(in) {
    in_.incrementRecursionLevel();
  }

  ~RecursionGuard() {
    in_.decrementRecursionLevel();
  }

 private:
  Input& in_;
};

dynamic parseValue(Input& in);
std::string parseString(Input& in);

dynamic parseObject(Input& in) {
  DCHECK_EQ(*in, '{');
  ++in;

  dynamic ret = dynamic::object;

  in.skipWhitespace();
  if (*in == '}') {
    ++in;
    return ret;
  }

  for (;;) {
    if (in.getOpts().allow_trailing_comma && *in == '}') {
      break;
    }
    auto key = parseString(in);
    in.skipWhitespace();
    in.expect('=');
    in.skipWhitespace();
    ret.insert(std::move(key), parseValue(in));

    in.skipWhitespace();
    if (*in != ',') {
      break;
    }
    ++in;
    in.skipWhitespace();
  }
  in.expect('}');

  return ret;
}

dynamic parseArray(Input& in) {
  DCHECK_EQ(*in, '[');
  ++in;

  dynamic ret = dynamic::array;

  in.skipWhitespace();
  if (*in == ']') {
    ++in;
    return ret;
  }

  for (;;) {
    if (in.getOpts().allow_trailing_comma && *in == ']') {
      break;
    }
    ret.push_back(parseValue(in));
    in.skipWhitespace();
    if (*in != ',') {
      break;
    }
    ++in;
    in.skipWhitespace();
  }
  in.expect(']');

  return ret;
}

std::string decodeUnicodeEscape(Input& in) {
  auto hexVal = [&] (int c) -> uint16_t {
    return uint16_t(
           c >= '0' && c <= '9' ? c - '0' :
           c >= 'a' && c <= 'f' ? c - 'a' + 10 :
           c >= 'A' && c <= 'F' ? c - 'A' + 10 :
           (in.error("invalid hex digit"), 0));
  };

  auto readHex = [&]() -> uint16_t {
    if (in.size() < 4) {
      in.error("expected 4 hex digits");
    }

    uint16_t ret = uint16_t(hexVal(*in) * 4096);
    ++in;
    ret += hexVal(*in) * 256;
    ++in;
    ret += hexVal(*in) * 16;
    ++in;
    ret += hexVal(*in);
    ++in;
    return ret;
  };

  /*
   * If the value encoded is in the surrogate pair range, we need to
   * make sure there is another escape that we can use also.
   */
  uint32_t codePoint = readHex();
  if (codePoint >= 0xd800 && codePoint <= 0xdbff) {
    if (!in.consume("\\u")) {
      in.error("expected another unicode escape for second half of "
        "surrogate pair");
    }
    uint16_t second = readHex();
    if (second >= 0xdc00 && second <= 0xdfff) {
      codePoint = 0x10000 + ((codePoint & 0x3ff) << 10) +
                  (second & 0x3ff);
    } else {
      in.error("second character in surrogate pair is invalid");
    }
  } else if (codePoint >= 0xdc00 && codePoint <= 0xdfff) {
    in.error("invalid unicode code point (in range [0xdc00,0xdfff])");
  }

  return codePointToUtf8(codePoint);
}

static bool isEndChar(char c) {
  switch (c) {
    case '=':
    case ',':
    case ']':
    case '}':
    case ' ':
    case '\n':
    case '\r':
    case '\t': return true;
    default: break;
  }
  return false;
}

std::string parseString(Input& in) {
  std::string ret;
  for (;;) {
    auto range = in.skipWhile(
      [] (char c) { return !isEndChar(c) && c != '\\'; }
    );
    ret.append(range.begin(), range.end());

    if (isEndChar(*in)) {
      break;
    }
    if (*in == '\\') {
      ++in;
      switch (*in) {
      case '\\':    ret.push_back('\\'); ++in; break;
      case '=':     ret.push_back('=');  ++in; break;
      case ',':     ret.push_back(',');  ++in; break;
      case ']':     ret.push_back(']');  ++in; break;
      case '}':     ret.push_back('}');  ++in; break;
      case '/':     ret.push_back('/');  ++in; break;
      case ' ':     ret.push_back(' ');  ++in; break;
      case 'b':     ret.push_back('\b'); ++in; break;
      case 'f':     ret.push_back('\f'); ++in; break;
      case 'n':     ret.push_back('\n'); ++in; break;
      case 'r':     ret.push_back('\r'); ++in; break;
      case 't':     ret.push_back('\t'); ++in; break;
      case 'u':     ++in; ret += decodeUnicodeEscape(in); break;
      default:
        in.error(
            to<std::string>("unknown escape ", *in, " in string").c_str());
      }
      continue;
    }
    if (*in == EOF) {
      in.error("unterminated string");
    }
    if (!*in) {
      /*
       * Apparently we're actually supposed to ban all control
       * characters from strings.  This seems unnecessarily
       * restrictive, so we're only banning zero bytes.  (Since the
       * string is presumed to be UTF-8 encoded it's fine to just
       * check this way.)
       */
      in.error("null byte in string");
    }

    ret.push_back(char(*in));
    ++in;
  }

  return ret;
}

dynamic parseValue(Input& in) {
  RecursionGuard guard(in);

  in.skipWhitespace();
  return *in == '[' ? parseArray(in) :
         *in == '{' ? parseObject(in) : parseString(in);
}

} // namespace

//////////////////////////////////////////////////////////////////////

std::string serialize(dynamic const& dyn, serialization_opts const& opts) {
  std::string ret;
  unsigned indentLevel = 0;
  Printer p(ret, opts.pretty_formatting ? &indentLevel : nullptr, &opts);
  p(dyn);
  return ret;
}

// Fast path to determine the longest prefix that can be left
// unescaped in a string of sizeof(T) bytes packed in an integer of
// type T.
template <class T>
size_t firstEscapableInWord(T s) {
  static_assert(std::is_unsigned<T>::value, "Unsigned integer required");
  static constexpr T kOnes = ~T() / 255; // 0x...0101
  static constexpr T kMsbs = kOnes * 0x80; // 0x...8080

  // Sets the MSB of bytes < b. Precondition: b < 128.
  auto isLess = [](T w, uint8_t b) {
    // A byte is < b iff subtracting b underflows, so we check that
    // the MSB wasn't set before and it's set after the subtraction.
    return (w - kOnes * b) & ~w & kMsbs;
  };

  auto isChar = [&](uint8_t c) {
    // A byte is == c iff it is 0 if xored with c.
    return isLess(s ^ (kOnes * c), 1);
  };

  // The following masks have the MSB set for each byte of the word
  // that satisfies the corresponding condition.
  auto isHigh = s & kMsbs; // >= 128
  auto isLow = isLess(s, 0x20); // <= 0x1f
  auto needsEscape = isHigh | isLow | isChar('\\')
      | isChar('=') | isChar(',') | isChar(']') | isChar('}')
      | isChar(' ') | isChar('\n') | isChar('\r') | isChar('\t')
      ;

  if (!needsEscape) {
    return sizeof(T);
  }

  if (kIsLittleEndian) {
    return findFirstSet(needsEscape) / 8 - 1;
  } else {
    return sizeof(T) - findLastSet(needsEscape) / 8;
  }
}

// Escape a string so that it is legal to print it in CSON text.
void escapeString(
    StringPiece input,
    std::string& out,
    const serialization_opts& opts) {
  auto hexDigit = [] (uint8_t c) -> char {
    return c < 10 ? c + '0' : c - 10 + 'a';
  };

  auto* p = reinterpret_cast<const unsigned char*>(input.begin());
  auto* q = reinterpret_cast<const unsigned char*>(input.begin());
  auto* e = reinterpret_cast<const unsigned char*>(input.end());

  while (p < e) {
    // Find the longest prefix that does not need escaping, and copy
    // it literally into the output string.
    auto firstEsc = p;
    while (firstEsc < e) {
      auto avail = e - firstEsc;
      uint64_t word = 0;
      if (avail >= 8) {
        word = loadUnaligned<uint64_t>(firstEsc);
      } else {
        word = partialLoadUnaligned<uint64_t>(firstEsc, avail);
      }
      auto prefix = firstEscapableInWord(word);
      DCHECK_LE(prefix, avail);
      firstEsc += prefix;
      if (prefix < 8) {
        break;
      }
    }
    if (firstEsc > p) {
      out.append(reinterpret_cast<const char*>(p), firstEsc - p);
      p = firstEsc;
      // We can't be in the middle of a multibyte sequence, so we can reset q.
      q = p;
      if (p == e) {
        break;
      }
    }

    // Handle the next byte that may need escaping.

    // Since non-ascii encoding inherently does utf8 validation
    // we explicitly validate utf8 only if non-ascii encoding is disabled.
    if (opts.skip_invalid_utf8 && !opts.encode_non_ascii) {
      // To achieve better spatial and temporal coherence
      // we do utf8 validation progressively along with the
      // string-escaping instead of two separate passes.

      // As the encoding progresses, q will stay at or ahead of p.
      ACCCHECK_GE(q, p);

      // As p catches up with q, move q forward.
      if (q == p) {
        // calling utf8_decode has the side effect of
        // checking that utf8 encodings are valid
        char32_t v = utf8ToCodePoint(q, e, opts.skip_invalid_utf8);
        if (opts.skip_invalid_utf8 && v == U'\ufffd') {
          out.append(u8"\ufffd");
          p = q;
          continue;
        }
      }
    }
    if (opts.encode_non_ascii && (*p & 0x80)) {
      // note that this if condition captures utf8 chars
      // with value > 127, so size > 1 byte
      char32_t v = utf8ToCodePoint(p, e, opts.skip_invalid_utf8);
      char buf[] = "\\u\0\0\0\0";
      buf[2] = hexDigit(uint8_t(v >> 12));
      buf[3] = hexDigit((v >> 8) & 0x0f);
      buf[4] = hexDigit((v >> 4) & 0x0f);
      buf[5] = hexDigit(v & 0x0f);
      out.append(buf, 6);
    } else if (*p == '\\' || isEndChar(*p)) {
      char buf[] = "\\\0";
      buf[1] = char(*p++);
      out.append(buf, 2);
    } else if (*p <= 0x1f) {
      switch (*p) {
        case '\b': out.append("\\b"); p++; break;
        case '\f': out.append("\\f"); p++; break;
        //case '\n': out.append("\\n"); p++; break;
        //case '\r': out.append("\\r"); p++; break;
        //case '\t': out.append("\\t"); p++; break;
        default:
          // Note that this if condition captures non readable chars
          // with value < 32, so size = 1 byte (e.g control chars).
          char buf[] = "\\u00\0\0";
          buf[4] = hexDigit(uint8_t((*p & 0xf0) >> 4));
          buf[5] = hexDigit(uint8_t(*p & 0xf));
          out.append(buf, 6);
          p++;
      }
    } else {
      out.push_back(char(*p++));
    }
  }
}

} // namespace cson

//////////////////////////////////////////////////////////////////////

dynamic parseCson(StringPiece range) {
  return parseCson(range, cson::serialization_opts());
}

dynamic parseCson(StringPiece range, cson::serialization_opts const& opts) {

  cson::Input in(range, &opts);

  auto ret = parseValue(in);
  in.skipWhitespace();
  if (in.size() && *in != '\0') {
    in.error("parsing didn't consume all input");
  }
  return ret;
}

std::string toCson(dynamic const& dyn) {
  return cson::serialize(dyn, cson::serialization_opts());
}

std::string toPrettyCson(dynamic const& dyn) {
  cson::serialization_opts opts;
  opts.pretty_formatting = true;
  return cson::serialize(dyn, opts);
}

} // namespace acc
