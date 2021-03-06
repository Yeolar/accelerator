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

#include <string.h>
#include <cassert>
#include <functional>
#include <limits>
#include <type_traits>
#include <boost/function_types/function_arity.hpp>

#include "accelerator/Portability.h"
#include "accelerator/ScopeGuard.h"
#include "accelerator/Time.h"
#include "accelerator/Traits.h"

#ifndef __FILENAME__
#define __FILENAME__ ((strrchr(__FILE__, '/') ?: __FILE__ - 1) + 1)
#endif

namespace acc {

typedef std::function<
  std::pair<uint64_t, unsigned int>(unsigned int)> BenchmarkFun;

/**
 * Runs all benchmarks defined. Usually put in main().
 */
void runBenchmarks();

namespace detail {

void addBenchmarkImpl(const char* file, const char* name, BenchmarkFun);

} // namespace detail

/**
 * Supporting type for BENCHMARK_SUSPEND defined below.
 */
struct BenchmarkSuspender {
  BenchmarkSuspender() {
    start = nanoTimestampNow();
  }

  BenchmarkSuspender(const BenchmarkSuspender &) = delete;
  BenchmarkSuspender(BenchmarkSuspender && rhs) noexcept {
    start = rhs.start;
    rhs.start = 0;
  }

  BenchmarkSuspender& operator=(const BenchmarkSuspender &) = delete;
  BenchmarkSuspender& operator=(BenchmarkSuspender && rhs) {
    if (start != 0) {
      tally();
    }
    start = rhs.start;
    rhs.start = 0;
    return *this;
  }

  ~BenchmarkSuspender() {
    if (start != 0) {
      tally();
    }
  }

  void dismiss() {
    assert(start != 0);
    tally();
    start = 0;
  }

  void rehire() {
    assert(start == 0);
    start = nanoTimestampNow();
  }

  template <class F>
  auto dismissing(F f) -> typename std::result_of<F()>::type {
    SCOPE_EXIT { rehire(); };
    dismiss();
    return f();
  }

  explicit operator bool() const {
    return false;
  }

  static uint64_t timeSpent;

 private:
  void tally() {
    auto end = nanoTimestampNow();
    timeSpent += end - start;
    start = end;
  }

  uint64_t start;
};

/**
 * Adds a benchmark. Usually not called directly but instead through
 * the macro BENCHMARK defined below. The lambda function involved
 * must take exactly one parameter of type unsigned, and the benchmark
 * uses it with counter semantics (iteration occurs inside the
 * function).
 */
template <typename Lambda>
typename std::enable_if<
  boost::function_types::function_arity<decltype(&Lambda::operator())>::value
  == 2
>::type
addBenchmark(const char* file, const char* name, Lambda&& lambda) {
  auto execute = [=](unsigned int times) {
    BenchmarkSuspender::timeSpent = {};
    unsigned int niter;

    // CORE MEASUREMENT STARTS
    uint64_t start = nanoTimestampNow();
    niter = lambda(times);
    uint64_t end = nanoTimestampNow();
    // CORE MEASUREMENT ENDS

    return std::pair<uint64_t, unsigned int>(
        (end - start) - BenchmarkSuspender::timeSpent, niter);
  };

  detail::addBenchmarkImpl(file, name, BenchmarkFun(execute));
}

/**
 * Adds a benchmark. Usually not called directly but instead through
 * the macro BENCHMARK defined below. The lambda function involved
 * must take zero parameters, and the benchmark calls it repeatedly
 * (iteration occurs outside the function).
 */
template <typename Lambda>
typename std::enable_if<
  boost::function_types::function_arity<decltype(&Lambda::operator())>::value
  == 1
>::type
addBenchmark(const char* file, const char* name, Lambda&& lambda) {
  addBenchmark(file, name, [=](unsigned int times) {
      unsigned int niter = 0;
      while (times-- > 0) {
        niter += lambda();
      }
      return niter;
    });
}

/**
 * Call doNotOptimizeAway(var) to ensure that var will be computed even
 * post-optimization.  Use it for variables that are computed during
 * benchmarking but otherwise are useless. The compiler tends to do a
 * good job at eliminating unused variables, and this function fools it
 * into thinking var is in fact needed.
 */

namespace detail {
template <typename T>
struct DoNotOptimizeAwayNeedsIndirect {
  using Decayed = typename std::decay<T>::type;

  // First two constraints ensure it can be an "r" operand.
  // std::is_pointer check is because callers seem to expect that
  // doNotOptimizeAway(&x) is equivalent to doNotOptimizeAway(x).
  constexpr static bool value = !acc::IsTriviallyCopyable<Decayed>::value ||
      sizeof(Decayed) > sizeof(long) || std::is_pointer<Decayed>::value;
};
} // namespace detail

template <typename T>
auto doNotOptimizeAway(const T& datum) -> typename std::enable_if<
    !detail::DoNotOptimizeAwayNeedsIndirect<T>::value>::type {
  // The "r" constraint forces the compiler to make datum available
  // in a register to the asm block, which means that it must have
  // computed/loaded it.  We use this path for things that are <=
  // sizeof(long) (they have to fit), trivial (otherwise the compiler
  // doesn't want to put them in a register), and not a pointer (because
  // doNotOptimizeAway(&foo) would otherwise be a foot gun that didn't
  // necessarily compute foo).
  //
  // An earlier version of this method had a more permissive input operand
  // constraint, but that caused unnecessary variation between clang and
  // gcc benchmarks.
  asm volatile("" ::"r"(datum));
}

template <typename T>
auto doNotOptimizeAway(const T& datum) -> typename std::enable_if<
    detail::DoNotOptimizeAwayNeedsIndirect<T>::value>::type {
  // This version of doNotOptimizeAway tells the compiler that the asm
  // block will read datum from memory, and that in addition it might read
  // or write from any memory location.  If the memory clobber could be
  // separated into input and output that would be preferrable.
  asm volatile("" ::"m"(datum) : "memory");
}

} // namespace acc

/**
 * Introduces a benchmark function. Used internally, see BENCHMARK and
 * friends below.
 */
#define BENCHMARK_IMPL(funName, stringName, rv, paramType, paramName)   \
  static void funName(paramType);                                       \
  static bool ACC_ANONYMOUS_VARIABLE(accBenchmarkUnused) = (            \
    ::acc::addBenchmark(__FILENAME__, stringName,                       \
      [](paramType paramName) -> unsigned { funName(paramName);         \
                                            return rv; }),              \
    true);                                                              \
  static void funName(paramType paramName)

/**
 * Introduces a benchmark function. Use with either one or two arguments.
 * The first is the name of the benchmark. Use something descriptive, such
 * as insertVectorBegin. The second argument may be missing, or could be a
 * symbolic counter. The counter dictates how many internal iteration the
 * benchmark does. Example:
 *
 * BENCHMARK(vectorPushBack) {
 *   vector<int> v;
 *   v.push_back(42);
 * }
 *
 * BENCHMARK(insertVectorBegin, n) {
 *   vector<int> v;
 *   for (unsigned i = 0; i < n; ++i) {
 *     v.insert(v.begin(), 42);
 *   }
 * }
 */
#define BENCHMARK(name, ...)                                    \
  BENCHMARK_IMPL(                                               \
    name,                                                       \
    ACC_STRINGIZE(name),                                        \
    ACC_ARG_2_OR_1(1, ## __VA_ARGS__),                          \
    ACC_ARG_1_OR_NONE(unsigned, ## __VA_ARGS__),                \
    __VA_ARGS__)

/**
 * Defines a benchmark that passes a parameter to another one. This is
 * common for benchmarks that need a "problem size" in addition to
 * "number of iterations". Consider:
 *
 * void pushBack(uint n, size_t initialSize) {
 *   vector<int> v;
 *   BENCHMARK_SUSPEND {
 *     v.resize(initialSize);
 *   }
 *   for (unsigned i = 0; i < n; ++i) {
 *    v.push_back(i);
 *   }
 * }
 * BENCHMARK_PARAM(pushBack, 0)
 * BENCHMARK_PARAM(pushBack, 1000)
 * BENCHMARK_PARAM(pushBack, 1000000)
 *
 * The benchmark above estimates the speed of push_back at different
 * initial sizes of the vector. The framework will pass 0, 1000, and
 * 1000000 for initialSize, and the iteration count for n.
 */
#define BENCHMARK_PARAM(name, param)                            \
  BENCHMARK_NAMED_PARAM(name, param, param)

/*
 * Like BENCHMARK_PARAM(), but allows a custom name to be specified for each
 * parameter, rather than using the parameter value.
 *
 * Useful when the parameter value is not a valid token for string pasting,
 * of when you want to specify multiple parameter arguments.
 *
 * For example:
 *
 * void addValue(uint n, int64_t bucketSize, int64_t min, int64_t max) {
 *   Histogram<int64_t> hist(bucketSize, min, max);
 *   int64_t num = min;
 *   for (unsigned i = 0; i < n; ++i) {
 *     hist.addValue(num);
 *     ++num;
 *     if (num > max) { num = min; }
 *   }
 * }
 *
 * BENCHMARK_NAMED_PARAM(addValue, 0_to_100, 1, 0, 100)
 * BENCHMARK_NAMED_PARAM(addValue, 0_to_1000, 10, 0, 1000)
 * BENCHMARK_NAMED_PARAM(addValue, 5k_to_20k, 250, 5000, 20000)
 */
#define BENCHMARK_NAMED_PARAM(name, param_name, ...)            \
  BENCHMARK_IMPL(                                               \
      ACC_CONCATENATE(name, ACC_CONCATENATE(_, param_name)),    \
      ACC_STRINGIZE(name) "(" ACC_STRINGIZE(param_name) ")",    \
      iters,                                                    \
      unsigned,                                                 \
      iters) {                                                  \
    name(iters, ## __VA_ARGS__);                                \
  }

/**
 * Just like BENCHMARK, but prints the time relative to a
 * baseline. The baseline is the most recent BENCHMARK() seen in
 * the current scope. Example:
 *
 * // This is the baseline
 * BENCHMARK(insertVectorBegin, n) {
 *   vector<int> v;
 *   for (unsigned i = 0; i < n; ++i) {
 *     v.insert(v.begin(), 42);
 *   }
 * }
 *
 * BENCHMARK_RELATIVE(insertListBegin, n) {
 *   list<int> s;
 *   for (unsigned i = 0; i < n; ++i) {
 *     s.insert(s.begin(), 42);
 *   }
 * }
 *
 * Any number of relative benchmark can be associated with a
 * baseline. Another BENCHMARK() occurrence effectively establishes a
 * new baseline.
 */
#define BENCHMARK_RELATIVE(name, ...)                           \
  BENCHMARK_IMPL(                                               \
    name,                                                       \
    "%" ACC_STRINGIZE(name),                                    \
    ACC_ARG_2_OR_1(1, ## __VA_ARGS__),                          \
    ACC_ARG_1_OR_NONE(unsigned, ## __VA_ARGS__),                \
    __VA_ARGS__)

/**
 * A combination of BENCHMARK_RELATIVE and BENCHMARK_PARAM.
 */
#define BENCHMARK_RELATIVE_PARAM(name, param)                   \
  BENCHMARK_RELATIVE_NAMED_PARAM(name, param, param)

/**
 * A combination of BENCHMARK_RELATIVE and BENCHMARK_NAMED_PARAM.
 */
#define BENCHMARK_RELATIVE_NAMED_PARAM(name, param_name, ...)   \
  BENCHMARK_IMPL(                                               \
      ACC_CONCATENATE(name, ACC_CONCATENATE(_, param_name)),    \
      "%" ACC_STRINGIZE(name) "(" ACC_STRINGIZE(param_name) ")",\
      iters,                                                    \
      unsigned,                                                 \
      iters) {                                                  \
    name(iters, ## __VA_ARGS__);                                \
  }

/**
 * Draws a line of dashes.
 */
#define BENCHMARK_DRAW_LINE()                                   \
  static bool ACC_ANONYMOUS_VARIABLE(accBenchmarkUnused) = (    \
    ::acc::addBenchmark(__FILENAME__, "-",                      \
      []() -> unsigned { return 0; }),                          \
    true);

/**
 * Allows execution of code that doesn't count torward the benchmark's
 * time budget. Example:
 *
 * BENCHMARK(insertVectorBegin, n) {
 *   vector<int> v;
 *   BENCHMARK_SUSPEND {
 *     v.reserve(n);
 *   }
 *   for (unsigned i = 0; i < n; ++i) {
 *     v.insert(v.begin(), 42);
 *   }
 * }
 */
#define BENCHMARK_SUSPEND                                       \
  if (auto ACC_ANONYMOUS_VARIABLE(BENCHMARK_SUSPEND) =          \
      ::acc::BenchmarkSuspender()) {}                           \
  else

