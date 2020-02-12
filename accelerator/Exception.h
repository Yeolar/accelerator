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

#include <cerrno>
#include <string>
#include <system_error>
#include <vector>

#include "accelerator/Conv.h"

namespace acc {

// Helper to throw std::system_error
[[noreturn]] inline void throwSystemErrorExplicit(int err, const char* msg) {
  throw std::system_error(err, std::system_category(), msg);
}

template <class... Args>
[[noreturn]] void throwSystemErrorExplicit(int err, Args&&... args) {
  throwSystemErrorExplicit(
    err, to<std::string>(std::forward<Args>(args)...).c_str());
}

// Helper to throw std::system_error from errno and components of a string
template <class... Args>
[[noreturn]] void throwSystemError(Args&&... args) {
  throwSystemErrorExplicit(errno, std::forward<Args>(args)...);
}

// Check a Posix return code (0 on success, error number on error), throw
// on error.
template <class... Args>
void checkPosixError(int err, Args&&... args) {
  if (ACC_UNLIKELY(err != 0)) {
    throwSystemErrorExplicit(err, std::forward<Args>(args)...);
  }
}

// Check a Linux kernel-style return code (>= 0 on success, negative error
// number on error), throw on error.
template <class... Args>
void checkKernelError(ssize_t ret, Args&&... args) {
  if (ACC_UNLIKELY(ret < 0)) {
    throwSystemErrorExplicit(int(-ret), std::forward<Args>(args)...);
  }
}

// Check a traditional Unix return code (-1 and sets errno on error), throw
// on error.
template <class... Args>
void checkUnixError(ssize_t ret, Args&&... args) {
  if (ACC_UNLIKELY(ret == -1)) {
    throwSystemError(std::forward<Args>(args)...);
  }
}

template <class... Args>
void checkUnixErrorExplicit(ssize_t ret, int savedErrno, Args&&... args) {
  if (ACC_UNLIKELY(ret == -1)) {
    throwSystemErrorExplicit(savedErrno, std::forward<Args>(args)...);
  }
}

// Check the return code from a fopen-style function (returns a non-nullptr
// FILE* on success, nullptr on error, sets errno).  Works with fopen, fdopen,
// freopen, tmpfile, etc.
template <class... Args>
void checkFopenError(FILE* fp, Args&&... args) {
  if (ACC_UNLIKELY(!fp)) {
    throwSystemError(std::forward<Args>(args)...);
  }
}

template <class... Args>
void checkFopenErrorExplicit(FILE* fp, int savedErrno, Args&&... args) {
  if (ACC_UNLIKELY(!fp)) {
    throwSystemErrorExplicit(savedErrno, std::forward<Args>(args)...);
  }
}

/**
 * If cond is not true, raise an exception of type E.  E must have a ctor that
 * works with const char* (a description of the failure).
 */
#define ACC_CHECK_THROW(cond, E)                                            \
  do {                                                                      \
    if (!(cond)) {                                                          \
      throw E("Check failed: " #cond " @(", __FILE__, ":", __LINE__, ")");  \
    }                                                                       \
  } while (0)

//////////////////////////////////////////////////////////////////////

std::vector<std::string> recordBacktrace();

void recordBacktraceToStr(std::string& out);

template <class Tag = void>
class TracingExceptionBase : public std::exception {
 public:
  template <class... Args>
  explicit TracingExceptionBase(Args&&... args)
      : msg_(to<std::string>(std::forward<Args>(args)...)) {
    recordBacktraceToStr(msg_);
  }

  virtual ~TracingExceptionBase() {}

  virtual const char* what() const noexcept {
    return msg_.c_str();
  }

 private:
  std::string msg_;
};

#define ACC_TRACING_EXCEPTION(ex_name)                    \
  struct ex_name##Tag {};                                 \
  typedef acc::TracingExceptionBase<ex_name##Tag> ex_name

#define ACC_TRACING_THROW(E, ...) \
  throw E(#E, " @(", __FILE__, ":", __LINE__, "): \"", ##__VA_ARGS__, "\"")

// logic error
ACC_TRACING_EXCEPTION(LogicError);
ACC_TRACING_EXCEPTION(DomainError);
ACC_TRACING_EXCEPTION(InvalidArgument);
ACC_TRACING_EXCEPTION(LengthError);
ACC_TRACING_EXCEPTION(OutOfRange);

// runtime error
ACC_TRACING_EXCEPTION(RuntimeError);
ACC_TRACING_EXCEPTION(RangeError);
ACC_TRACING_EXCEPTION(OverflowError);
ACC_TRACING_EXCEPTION(UnderflowError);
ACC_TRACING_EXCEPTION(NotImplementedError);

} // namespace acc
