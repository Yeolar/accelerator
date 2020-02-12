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

/**
 * Portable version check.
 */
#ifndef __GNUC_PREREQ
# if defined __GNUC__ && defined __GNUC_MINOR__
/* nolint */
#  define __GNUC_PREREQ(maj, min) ((__GNUC__ << 16) + __GNUC_MINOR__ >= \
                                   ((maj) << 16) + (min))
# else
/* nolint */
#  define __GNUC_PREREQ(maj, min) 0
# endif
#endif

// portable version check for clang
#ifndef __CLANG_PREREQ
# if defined __clang__ && defined __clang_major__ && defined __clang_minor__
/* nolint */
#  define __CLANG_PREREQ(maj, min) \
    ((__clang_major__ << 16) + __clang_minor__ >= ((maj) << 16) + (min))
# else
/* nolint */
#  define __CLANG_PREREQ(maj, min) 0
# endif
#endif

#if defined(__has_builtin)
#define ACC_HAS_BUILTIN(...) __has_builtin(__VA_ARGS__)
#else
#define ACC_HAS_BUILTIN(...) 0
#endif

#if defined(__has_feature)
#define ACC_HAS_FEATURE(...) __has_feature(__VA_ARGS__)
#else
#define ACC_HAS_FEATURE(...) 0
#endif

#if defined(__has_include)
#define ACC_HAS_INCLUDE(...) __has_include(__VA_ARGS__)
#else
#define ACC_HAS_INCLUDE(...) 0
#endif

/* Define a convenience macro to test when address sanitizer is being used
 * across the different compilers (e.g. clang, gcc) */
#if ACC_HAS_FEATURE(address_sanitizer) || __SANITIZE_ADDRESS__
# define ACC_SANITIZE_ADDRESS 1
#endif

/* Define attribute wrapper for function attribute used to disable
 * address sanitizer instrumentation. Unfortunately, this attribute
 * has issues when inlining is used, so disable that as well. */
#ifdef ACC_SANITIZE_ADDRESS
# if defined(__clang__)
#  if __has_attribute(__no_sanitize__)
#   define ACC_DISABLE_ADDRESS_SANITIZER \
      __attribute__((__no_sanitize__("address"), __noinline__))
#  elif __has_attribute(__no_address_safety_analysis__)
#   define ACC_DISABLE_ADDRESS_SANITIZER \
      __attribute__((__no_address_safety_analysis__, __noinline__))
#  elif __has_attribute(__no_sanitize_address__)
#   define ACC_DISABLE_ADDRESS_SANITIZER \
      __attribute__((__no_sanitize_address__, __noinline__))
#  endif
# elif defined(__GNUC__)
#  define ACC_DISABLE_ADDRESS_SANITIZER \
     __attribute__((__no_address_safety_analysis__, __noinline__))
# endif
#endif
#ifndef ACC_DISABLE_ADDRESS_SANITIZER
# define ACC_DISABLE_ADDRESS_SANITIZER
#endif

/* Define a convenience macro to test when thread sanitizer is being used
 * across the different compilers (e.g. clang, gcc) */
#if ACC_HAS_FEATURE(thread_sanitizer) || __SANITIZE_THREAD__
# define ACC_SANITIZE_THREAD 1
#endif

/**
 * Define a convenience macro to test when ASAN, UBSAN or TSAN sanitizer are
 * being used
 */
#if defined(ACC_SANITIZE_ADDRESS) || defined(ACC_SANITIZE_THREAD)
#define ACC_SANITIZE 1
#endif

#if ACC_SANITIZE
#define ACC_DISABLE_UNDEFINED_BEHAVIOR_SANITIZER(...) \
  __attribute__((no_sanitize(__VA_ARGS__)))
#else
#define ACC_DISABLE_UNDEFINED_BEHAVIOR_SANITIZER(...)
#endif // ACC_SANITIZE

/**
 * Macro for marking functions as having public visibility.
 */
#if defined(__GNUC__)
# if __GNUC_PREREQ(4, 9)
#  define ACC_EXPORT [[gnu::visibility("default")]]
# else
#  define ACC_EXPORT __attribute__((__visibility__("default")))
# endif
#else
# define ACC_EXPORT
#endif

// noinline
#if defined(__clang__) || defined(__GNUC__)
# define ACC_NOINLINE __attribute__((__noinline__))
#else
# define ACC_NOINLINE
#endif

// always inline
#if defined(__clang__) || defined(__GNUC__)
# define ACC_ALWAYS_INLINE inline __attribute__((__always_inline__))
#else
# define ACC_ALWAYS_INLINE inline
#endif

// attribute hidden
#if defined(__clang__) || defined(__GNUC__)
#define ACC_ATTR_VISIBILITY_HIDDEN __attribute__((__visibility__("hidden")))
#else
#define ACC_ATTR_VISIBILITY_HIDDEN
#endif

// An attribute for marking symbols as weak, if supported
#if ACC_HAVE_WEAK_SYMBOLS
#define ACC_ATTR_WEAK __attribute__((__weak__))
#else
#define ACC_ATTR_WEAK
#endif
