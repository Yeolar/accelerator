/*
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

#include "accelerator/Time.h"

namespace acc {

class TimeWatcher {
 public:
  TimeWatcher() : checkpoint_(timestampNow()) {}

  void reset() {
    checkpoint_ = timestampNow();
  }

  uint64_t elapsed() const {
    return acc::elapsed(checkpoint_);
  }

  bool elapsed(uint64_t duration) const {
    return acc::elapsed(checkpoint_) >= duration;
  }

  uint64_t lap() {
    uint64_t interval = elapsed();
    checkpoint_ += interval;
    return interval;
  }

  bool lap(uint64_t duration) {
    uint64_t interval = elapsed();
    if (interval < duration) {
      return false;
    }
    checkpoint_ += interval;
    return true;
  }

  uint64_t getCheckpoint() const {
    return checkpoint_;
  }

 private:
  uint64_t checkpoint_;
};

} // namespace acc
