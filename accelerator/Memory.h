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

#include <cassert>
#include <cerrno>
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <limits>
#include <memory>
#include <stdexcept>
#include <utility>

namespace acc {

#if _POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600 || \
    (defined(__APPLE__) &&                                \
     (__MAC_OS_X_VERSION_MIN_REQUIRED >= __MAC_10_6 ||    \
      __IPHONE_OS_VERSION_MIN_REQUIRED >= __IPHONE_3_0))

inline void* aligned_malloc(size_t size, size_t align) {
  // use posix_memalign, but mimic the behaviour of memalign
  void* ptr = nullptr;
  int rc = posix_memalign(&ptr, align, size);
  return rc == 0 ? (errno = 0, ptr) : (errno = rc, nullptr);
}

inline void aligned_free(void* aligned_ptr) {
  free(aligned_ptr);
}

#else

inline void* aligned_malloc(size_t size, size_t align) {
  return memalign(align, size);
}

inline void aligned_free(void* aligned_ptr) {
  free(aligned_ptr);
}

#endif

/**
 * static_function_deleter
 *
 * So you can write this:
 *
 *      using RSA_deleter = acc::static_function_deleter<RSA, &RSA_free>;
 *      auto rsa = std::unique_ptr<RSA, RSA_deleter>(RSA_new());
 *      RSA_generate_key_ex(rsa.get(), bits, exponent, nullptr);
 *      rsa = nullptr;  // calls RSA_free(rsa.get())
 *
 * This would be sweet as well for BIO, but unfortunately BIO_free has signature
 * int(BIO*) while we require signature void(BIO*). So you would need to make a
 * wrapper for it:
 *
 *      inline void BIO_free_fb(BIO* bio) { CHECK_EQ(1, BIO_free(bio)); }
 *      using BIO_deleter = acc::static_function_deleter<BIO, &BIO_free_fb>;
 *      auto buf = std::unique_ptr<BIO, BIO_deleter>(BIO_new(BIO_s_mem()));
 *      buf = nullptr;  // calls BIO_free(buf.get())
 */

template <typename T, void (*f)(T*)>
struct static_function_deleter {
  void operator()(T* t) const {
    f(t);
  }
};

namespace detail {
template <typename T>
struct lift_void_to_char {
  using type = T;
};
template <>
struct lift_void_to_char<void> {
  using type = char;
};
} // namespace detail

/**
 * SysAllocator
 *
 * Resembles std::allocator, the default Allocator, but wraps std::malloc and
 * std::free.
 */
template <typename T>
class SysAllocator {
 private:
  using Self = SysAllocator<T>;

 public:
  using value_type = T;

  T* allocate(size_t count) {
    using lifted = typename detail::lift_void_to_char<T>::type;
    auto const p = std::malloc(sizeof(lifted) * count);
    if (!p) {
      throw std::bad_alloc();
    }
    return static_cast<T*>(p);
  }
  void deallocate(T* p, size_t /* count */) {
    std::free(p);
  }

  friend bool operator==(Self const&, Self const&) noexcept {
    return true;
  }
  friend bool operator!=(Self const&, Self const&) noexcept {
    return false;
  }
};

/**
 * CxxAllocatorAdaptor
 *
 * A type conforming to C++ concept Allocator, delegating operations to an
 * unowned Inner which has this required interface:
 *
 *   void* allocate(std::size_t)
 *   void deallocate(void*, std::size_t)
 *
 * Note that Inner is *not* a C++ Allocator.
 */
template <typename T, class Inner>
class CxxAllocatorAdaptor {
 private:
  using Self = CxxAllocatorAdaptor<T, Inner>;

  template <typename U, typename UAlloc>
  friend class CxxAllocatorAdaptor;

  std::reference_wrapper<Inner> ref_;

 public:
  using value_type = T;

  using propagate_on_container_copy_assignment = std::true_type;
  using propagate_on_container_move_assignment = std::true_type;
  using propagate_on_container_swap = std::true_type;

  explicit CxxAllocatorAdaptor(Inner& ref) : ref_(ref) {}

  template <typename U>
  explicit CxxAllocatorAdaptor(CxxAllocatorAdaptor<U, Inner> const& other)
      : ref_(other.ref_) {}

  T* allocate(std::size_t n) {
    using lifted = typename detail::lift_void_to_char<T>::type;
    return static_cast<T*>(ref_.get().allocate(sizeof(lifted) * n));
  }
  void deallocate(T* p, std::size_t n) {
    using lifted = typename detail::lift_void_to_char<T>::type;
    ref_.get().deallocate(p, sizeof(lifted) * n);
  }

  friend bool operator==(Self const& a, Self const& b) noexcept {
    return std::addressof(a.ref_.get()) == std::addressof(b.ref_.get());
  }
  friend bool operator!=(Self const& a, Self const& b) noexcept {
    return std::addressof(a.ref_.get()) != std::addressof(b.ref_.get());
  }
};

/**
 * AllocatorHasTrivialDeallocate
 *
 * Unambiguously inherits std::integral_constant<bool, V> for some bool V.
 *
 * Describes whether a C++ Aallocator has trivial, i.e. no-op, deallocate().
 *
 * Also may be used to describe types which may be used with
 * CxxAllocatorAdaptor.
 */
template <typename Alloc>
struct AllocatorHasTrivialDeallocate : std::false_type {};

template <typename T, class Alloc>
struct AllocatorHasTrivialDeallocate<CxxAllocatorAdaptor<T, Alloc>>
    : AllocatorHasTrivialDeallocate<Alloc> {};

} // namespace acc
