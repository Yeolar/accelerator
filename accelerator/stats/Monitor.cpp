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

namespace acc {

namespace detail {

template <typename T>
void updateMax(std::atomic<T>& maxValue, const T& value) {
  T prev = maxValue;
  while (prev < value && !maxValue.compare_exchange_weak(prev, value));
}

template <typename T>
void updateMin(std::atomic<T>& minValue, const T& value) {
  T prev = minValue;
  while (prev > value && !minValue.compare_exchange_weak(prev, value));
}

} // namespace detail

void MonitorValue::init(Type type) {
  type_ = type;
  reset();
}

void MonitorValue::reset() {
  isset_ = type_ & (CNT | SUM);
  count_ = 0;
  value_ = 0;
}

void MonitorValue::add(int64_t value) {
  isset_ = true;
  switch (type_) {
    case CNT:
    case AVG: count_++;
    case SUM: value_ += value; break;
    case MIN: detail::updateMin(value_, value); break;
    case MAX: detail::updateMax(value_, value); break;
    default: break;
  }
}

int64_t MonitorValue::value() const {
  switch (type_) {
    case CNT: return count_;
    case AVG: {
      int32_t n = count_;
      return n != 0 ? value_ / n : 0;
    }
    case MIN:
    case MAX:
    case SUM: return value_;
    default: return 0;
  }
}

void MonitorBase::start() {
  handle_ = std::thread(&MonitorBase::run, this);
  handle_.detach();
}

void MonitorBase::stop() {
  open_ = false;
}

bool MonitorBase::running() {
  return open_;
}

void MonitorBase::run() {
  setCurrentThreadName("MonitorThread");
  open_ = true;
  CycleTimer timer(interval_);
  while (open_) {
    if (timer.isExpired()) {
      Data data;
      dump(data);
      if (sender_) {
        sender_(data);
      }
    }
    sleep(1);
  }
}

void MonitorBase::setPrefix(const std::string& prefix) {
  if (!prefix.empty()) {
    prefix_ = prefix + '.';
  }
}

void MonitorBase::setSender(std::function<void(const Data&)>&& sender) {
  sender_ = std::move(sender);
}

void MonitorBase::setDumpInterval(uint64_t interval) {
  interval_ = interval;
}

} // namespace acc
