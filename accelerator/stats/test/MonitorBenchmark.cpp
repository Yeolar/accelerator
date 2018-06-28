/*
 * Copyright 2018 Yeolar
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

#include "accelerator/Benchmark.h"
#include "accelerator/Portability.h"
#include "accelerator/stats/test/MonitorTest.h"

using namespace acc;

// sudo nice -n -20 ./accelerator/stats/test/accelerator_stats_MonitorBenchmark -bm_min_iters 10000000
// ============================================================================
// MonitorBenchmark.cpp                            relative  time/iter  iters/s
// ============================================================================
// addToMonitor_cnt                                             1.29ns  775.95M
// addToMonitor_min                                             1.30ns  769.61M
// addToMonitor_max                                             1.29ns  776.02M
// addToMonitor_avg                                             1.29ns  775.98M
// addToMonitor_sum                                             1.29ns  775.86M
// ============================================================================

BENCHMARK(addToMonitor_cnt, n) {
  for (unsigned i = 0; i < n; ++i) {
    ACCMON_CNT(TestMonitorKey, kTestCnt);
  }
}

BENCHMARK(addToMonitor_min, n) {
  for (unsigned i = 0; i < n; ++i) {
    ACCMON_ADD(TestMonitorKey, kTestMin, -i);
  }
}

BENCHMARK(addToMonitor_max, n) {
  for (unsigned i = 0; i < n; ++i) {
    ACCMON_ADD(TestMonitorKey, kTestMax, i);
  }
}

BENCHMARK(addToMonitor_avg, n) {
  for (unsigned i = 0; i < n; ++i) {
    ACCMON_ADD(TestMonitorKey, kTestAvg, i);
  }
}

BENCHMARK(addToMonitor_sum, n) {
  for (unsigned i = 0; i < n; ++i) {
    ACCMON_ADD(TestMonitorKey, kTestSum, i);
  }
}

int main(int argc, char** argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  acc::runBenchmarks();
  return 0;
}
