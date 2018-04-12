/*
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

#include <sys/time.h>

#include "accelerator/Benchmark.h"
#include "accelerator/Time.h"

using namespace acc;

inline uint64_t systemNanoTimestampNow() {
  timespec tv;
  clock_gettime(CLOCK_REALTIME, &tv);
  return tv.tv_sec * 1000000000 + tv.tv_nsec;
}

inline uint64_t systemTimestampNow() {
  timespec tv;
  clock_gettime(CLOCK_REALTIME, &tv);
  return tv.tv_sec * 1000000 + tv.tv_nsec / 1000;
}

// sudo nice -n -20 ./accelerator/util/test/accelerator_util_TimeBenchmark -bm_min_iters 1000000
// ============================================================================
// TimeBenchmark.cpp                               relative  time/iter  iters/s
// ============================================================================
// system_nano                                                 16.15ns   61.92M
// chrono_nano                                       95.08%    16.99ns   58.87M
// ----------------------------------------------------------------------------
// system_micro                                                17.26ns   57.92M
// chrono_micro                                      96.88%    17.82ns   56.11M
// ----------------------------------------------------------------------------
// time                                                         2.51ns  399.09M
// ============================================================================

BENCHMARK(system_nano, n) {
  for (unsigned i = 0; i < n; ++i) {
    auto t = systemNanoTimestampNow();
    acc::doNotOptimizeAway(t);
  }
}

BENCHMARK_RELATIVE(chrono_nano, n) {
  for (unsigned i = 0; i < n; ++i) {
    auto t = nanoTimestampNow();
    acc::doNotOptimizeAway(t);
  }
}

BENCHMARK_DRAW_LINE();

BENCHMARK(system_micro, n) {
  for (unsigned i = 0; i < n; ++i) {
    auto t = systemTimestampNow();
    acc::doNotOptimizeAway(t);
  }
}

BENCHMARK_RELATIVE(chrono_micro, n) {
  for (unsigned i = 0; i < n; ++i) {
    auto t = timestampNow();
    acc::doNotOptimizeAway(t);
  }
}

BENCHMARK_DRAW_LINE();

BENCHMARK(time, n) {
  for (unsigned i = 0; i < n; ++i) {
    auto t = time(nullptr);
    acc::doNotOptimizeAway(t);
  }
}

int main(int argc, char** argv) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  acc::runBenchmarks();
  return 0;
}
