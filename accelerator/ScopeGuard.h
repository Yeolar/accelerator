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

#include <cstddef>
#include <exception>
#include <functional>
#include <new>

#include "accelerator/Macro.h"

namespace __cxxabiv1 {
// forward declaration (originally defined in unwind-cxx.h from from libstdc++)
struct __cxa_eh_globals;
// declared in cxxabi.h from libstdc++-v3
extern "C" __cxa_eh_globals* __cxa_get_globals() noexcept;
}

namespace acc {

namespace detail {

class UncaughtExceptionCounter {
 public:
  UncaughtExceptionCounter()
    : exceptionCount_(getUncaughtExceptionCount()) {}

  UncaughtExceptionCounter(const UncaughtExceptionCounter& other)
    : exceptionCount_(other.exceptionCount_) {}

  bool isNewUncaughtException() noexcept {
    return getUncaughtExceptionCount() > exceptionCount_;
  }

 private:
  int getUncaughtExceptionCount() noexcept;

  int exceptionCount_;
};

/*
 * This function is based on Evgeny Panasyuk's implementation from here:
 * http://fburl.com/15190026
 */
inline int UncaughtExceptionCounter::getUncaughtExceptionCount() noexcept {
  // __cxa_get_globals returns a __cxa_eh_globals* (defined in unwind-cxx.h).
  // The offset below returns __cxa_eh_globals::uncaughtExceptions.
  return *(reinterpret_cast<unsigned int*>(static_cast<char*>(
      static_cast<void*>(__cxxabiv1::__cxa_get_globals())) + sizeof(void*)));
}

} // namespace detail

class ScopeGuardImplBase {
 public:
  void dismiss() noexcept {
    dismissed_ = true;
  }

 protected:
  ScopeGuardImplBase()
    : dismissed_(false) {}

  ScopeGuardImplBase(ScopeGuardImplBase&& other) noexcept
    : dismissed_(other.dismissed_) {
    other.dismissed_ = true;
  }

  bool dismissed_;
};

template <typename FunctionType>
class ScopeGuardImpl : public ScopeGuardImplBase {
 public:
  explicit ScopeGuardImpl(const FunctionType& fn)
    : function_(fn) {}

  explicit ScopeGuardImpl(FunctionType&& fn)
    : function_(std::move(fn)) {}

  ScopeGuardImpl(ScopeGuardImpl&& other)
    : ScopeGuardImplBase(std::move(other))
    , function_(std::move(other.function_)) {
  }

  ~ScopeGuardImpl() noexcept {
    if (!dismissed_) {
      execute();
    }
  }

 private:
  void* operator new(size_t) = delete;

  void execute() noexcept { function_(); }

  FunctionType function_;
};

template <typename FunctionType>
ScopeGuardImpl<typename std::decay<FunctionType>::type>
makeGuard(FunctionType&& fn) {
  return ScopeGuardImpl<typename std::decay<FunctionType>::type>(
      std::forward<FunctionType>(fn));
}

/**
 * This is largely unneeded if you just use auto for your guards.
 */
typedef ScopeGuardImplBase&& ScopeGuard;

namespace detail {

template <typename FunctionType, bool executeOnException>
class ScopeGuardForNewException {
 public:
  explicit ScopeGuardForNewException(const FunctionType& fn)
      : function_(fn) {
  }

  explicit ScopeGuardForNewException(FunctionType&& fn)
      : function_(std::move(fn)) {
  }

  ScopeGuardForNewException(ScopeGuardForNewException&& other)
      : function_(std::move(other.function_))
      , exceptionCounter_(std::move(other.exceptionCounter_)) {
  }

  ~ScopeGuardForNewException() noexcept(executeOnException) {
    if (executeOnException == exceptionCounter_.isNewUncaughtException()) {
      function_();
    }
  }

 private:
  ScopeGuardForNewException(const ScopeGuardForNewException& other) = delete;

  void* operator new(size_t) = delete;

  FunctionType function_;
  UncaughtExceptionCounter exceptionCounter_;
};

/**
 * Internal use for the macro SCOPE_FAIL below
 */
enum class ScopeGuardOnFail {};

template <typename FunctionType>
ScopeGuardForNewException<typename std::decay<FunctionType>::type, true>
operator+(detail::ScopeGuardOnFail, FunctionType&& fn) {
  return
      ScopeGuardForNewException<typename std::decay<FunctionType>::type, true>(
      std::forward<FunctionType>(fn));
}

/**
 * Internal use for the macro SCOPE_SUCCESS below
 */
enum class ScopeGuardOnSuccess {};

template <typename FunctionType>
ScopeGuardForNewException<typename std::decay<FunctionType>::type, false>
operator+(ScopeGuardOnSuccess, FunctionType&& fn) {
  return
      ScopeGuardForNewException<typename std::decay<FunctionType>::type, false>(
      std::forward<FunctionType>(fn));
}

/**
 * Internal use for the macro SCOPE_EXIT below
 */
enum class ScopeGuardOnExit {};

template <typename FunctionType>
ScopeGuardImpl<typename std::decay<FunctionType>::type>
operator+(detail::ScopeGuardOnExit, FunctionType&& fn) {
  return ScopeGuardImpl<typename std::decay<FunctionType>::type>(
      std::forward<FunctionType>(fn));
}

} // namespace detail

} // namespace acc

#define SCOPE_EXIT \
  auto ACC_ANONYMOUS_VARIABLE(SCOPE_EXIT_STATE) \
  = ::acc::detail::ScopeGuardOnExit() + [&]() noexcept

#define SCOPE_FAIL \
  auto ACC_ANONYMOUS_VARIABLE(SCOPE_FAIL_STATE) \
  = ::acc::detail::ScopeGuardOnFail() + [&]() noexcept

#define SCOPE_SUCCESS \
  auto ACC_ANONYMOUS_VARIABLE(SCOPE_SUCCESS_STATE) \
  = ::acc::detail::ScopeGuardOnSuccess() + [&]()

