/*
 * Copyright 2018-present Yeolar
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

#include <sys/uio.h>
#include <gflags/gflags.h>

#include "accelerator/accelerator-config.h"
#include "accelerator/CPortability.h"

#if ACC_UNUSUAL_GFLAGS_NAMESPACE
namespace ACC_GFLAGS_NAMESPACE { }
namespace gflags {
  using namespace ACC_GFLAGS_NAMESPACE;
}  // namespace gflags
#endif

//////////////////////////////////////////////////////////////////////

#if defined(__APPLE__)
#define ACC_STATIC_CTOR_PRIORITY_MAX
#else
// 101 is the highest priority allowed by the init_priority attribute.
// This priority is already used by JEMalloc and other memory allocators so
// we will take the next one.
#define ACC_STATIC_CTOR_PRIORITY_MAX __attribute__((__init_priority__(102)))
#endif

//////////////////////////////////////////////////////////////////////

// Define ACC_USE_CPP14_CONSTEXPR to be true if the compiler's C++14
// constexpr support is "good enough".
#ifndef ACC_USE_CPP14_CONSTEXPR
#if defined(__clang__)
#define ACC_USE_CPP14_CONSTEXPR __cplusplus >= 201300L
#elif defined(__GNUC__)
#define ACC_USE_CPP14_CONSTEXPR __cplusplus >= 201304L
#else
#define ACC_USE_CPP14_CONSTEXPR 0 // MSVC?
#endif
#endif

#if ACC_USE_CPP14_CONSTEXPR
#define ACC_CPP14_CONSTEXPR constexpr
#else
#define ACC_CPP14_CONSTEXPR inline
#endif

#define ACC_STORAGE_CONSTEXPR constexpr
#if ACC_USE_CPP14_CONSTEXPR
#define ACC_STORAGE_CPP14_CONSTEXPR constexpr
#else
#define ACC_STORAGE_CPP14_CONSTEXPR
#endif

//////////////////////////////////////////////////////////////////////

inline void asm_volatile_memory() {
#if defined(__clang__) || defined(__GNUC__)
  asm volatile("" : : : "memory");
#endif
}

inline void asm_volatile_pause() {
#if defined(__i386__) || defined(__x86_64__)
  asm volatile("pause");
#endif
}

//////////////////////////////////////////////////////////////////////

#include <string.h>

#if !ACC_HAVE_MEMRCHR
extern "C" void* memrchr(const void* s, int c, size_t n);
#endif

#if !ACC_HAVE_PREADV
extern "C" ssize_t preadv(int fd, const iovec* iov, int count, off_t offset);
#endif
#if !ACC_HAVE_PWRITEV
extern "C" ssize_t pwritev(int fd, const iovec* iov, int count, off_t offset);
#endif
