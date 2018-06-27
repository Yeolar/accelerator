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

#pragma once

#include <algorithm>
#include <array>
#include <string>
#include <thread>
#include <unordered_map>

#include "accelerator/Logging.h"
#include "accelerator/Singleton.h"
#include "accelerator/thread/RWSpinLock.h"
#include "accelerator/thread/ThreadUtil.h"

namespace acc {

class MonitorValue {
 public:
  enum Type : char {
    NON = 0,
    CNT = 1,
    AVG = 1 << 1,
    MIN = 1 << 2,
    MAX = 1 << 3,
    SUM = 1 << 4,
  };

  MonitorValue() : type_(NON) {}

  void init(Type type);

  void reset();

  Type type() const {
    RWSpinLock::ReadHolder r{&lock_};
    return type_;
  }
  bool isSet() const {
    RWSpinLock::ReadHolder r{&lock_};
    return isset_;
  }

  void add(int64_t value = 0);

  int64_t value() const;

 private:
  Type type_;
  bool isset_;
  int32_t count_;
  int64_t value_;
  mutable RWSpinLock lock_;
};

template <class T>
class Monitor {
 public:
  typedef std::unordered_map<std::string, int> MonMap;

  Monitor() {
    for (int key = 0; key < T::kMax; key++) {
      mvalues_[key].init(T::getType(key));
    }
  }

  void start() {
    handle_ = std::thread(&Monitor::run, this);
    handle_.detach();
  }

  void stop() {
    open_ = false;
  }

  void run() {
    setCurrentThreadName("MonitorThread");
    open_ = true;
    CycleTimer timer(60000000); // 60s
    while (open_) {
      if (timer.isExpired()) {
        MonMap data;
        dump(data);
        if (sender_) {
          sender_(data);
        }
      }
      sleep(1);
    }
  }

  void setPrefix(const std::string& prefix) {
    prefix_ = prefix + '.';
  }

  void setSender(std::function<void(const MonMap&)>&& sender) {
    sender_ = std::move(sender);
  }

  void addToMonitor(int key, int64_t value = 0) {
    if (open_) {
      mvalues_[key].add(value);
    }
  }

 private:
  void dump(MonMap& data) {
    int key = 0;
    for (auto& m : mvalues_) {
      if (m.isSet()) {
        data[prefix_ + T::getName(key)] = m.value();
        m.reset();
      }
      key++;
    }
  }

  std::string prefix_;
  std::function<void(const MonMap&)> sender_;
  std::array<MonitorValue, T::kMax> mvalues_;
  std::thread handle_;
  std::atomic<bool> open_{false};
};

template <class T, class F>
void setupMonitor(const std::string& prefix, F&& sender) {
  auto mon = Singleton<Monitor<T>>::get();
  mon->setPrefix(prefix);
  mon->setSender(sender);
  mon->start();
}

} // namespace acc

#define ACCMON_KEY_CSTR(type, key) #key
#define ACCMON_KEY_ENUM(type, key) k##key
#define ACCMON_KEY_TYPE(type, key) ::acc::MonitorValue::type

#define ACCMON_KEY(cls, gen)                            \
struct cls {                                            \
  enum Key {                                            \
    gen(ACCMON_KEY_ENUM)                                \
  };                                                    \
                                                        \
  static ::acc::MonitorValue::Type getType(int key) {   \
    ::acc::MonitorValue::Type types[] = {               \
      gen(ACCMON_KEY_TYPE)                              \
    };                                                  \
    return types[key];                                  \
  }                                                     \
  static const char* getName(int key) {                 \
    const char* names[] = {                             \
      gen(ACCMON_KEY_CSTR)                              \
    };                                                  \
    return names[key];                                  \
  }                                                     \
}; //

#define ACCMON(T, key, value) \
  ::acc::Singleton< ::acc::Monitor<T>>::get()->addToMonitor(T::key, value)
#define ACCMON_CNT(T, key)        ACCMON(T, key, 0)
#define ACCMON_VAL(T, key, value) ACCMON(T, key, value)

