/*
 * Copyright 2017-present Yeolar
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

#include <gtest/gtest.h>

#include "accelerator/Logging.h"

using namespace acc;
using namespace acc::logging;

TEST(ACCLog, log) {
  ACCLOG(DEBUG) << "debug log default output to stderr";
  ACCLOG(INFO) << "info log default output to stderr";
  ACCLOG(WARN) << "warning log default output to stderr";
  ACCLOG(ERROR) << "error log default output to stderr";
}

TEST(ACCLog, plog) {
  int saved_errno = errno;
  errno = EAGAIN;
  ACCPLOG(WARN) << "EAGAIN warning log";
  errno = saved_errno;
}

TEST(ACCLog, setLevel) {
  Singleton<ACCLogger>::get()->setLevel(logging::LOG_INFO);
  ACCLOG(DEBUG) << "debug log SHOULD NOT BE SEEN";
  ACCLOG(INFO) << "info log available";
  Singleton<ACCLogger>::get()->setLevel(logging::LOG_DEBUG);
  ACCLOG(DEBUG) << "debug log available";
  ACCLOG(INFO) << "info log available";
}

TEST(ACCLog, async) {
  Singleton<ACCLogger>::get()->setLogFile("log.txt");
  ACCLOG(INFO) << "async info log to file";
  Singleton<ACCLogger>::get()->setLogFile("nondir/log.txt");
}

TEST(ACCLog, raw) {
  ACCRLOG(INFO) << "raw log";
}

TEST(ACCLog, cost) {
  ACCCOST_SCOPE(INFO, 1000) {
    usleep(100);
  };
  ACCCOST_SCOPE(INFO, 1000) {
    usleep(10000);
  };
}
