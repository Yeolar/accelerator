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

/**
 * Various low-level, bit-manipulation routines.
 *
 * findFirstSet(x)  [constexpr]
 *    find first (least significant) bit set in a value of an integral type,
 *    1-based (like ffs()).  0 = no bits are set (x == 0)
 *
 * findLastSet(x)  [constexpr]
 *    find last (most significant) bit set in a value of an integral type,
 *    1-based.  0 = no bits are set (x == 0)
 *    for x != 0, findLastSet(x) == 1 + floor(log2(x))
 *
 * nextPowTwo(x)  [constexpr]
 *    Finds the next power of two >= x.
 *
 * isPowTwo(x)  [constexpr]
 *    return true iff x is a power of two
 *
 * popcount(x)
 *    return the number of 1 bits in x
 *
 * Endian
 *    convert between native, big, and little endian representation
 *    Endian::big(x)      big <-> native
 *    Endian::little(x)   little <-> native
 *    Endian::swap(x)     big <-> little
 */

#pragma once

#include <cinttypes>
#include <cstdint>
#include <cstring>
#include <limits>

#include "accelerator/ConstexprMath.h"
#include "accelerator/Traits.h"

namespace acc {

namespace detail {
template <typename Dst, typename Src>
constexpr std::make_signed_t<Dst> bits_to_signed(Src const s) {
  static_assert(std::is_signed<Dst>::value, "unsigned type");
  return to_signed(static_cast<std::make_unsigned_t<Dst>>(to_unsigned(s)));
}
template <typename Dst, typename Src>
constexpr std::make_unsigned_t<Dst> bits_to_unsigned(Src const s) {
  static_assert(std::is_unsigned<Dst>::value, "signed type");
  return static_cast<Dst>(to_unsigned(s));
}
} // namespace detail

/// findFirstSet
///
/// Return the 1-based index of the least significant bit which is set.
/// For x > 0, the exponent in the largest power of two which does not divide x.
template <typename T>
inline constexpr unsigned int findFirstSet(T const v) {
  using S0 = int;
  using S1 = long int;
  using S2 = long long int;
  using detail::bits_to_signed;
  static_assert(sizeof(T) <= sizeof(S2), "over-sized type");
  static_assert(std::is_integral<T>::value, "non-integral type");
  static_assert(!std::is_same<T, bool>::value, "bool type");

  // clang-format off
  return static_cast<unsigned int>(
      sizeof(T) <= sizeof(S0) ? __builtin_ffs(bits_to_signed<S0>(v)) :
      sizeof(T) <= sizeof(S1) ? __builtin_ffsl(bits_to_signed<S1>(v)) :
      sizeof(T) <= sizeof(S2) ? __builtin_ffsll(bits_to_signed<S2>(v)) :
      0);
  // clang-format on
}

/// findLastSet
///
/// Return the 1-based index of the most significant bit which is set.
/// For x > 0, findLastSet(x) == 1 + floor(log2(x)).
template <typename T>
inline constexpr unsigned int findLastSet(T const v) {
  using U0 = unsigned int;
  using U1 = unsigned long int;
  using U2 = unsigned long long int;
  using detail::bits_to_unsigned;
  static_assert(sizeof(T) <= sizeof(U2), "over-sized type");
  static_assert(std::is_integral<T>::value, "non-integral type");
  static_assert(!std::is_same<T, bool>::value, "bool type");

  // If X is a power of two X - Y = 1 + ((X - 1) ^ Y). Doing this transformation
  // allows GCC to remove its own xor that it adds to implement clz using bsr.
  // clang-format off
  using size = index_constant<constexpr_max(sizeof(T), sizeof(U0))>;
  return v ? 1u + static_cast<unsigned int>((8u * size{} - 1u) ^ (
      sizeof(T) <= sizeof(U0) ? __builtin_clz(bits_to_unsigned<U0>(v)) :
      sizeof(T) <= sizeof(U1) ? __builtin_clzl(bits_to_unsigned<U1>(v)) :
      sizeof(T) <= sizeof(U2) ? __builtin_clzll(bits_to_unsigned<U2>(v)) :
      0)) : 0u;
  // clang-format on
}

/// popcount
///
/// Returns the number of bits which are set.
template <typename T>
inline constexpr unsigned int popcount(T const v) {
  using U0 = unsigned int;
  using U1 = unsigned long int;
  using U2 = unsigned long long int;
  using detail::bits_to_unsigned;
  static_assert(sizeof(T) <= sizeof(U2), "over-sized type");
  static_assert(std::is_integral<T>::value, "non-integral type");
  static_assert(!std::is_same<T, bool>::value, "bool type");

  // clang-format off
  return static_cast<unsigned int>(
      sizeof(T) <= sizeof(U0) ? __builtin_popcount(bits_to_unsigned<U0>(v)) :
      sizeof(T) <= sizeof(U1) ? __builtin_popcountl(bits_to_unsigned<U1>(v)) :
      sizeof(T) <= sizeof(U2) ? __builtin_popcountll(bits_to_unsigned<U2>(v)) :
      0);
  // clang-format on
}

template <class T>
inline constexpr T nextPowTwo(T const v) {
  static_assert(std::is_unsigned<T>::value, "signed type");
  return v ? (T(1) << findLastSet(v - 1)) : T(1);
}

template <class T>
inline constexpr T prevPowTwo(T const v) {
  static_assert(std::is_unsigned<T>::value, "signed type");
  return v ? (T(1) << (findLastSet(v) - 1)) : T(0);
}

template <class T>
inline constexpr bool isPowTwo(T const v) {
  static_assert(std::is_integral<T>::value, "non-integral type");
  static_assert(std::is_unsigned<T>::value, "signed type");
  static_assert(!std::is_same<T, bool>::value, "bool type");
  return (v != 0) && !(v & (v - 1));
}

constexpr auto kIsLittleEndian = __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__;
constexpr auto kIsBigEndian = !kIsLittleEndian;

/**
 * Endianness detection and manipulation primitives.
 */
namespace detail {

template <size_t Size>
struct uint_types_by_size;

#define ACC_GEN(sz, fn)                                      \
  static inline uint##sz##_t byteswap_gen(uint##sz##_t v) { \
    return fn(v);                                           \
  }                                                         \
  template <>                                               \
  struct uint_types_by_size<sz / 8> {                       \
    using type = uint##sz##_t;                              \
  };

ACC_GEN(8, uint8_t)
#ifdef _MSC_VER
ACC_GEN(64, _byteswap_uint64)
ACC_GEN(32, _byteswap_ulong)
ACC_GEN(16, _byteswap_ushort)
#else
ACC_GEN(64, __builtin_bswap64)
ACC_GEN(32, __builtin_bswap32)
ACC_GEN(16, __builtin_bswap16)
#endif

#undef ACC_GEN

template <class T>
struct EndianInt {
  static_assert(
      (std::is_integral<T>::value && !std::is_same<T, bool>::value) ||
          std::is_floating_point<T>::value,
      "template type parameter must be non-bool integral or floating point");
  static T swap(T x) {
    // we implement this with memcpy because that is defined behavior in C++
    // we rely on compilers to optimize away the memcpy calls
    constexpr auto s = sizeof(T);
    using B = typename uint_types_by_size<s>::type;
    B b;
    std::memcpy(&b, &x, s);
    b = byteswap_gen(b);
    std::memcpy(&x, &b, s);
    return x;
  }
  static T big(T x) {
    return kIsLittleEndian ? EndianInt::swap(x) : x;
  }
  static T little(T x) {
    return kIsBigEndian ? EndianInt::swap(x) : x;
  }
};

} // namespace detail

// big* convert between native and big-endian representations
// little* convert between native and little-endian representations
// swap* convert between big-endian and little-endian representations
//
// ntohs, htons == big16
// ntohl, htonl == big32
#define ACC_GEN1(fn, t, sz) \
  static t fn##sz(t x) { return fn<t>(x); } \

#define ACC_GEN2(t, sz) \
  ACC_GEN1(swap, t, sz) \
  ACC_GEN1(big, t, sz) \
  ACC_GEN1(little, t, sz)

#define ACC_GEN(sz) \
  ACC_GEN2(uint##sz##_t, sz) \
  ACC_GEN2(int##sz##_t, sz)

class Endian {
 public:
  enum class Order : uint8_t {
    LITTLE,
    BIG
  };

  static constexpr Order order = kIsLittleEndian ? Order::LITTLE : Order::BIG;

  template <class T> static T swap(T x) {
    return detail::EndianInt<T>::swap(x);
  }
  template <class T> static T big(T x) {
    return detail::EndianInt<T>::big(x);
  }
  template <class T> static T little(T x) {
    return detail::EndianInt<T>::little(x);
  }

  ACC_GEN(64)
  ACC_GEN(32)
  ACC_GEN(16)
  ACC_GEN(8)
};

#undef ACC_GEN
#undef ACC_GEN2
#undef ACC_GEN1

template <class T, class Enable=void> struct Unaligned;

/**
 * Representation of an unaligned value of a POD type.
 */
template <class T>
struct Unaligned<
    T,
    typename std::enable_if<std::is_pod<T>::value>::type> {
  Unaligned() = default;  // uninitialized
  /* implicit */ Unaligned(T v) : value(v) { }
  T value;
} __attribute__((__packed__));

/**
 * Read an unaligned value of type T and return it.
 */
template <class T>
inline T loadUnaligned(const void* p) {
  static_assert(sizeof(Unaligned<T>) == sizeof(T), "Invalid unaligned size");
  static_assert(alignof(Unaligned<T>) == 1, "Invalid alignment");
  return static_cast<const Unaligned<T>*>(p)->value;
}

/**
 * Write an unaligned value of type T.
 */
template <class T>
inline void storeUnaligned(void* p, T value) {
  static_assert(sizeof(Unaligned<T>) == sizeof(T), "Invalid unaligned size");
  static_assert(alignof(Unaligned<T>) == 1, "Invalid alignment");
  new (p) Unaligned<T>(value);
}

} // namespace acc
