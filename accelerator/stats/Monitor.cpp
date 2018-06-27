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

#include "accelerator/stats/Monitor.h"

#include "accelerator/Algorithm.h"
#include "accelerator/Time.h"

namespace acc {

void MonitorValue::init(Type type) {
  type_ = type;
  reset();
}

void MonitorValue::reset() {
  RWSpinLock::WriteHolder w{&lock_};
  isset_ = type_ & (CNT | SUM);
  count_ = 0;
  value_ = 0;
}

void MonitorValue::add(int64_t value) {
  RWSpinLock::WriteHolder w{&lock_};
  isset_ = true;
  count_++;
  switch (type_) {
    case AVG:
    case SUM: value_ += value; break;
    case MIN: value_ = std::min(value_, value); break;
    case MAX: value_ = std::max(value_, value); break;
    default: break;
  }
}

int64_t MonitorValue::value() const {
  RWSpinLock::ReadHolder r{&lock_};
  switch (type_) {
    case CNT: return count_;
    case AVG: return count_ != 0 ? value_ / count_ : 0;
    case MIN:
    case MAX:
    case SUM: return value_;
    default: return 0;
  }
}

} // namespace acc
