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

#include <cstdint>
#include <cstring>
#include <limits>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

#include "accelerator/Bits.h"
#include "accelerator/SpookyHashV2.h"
#include "accelerator/Tuple.h"

/*
 * Various hashing functions.
 */

namespace acc {
namespace hash {

// This is a general-purpose way to create a single hash from multiple
// hashable objects. hash_combine_generic takes a class Hasher implementing
// hash<T>; hash_combine uses a default hasher StdHasher that uses std::hash.
// hash_combine_generic hashes each argument and combines those hashes in
// an order-dependent way to yield a new hash.

// This is the Hash128to64 function from Google's cityhash (available
// under the MIT License).  We use it to reduce multiple 64 bit hashes
// into a single hash.
inline uint64_t hash_128_to_64(
    const uint64_t upper,
    const uint64_t lower) noexcept {
  // Murmur-inspired hashing.
  const uint64_t kMul = 0x9ddfea08eb382d69ULL;
  uint64_t a = (lower ^ upper) * kMul;
  a ^= (a >> 47);
  uint64_t b = (upper ^ a) * kMul;
  b ^= (b >> 47);
  b *= kMul;
  return b;
}

// Never used, but gcc demands it.
template <class Hasher>
inline size_t hash_combine_generic() {
  return 0;
}

template <
    class Iter,
    class Hash = std::hash<typename std::iterator_traits<Iter>::value_type>>
uint64_t
hash_range(Iter begin, Iter end, uint64_t hash = 0, Hash hasher = Hash()) {
  for (; begin != end; ++begin) {
    hash = hash_128_to_64(hash, hasher(*begin));
  }
  return hash;
}

inline uint32_t twang_32from64(uint64_t key) noexcept;

template <class Hasher, typename T, typename... Ts>
size_t hash_combine_generic(const T& t, const Ts&... ts) {
  size_t seed = Hasher::hash(t);
  if (sizeof...(ts) == 0) {
    return seed;
  }
  size_t remainder = hash_combine_generic<Hasher>(ts...);
  /* static */ if (sizeof(size_t) == sizeof(uint32_t)) {
    return twang_32from64((uint64_t(seed) << 32) | remainder);
  } else {
    return static_cast<size_t>(hash_128_to_64(seed, remainder));
  }
}

// Simply uses std::hash to hash.  Note that std::hash is not guaranteed
// to be a very good hash function; provided std::hash doesn't collide on
// the individual inputs, you are fine, but that won't be true for, say,
// strings or pairs
class StdHasher {
 public:
  // The standard requires all explicit and partial specializations of std::hash
  // supplied by either the standard library or by users to be default
  // constructible.
  template <typename T>
  static size_t hash(const T& t) noexcept(noexcept(std::hash<T>()(t))) {
    return std::hash<T>()(t);
  }
};

template <typename T, typename... Ts>
size_t hash_combine(const T& t, const Ts&... ts) {
  return hash_combine_generic<StdHasher>(t, ts...);
}

//////////////////////////////////////////////////////////////////////

/*
 * Thomas Wang 64 bit mix hash function
 */

inline uint64_t twang_mix64(uint64_t key) noexcept {
  key = (~key) + (key << 21); // key *= (1 << 21) - 1; key -= 1;
  key = key ^ (key >> 24);
  key = key + (key << 3) + (key << 8); // key *= 1 + (1 << 3) + (1 << 8)
  key = key ^ (key >> 14);
  key = key + (key << 2) + (key << 4); // key *= 1 + (1 << 2) + (1 << 4)
  key = key ^ (key >> 28);
  key = key + (key << 31); // key *= 1 + (1 << 31)
  return key;
}

/*
 * Inverse of twang_mix64
 *
 * Note that twang_unmix64 is significantly slower than twang_mix64.
 */

inline uint64_t twang_unmix64(uint64_t key) noexcept {
  // See the comments in jenkins_rev_unmix32 for an explanation as to how this
  // was generated
  key *= 4611686016279904257U;
  key ^= (key >> 28) ^ (key >> 56);
  key *= 14933078535860113213U;
  key ^= (key >> 14) ^ (key >> 28) ^ (key >> 42) ^ (key >> 56);
  key *= 15244667743933553977U;
  key ^= (key >> 24) ^ (key >> 48);
  key = (key + 1) * 9223367638806167551U;
  return key;
}

/*
 * Thomas Wang downscaling hash function
 */

inline uint32_t twang_32from64(uint64_t key) noexcept {
  key = (~key) + (key << 18);
  key = key ^ (key >> 31);
  key = key * 21;
  key = key ^ (key >> 11);
  key = key + (key << 6);
  key = key ^ (key >> 22);
  return (uint32_t)key;
}

/*
 * Robert Jenkins' reversible 32 bit mix hash function
 */

inline uint32_t jenkins_rev_mix32(uint32_t key) noexcept {
  key += (key << 12); // key *= (1 + (1 << 12))
  key ^= (key >> 22);
  key += (key << 4);  // key *= (1 + (1 << 4))
  key ^= (key >> 9);
  key += (key << 10); // key *= (1 + (1 << 10))
  key ^= (key >> 2);
  // key *= (1 + (1 << 7)) * (1 + (1 << 12))
  key += (key << 7);
  key += (key << 12);
  return key;
}

/*
 * Inverse of jenkins_rev_mix32
 *
 * Note that jenkinks_rev_unmix32 is significantly slower than
 * jenkins_rev_mix32.
 */

inline uint32_t jenkins_rev_unmix32(uint32_t key) noexcept {
  // These are the modular multiplicative inverses (in Z_2^32) of the
  // multiplication factors in jenkins_rev_mix32, in reverse order.  They were
  // computed using the Extended Euclidean algorithm, see
  // http://en.wikipedia.org/wiki/Modular_multiplicative_inverse
  key *= 2364026753U;

  // The inverse of a ^= (a >> n) is
  // b = a
  // for (int i = n; i < 32; i += n) {
  //   b ^= (a >> i);
  // }
  key ^= (key >> 2) ^ (key >> 4) ^ (key >> 6) ^ (key >> 8) ^ (key >> 10) ^
      (key >> 12) ^ (key >> 14) ^ (key >> 16) ^ (key >> 18) ^ (key >> 20) ^
      (key >> 22) ^ (key >> 24) ^ (key >> 26) ^ (key >> 28) ^ (key >> 30);
  key *= 3222273025U;
  key ^= (key >> 9) ^ (key >> 18) ^ (key >> 27);
  key *= 4042322161U;
  key ^= (key >> 22);
  key *= 16773121U;
  return key;
}

/*
 * Fowler / Noll / Vo (FNV) Hash
 *     http://www.isthe.com/chongo/tech/comp/fnv/
 */

const uint32_t FNV_32_HASH_START = 2166136261UL;
const uint64_t FNV_64_HASH_START = 14695981039346656037ULL;
const uint64_t FNVA_64_HASH_START = 14695981039346656037ULL;

inline uint32_t fnv32(
    const char* buf,
    uint32_t hash = FNV_32_HASH_START) noexcept {
  // forcing signed char, since other platforms can use unsigned
  const signed char* s = reinterpret_cast<const signed char*>(buf);

  for (; *s; ++s) {
    hash +=
        (hash << 1) + (hash << 4) + (hash << 7) + (hash << 8) + (hash << 24);
    hash ^= *s;
  }
  return hash;
}

inline uint32_t fnv32_buf(
    const void* buf,
    size_t n,
    uint32_t hash = FNV_32_HASH_START) noexcept {
  // forcing signed char, since other platforms can use unsigned
  const signed char* char_buf = reinterpret_cast<const signed char*>(buf);

  for (size_t i = 0; i < n; ++i) {
    hash +=
        (hash << 1) + (hash << 4) + (hash << 7) + (hash << 8) + (hash << 24);
    hash ^= char_buf[i];
  }

  return hash;
}

inline uint32_t fnv32(
    const std::string& str,
    uint32_t hash = FNV_32_HASH_START) noexcept {
  return fnv32_buf(str.data(), str.size(), hash);
}

inline uint64_t fnv64(
    const char* buf,
    uint64_t hash = FNV_64_HASH_START) noexcept {
  // forcing signed char, since other platforms can use unsigned
  const signed char* s = reinterpret_cast<const signed char*>(buf);

  for (; *s; ++s) {
    hash += (hash << 1) + (hash << 4) + (hash << 5) + (hash << 7) +
        (hash << 8) + (hash << 40);
    hash ^= *s;
  }
  return hash;
}

inline uint64_t fnv64_buf(
    const void* buf,
    size_t n,
    uint64_t hash = FNV_64_HASH_START) noexcept {
  // forcing signed char, since other platforms can use unsigned
  const signed char* char_buf = reinterpret_cast<const signed char*>(buf);

  for (size_t i = 0; i < n; ++i) {
    hash += (hash << 1) + (hash << 4) + (hash << 5) + (hash << 7) +
        (hash << 8) + (hash << 40);
    hash ^= char_buf[i];
  }
  return hash;
}

inline uint64_t fnv64(
    const std::string& str,
    uint64_t hash = FNV_64_HASH_START) noexcept {
  return fnv64_buf(str.data(), str.size(), hash);
}

inline uint64_t fnva64_buf(
    const void* buf,
    size_t n,
    uint64_t hash = FNVA_64_HASH_START) noexcept {
  const uint8_t* char_buf = reinterpret_cast<const uint8_t*>(buf);

  for (size_t i = 0; i < n; ++i) {
    hash ^= char_buf[i];
    hash += (hash << 1) + (hash << 4) + (hash << 5) + (hash << 7) +
        (hash << 8) + (hash << 40);
  }
  return hash;
}

inline uint64_t fnva64(
    const std::string& str,
    uint64_t hash = FNVA_64_HASH_START) noexcept {
  return fnva64_buf(str.data(), str.size(), hash);
}

/*
 * Paul Hsieh: http://www.azillionmonkeys.com/qed/hash.html
 */

#define get16bits(d) acc::loadUnaligned<uint16_t>(d)

inline uint32_t hsieh_hash32_buf(const void* buf, size_t len) noexcept {
  // forcing signed char, since other platforms can use unsigned
  const unsigned char* s = reinterpret_cast<const unsigned char*>(buf);
  uint32_t hash = static_cast<uint32_t>(len);
  uint32_t tmp;
  size_t rem;

  if (len <= 0 || buf == nullptr) {
    return 0;
  }

  rem = len & 3;
  len >>= 2;

  /* Main loop */
  for (; len > 0; len--) {
    hash += get16bits(s);
    tmp = (get16bits(s + 2) << 11) ^ hash;
    hash = (hash << 16) ^ tmp;
    s += 2 * sizeof(uint16_t);
    hash += hash >> 11;
  }

  /* Handle end cases */
  switch (rem) {
    case 3:
      hash += get16bits(s);
      hash ^= hash << 16;
      hash ^= s[sizeof(uint16_t)] << 18;
      hash += hash >> 11;
      break;
    case 2:
      hash += get16bits(s);
      hash ^= hash << 11;
      hash += hash >> 17;
      break;
    case 1:
      hash += *s;
      hash ^= hash << 10;
      hash += hash >> 1;
  }

  /* Force "avalanching" of final 127 bits */
  hash ^= hash << 3;
  hash += hash >> 5;
  hash ^= hash << 4;
  hash += hash >> 17;
  hash ^= hash << 25;
  hash += hash >> 6;

  return hash;
}

#undef get16bits

inline uint32_t hsieh_hash32(const char* s) noexcept {
  return hsieh_hash32_buf(s, std::strlen(s));
}

inline uint32_t hsieh_hash32_str(const std::string& str) noexcept {
  return hsieh_hash32_buf(str.data(), str.size());
}

//////////////////////////////////////////////////////////////////////

} // namespace hash

namespace detail {

template <typename I>
struct integral_hasher {
  size_t operator()(I const& i) const noexcept {
    static_assert(sizeof(I) <= 16, "Input type is too wide");
    /* constexpr */ if (sizeof(I) <= 4) {
      auto const i32 = static_cast<int32_t>(i); // impl accident: sign-extends
      auto const u32 = static_cast<uint32_t>(i32);
      return static_cast<size_t>(hash::jenkins_rev_mix32(u32));
    } else if (sizeof(I) <= 8) {
      auto const u64 = static_cast<uint64_t>(i);
      return static_cast<size_t>(hash::twang_mix64(u64));
    } else {
      auto const u = to_unsigned(i);
      auto const hi = static_cast<uint64_t>(u >> sizeof(I) * 4);
      auto const lo = static_cast<uint64_t>(u);
      return hash::hash_128_to_64(hi, lo);
    }
  }
};

template <typename F>
struct float_hasher {
  size_t operator()(F const& f) const noexcept {
    static_assert(sizeof(F) <= 8, "Input type is too wide");

    if (f == F{}) { // Ensure 0 and -0 get the same hash.
      return 0;
    }

    uint64_t u64 = 0;
    memcpy(&u64, &f, sizeof(F));
    return static_cast<size_t>(hash::twang_mix64(u64));
  }
};

} // namespace detail

template <class Key, class Enable = void>
struct hasher;

struct Hash {
  template <class T>
  size_t operator()(const T& v) const noexcept(noexcept(hasher<T>()(v))) {
    return hasher<T>()(v);
  }

  template <class T, class... Ts>
  size_t operator()(const T& t, const Ts&... ts) const {
    return hash::hash_128_to_64((*this)(t), (*this)(ts...));
  }
};

template <>
struct hasher<bool> {
  size_t operator()(bool key) const noexcept {
    // Make sure that all the output bits depend on the input.
    return key ? std::numeric_limits<size_t>::max() : 0;
  }
};

template <>
struct hasher<unsigned long long>
    : detail::integral_hasher<unsigned long long> {};

template <>
struct hasher<signed long long> : detail::integral_hasher<signed long long> {};

template <>
struct hasher<unsigned long> : detail::integral_hasher<unsigned long> {};

template <>
struct hasher<signed long> : detail::integral_hasher<signed long> {};

template <>
struct hasher<unsigned int> : detail::integral_hasher<unsigned int> {};

template <>
struct hasher<signed int> : detail::integral_hasher<signed int> {};

template <>
struct hasher<unsigned short> : detail::integral_hasher<unsigned short> {};

template <>
struct hasher<signed short> : detail::integral_hasher<signed short> {};

template <>
struct hasher<unsigned char> : detail::integral_hasher<unsigned char> {};

template <>
struct hasher<signed char> : detail::integral_hasher<signed char> {};

template <> // char is a different type from both signed char and unsigned char
struct hasher<char> : detail::integral_hasher<char> {};

template <>
struct hasher<float> : detail::float_hasher<float> {};

template <>
struct hasher<double> : detail::float_hasher<double> {};

template <>
struct hasher<std::string> {
  size_t operator()(const std::string& key) const {
    return static_cast<size_t>(
        hash::SpookyHashV2::Hash64(key.data(), key.size(), 0));
  }
};

template <typename T>
struct hasher<T, std::enable_if_t<std::is_enum<T>::value>> {
  size_t operator()(T key) const noexcept {
    return Hash()(static_cast<std::underlying_type_t<T>>(key));
  }
};

template <typename T1, typename T2>
struct hasher<std::pair<T1, T2>> {
  size_t operator()(const std::pair<T1, T2>& key) const {
    return Hash()(key.first, key.second);
  }
};

template <typename... Ts>
struct hasher<std::tuple<Ts...>> {
  size_t operator()(const std::tuple<Ts...>& key) const {
    return apply(Hash(), key);
  }
};

// recursion
template <size_t index, typename... Ts>
struct TupleHasher {
  size_t operator()(std::tuple<Ts...> const& key) const {
    return hash::hash_combine(
        TupleHasher<index - 1, Ts...>()(key), std::get<index>(key));
  }
};

// base
template <typename... Ts>
struct TupleHasher<0, Ts...> {
  size_t operator()(std::tuple<Ts...> const& key) const {
    // we could do std::hash here directly, but hash_combine hides all the
    // ugly templating implicitly
    return hash::hash_combine(std::get<0>(key));
  }
};

} // namespace acc

// Custom hash functions.
namespace std {
// Hash function for pairs. Requires default hash functions for both
// items in the pair.
template <typename T1, typename T2>
struct hash<std::pair<T1, T2>> {
  size_t operator()(const std::pair<T1, T2>& x) const {
    return acc::hash::hash_combine(x.first, x.second);
  }
};

// Hash function for tuples. Requires default hash functions for all types.
template <typename... Ts>
struct hash<std::tuple<Ts...>> {
  size_t operator()(std::tuple<Ts...> const& key) const {
    acc::TupleHasher<
        sizeof...(Ts) - 1, // start index
        Ts...>
        hasher;

    return hasher(key);
  }
};
} // namespace std
