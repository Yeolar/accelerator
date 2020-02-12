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

#include <cstddef>
#include <exception>
#include <functional>
#include <new>

#include "accelerator/CPortability.h"
#include "accelerator/Macro.h"

namespace __cxxabiv1 {
// forward declaration (originally defined in unwind-cxx.h from from libstdc++)
struct __cxa_eh_globals;
// declared in cxxabi.h from libstdc++-v3
extern "C" __cxa_eh_globals* __cxa_get_globals() noexcept;
} // namespace __cxxabiv1

namespace acc {

/**
 * Returns the number of uncaught exceptions.
 *
 * This function is based on Evgeny Panasyuk's implementation from here:
 * http://fburl.com/15190026
 */
inline int uncaught_exceptions() noexcept {
  // __cxa_get_globals returns a __cxa_eh_globals* (defined in unwind-cxx.h).
  // The offset below returns __cxa_eh_globals::uncaughtExceptions.
  return *(reinterpret_cast<unsigned int*>(static_cast<char*>(
      static_cast<void*>(__cxxabiv1::__cxa_get_globals())) + sizeof(void*)));
}

namespace detail {

class ScopeGuardImplBase {
 public:
  void dismiss() noexcept {
    dismissed_ = true;
  }

  template <typename T>
  ACC_ALWAYS_INLINE static void runAndWarnAboutToCrashOnException(
      T& function) noexcept {
    try {
      function();
    } catch (...) {
      warnAboutToCrash();
      std::terminate();
    }
  }

 protected:
  ScopeGuardImplBase() noexcept : dismissed_(false) {}

  static ScopeGuardImplBase makeEmptyScopeGuard() noexcept {
    return ScopeGuardImplBase{};
  }

  template <typename T>
  static const T& asConst(const T& t) noexcept {
    return t;
  }

  bool dismissed_;

 private:
  static void warnAboutToCrash() noexcept;
};

template <typename FunctionType>
class ScopeGuardImpl : public ScopeGuardImplBase {
 public:
  explicit ScopeGuardImpl(FunctionType& fn) noexcept(
      std::is_nothrow_copy_constructible<FunctionType>::value)
      : ScopeGuardImpl(
            asConst(fn),
            makeFailsafe(std::is_nothrow_copy_constructible<FunctionType>{},
                         &fn)) {}

  explicit ScopeGuardImpl(const FunctionType& fn) noexcept(
      std::is_nothrow_copy_constructible<FunctionType>::value)
      : ScopeGuardImpl(
            fn,
            makeFailsafe(std::is_nothrow_copy_constructible<FunctionType>{},
                         &fn)) {}

  explicit ScopeGuardImpl(FunctionType&& fn) noexcept(
      std::is_nothrow_move_constructible<FunctionType>::value)
      : ScopeGuardImpl(
            std::move_if_noexcept(fn),
            makeFailsafe(std::is_nothrow_move_constructible<FunctionType>{},
                         &fn)) {}

  ScopeGuardImpl(ScopeGuardImpl&& other) noexcept(
      std::is_nothrow_move_constructible<FunctionType>::value)
      : function_(std::move_if_noexcept(other.function_)) {
    // If the above line attempts a copy and the copy throws, other is
    // left owning the cleanup action and will execute it (or not) depending
    // on the value of other.dismissed_. The following lines only execute
    // if the move/copy succeeded, in which case *this assumes ownership of
    // the cleanup action and dismisses other.
    dismissed_ = other.dismissed_;
    other.dismissed_ = true;
  }

  ~ScopeGuardImpl() noexcept {
    if (!dismissed_) {
      execute();
    }
  }

 private:
  static ScopeGuardImplBase makeFailsafe(std::true_type, const void*) noexcept {
    return makeEmptyScopeGuard();
  }

  template <typename Fn>
  static auto makeFailsafe(std::false_type, Fn* fn) noexcept
      -> ScopeGuardImpl<decltype(std::ref(*fn))> {
    return ScopeGuardImpl<decltype(std::ref(*fn))>{std::ref(*fn)};
  }

  template <typename Fn>
  explicit ScopeGuardImpl(Fn&& fn, ScopeGuardImplBase&& failsafe)
      : ScopeGuardImplBase{}, function_(std::forward<Fn>(fn)) {
    failsafe.dismiss();
  }

  void* operator new(std::size_t) = delete;

  void execute() noexcept {
    runAndWarnAboutToCrashOnException(function_);
  }

  FunctionType function_;
};

template <typename F>
using ScopeGuardImplDecay = ScopeGuardImpl<typename std::decay<F>::type>;

} // namespace detail

/**
 * ScopeGuard is a general implementation of the "Initialization is
 * Resource Acquisition" idiom.  Basically, it guarantees that a function
 * is executed upon leaving the currrent scope unless otherwise told.
 *
 * The makeGuard() function is used to create a new ScopeGuard object.
 * It can be instantiated with a lambda function, a std::function<void()>,
 * a functor, or a void(*)() function pointer.
 *
 *
 * Usage example: Add a friend to memory if and only if it is also added
 * to the db.
 *
 * void User::addFriend(User& newFriend) {
 *   // add the friend to memory
 *   friends_.push_back(&newFriend);
 *
 *   // If the db insertion that follows fails, we should
 *   // remove it from memory.
 *   auto guard = makeGuard([&] { friends_.pop_back(); });
 *
 *   // this will throw an exception upon error, which
 *   // makes the ScopeGuard execute UserCont::pop_back()
 *   // once the Guard's destructor is called.
 *   db_->addFriend(GetName(), newFriend.GetName());
 *
 *   // an exception was not thrown, so don't execute
 *   // the Guard.
 *   guard.dismiss();
 * }
 *
 * Examine ScopeGuardTest.cpp for some more sample usage.
 *
 * Stolen from:
 *   Andrei's and Petru Marginean's CUJ article:
 *     http://drdobbs.com/184403758
 *   and the loki library:
 *     http://loki-lib.sourceforge.net/index.php?n=Idioms.ScopeGuardPointer
 *   and triendl.kj article:
 *     http://www.codeproject.com/KB/cpp/scope_guard.aspx
 */
template <typename F>
detail::ScopeGuardImplDecay<F> makeGuard(F&& f) noexcept(
    noexcept(detail::ScopeGuardImplDecay<F>(static_cast<F&&>(f)))) {
  return detail::ScopeGuardImplDecay<F>(static_cast<F&&>(f));
}

namespace detail {

/**
 * ScopeGuard used for executing a function when leaving the current scope
 * depending on the presence of a new uncaught exception.
 *
 * If the executeOnException template parameter is true, the function is
 * executed if a new uncaught exception is present at the end of the scope.
 * If the parameter is false, then the function is executed if no new uncaught
 * exceptions are present at the end of the scope.
 *
 * Used to implement SCOPE_FAIL and SCOPE_SUCCESS below.
 */
template <typename FunctionType, bool ExecuteOnException>
class ScopeGuardForNewException {
 public:
  explicit ScopeGuardForNewException(const FunctionType& fn) : function_(fn) {}

  explicit ScopeGuardForNewException(FunctionType&& fn)
      : function_(std::move(fn)) {}

  ScopeGuardForNewException(ScopeGuardForNewException&& other) = default;

  ~ScopeGuardForNewException() noexcept(ExecuteOnException) {
    if (ExecuteOnException == (exceptionCounter_ < uncaught_exceptions())) {
      if (ExecuteOnException) {
        ScopeGuardImplBase::runAndWarnAboutToCrashOnException(function_);
      } else {
        function_();
      }
    }
  }

 private:
  ScopeGuardForNewException(const ScopeGuardForNewException& other) = delete;

  void* operator new(std::size_t) = delete;

  FunctionType function_;
  int exceptionCounter_{uncaught_exceptions()};
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
