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

#include <cerrno>
#include <system_error>

#include "accelerator/Backtrace.h"
#include "accelerator/Conv.h"

namespace acc {

// Helper to throw std::system_error
inline void throwSystemErrorExplicit(int err, const char* msg) {
  throw std::system_error(err, std::system_category(), msg);
}

template <class... Args>
void throwSystemErrorExplicit(int err, Args&&... args) {
  throwSystemErrorExplicit(
    err, to<fbstring>(std::forward<Args>(args)...).c_str());
}

template <class... Args>
void throwSystemError(Args&&... args) {
  throwSystemErrorExplicit(errno, std::forward<Args>(args)...);
}

template <class... Args>
void checkPosixError(int err, Args&&... args) {
  if (UNLIKELY(err != 0)) {
    throwSystemErrorExplicit(err, std::forward<Args>(args)...);
  }
}

template <class... Args>
void checkUnixError(ssize_t ret, Args&&... args) {
  if (UNLIKELY(ret == -1)) {
    throwSystemError(std::forward<Args>(args)...);
  }
}

template <class... Args>
void checkFopenError(FILE* fp, Args&&... args) {
  if (UNLIKELY(!fp)) {
    throwSystemError(std::forward<Args>(args)...);
  }
}

template <typename E, typename V, typename... Args>
void throwOnFail(V&& value, Args&&... args) {
  if (!value) {
    throw E(std::forward<Args>(args)...);
  }
}

/**
 * If cond is not true, raise an exception of type E.  E must have a ctor that
 * works with const char* (a description of the failure).
 */
#define ACC_CHECK_THROW(cond, E) \
  ::acc::throwOnFail<E>((cond), "Check failed: " #cond \
                        " @(", __FILE__, ":", __LINE__, ")")

//////////////////////////////////////////////////////////////////////

template <class Tag = void, bool tracing = false>
class ExceptionBase : public std::exception {
 public:
  template <class... Args>
  explicit ExceptionBase(Args&&... args)
    : msg_(to<fbstring>(std::forward<Args>(args)...)) {
    if (tracing) {
      auto trace = recordBacktrace();
      toAppend(", trace info:", &msg_);
      for (auto& s : trace) {
        toAppend('\n', s, &msg_);
      }
    }
  }

  virtual ~ExceptionBase() {}

  virtual const char* what() const noexcept {
    return msg_.c_str();
  }

 private:
  fbstring msg_;
};

typedef ExceptionBase<> Exception;

#define ACC_EXCEPTION(ex_name) \
  struct ex_name##Tag {}; \
  typedef acc::ExceptionBase<ex_name##Tag> ex_name

#define ACC_TRACING_EXCEPTION(ex_name) \
  struct ex_name##Tag {}; \
  typedef acc::ExceptionBase<ex_name##Tag, true> ex_name

// logic error
ACC_TRACING_EXCEPTION(LogicError);
ACC_TRACING_EXCEPTION(DomainError);
ACC_TRACING_EXCEPTION(InvalidArgument);
ACC_TRACING_EXCEPTION(LengthError);
ACC_TRACING_EXCEPTION(OutOfRange);
ACC_TRACING_EXCEPTION(TypeError);

// runtime error
ACC_TRACING_EXCEPTION(RuntimeError);
ACC_TRACING_EXCEPTION(RangeError);
ACC_TRACING_EXCEPTION(OverflowError);
ACC_TRACING_EXCEPTION(UnderflowError);
ACC_TRACING_EXCEPTION(NotImplementedError);

#define ACC_THROW(E, ...) \
  throw E(#E, " @(", __FILE__, ":", __LINE__, "): \"", ##__VA_ARGS__, "\"")

} // namespace acc
