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

#include "accelerator/accelerator-config.h"
#include "accelerator/Logging.h"
#include "accelerator/Singleton.h"
#include "accelerator/Time.h"

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
    return type_;
  }
  bool isSet() const {
    return isset_;
  }

  void add(int64_t value = 0);

  int64_t value() const;

 private:
  Type type_;
  std::atomic<bool> isset_;
  std::atomic<int32_t> count_;
  std::atomic<int64_t> value_;
};

class MonitorBase {
 public:
  typedef std::unordered_map<std::string, int64_t> Data;

  MonitorBase() {}
  virtual ~MonitorBase() {}

  void start();
  void stop();

  bool running();

  void setPrefix(const std::string& prefix);
  void setSender(std::function<void(const Data&)>&& sender);
  void setDumpInterval(uint64_t interval);

  virtual void addToMonitor(int key, int64_t value = 0) = 0;

 protected:
  virtual void dump(Data& data) = 0;

  void run();

  std::string prefix_;
  std::function<void(const Data&)> sender_;
  uint64_t interval_{60000000}; // 60s
  std::thread handle_;
  std::atomic<bool> open_{false};
};

template <class T>
class Monitor : public MonitorBase {
 public:
  Monitor() {
    for (int key = 0; key < T::kMax; key++) {
      mvalues_[key].init(T::getType(key));
    }
  }

  void addToMonitor(int key, int64_t value = 0) override {
    if (open_) {
      mvalues_[key].add(value);
    }
  }

 private:
  void dump(Data& data) override {
    int key = 0;
    for (auto& m : mvalues_) {
      if (m.isSet()) {
        data[prefix_ + T::getName(key)] = m.value();
        m.reset();
      }
      key++;
    }
  }

  std::array<MonitorValue, T::kMax> mvalues_;
};

template <class T, class F>
void setupMonitor(const std::string& prefix,
                  F&& sender,
                  uint64_t interval = 60000000) {
#if ACC_MON_ENABLE
  auto mon = Singleton<Monitor<T>>::get();
  mon->setPrefix(prefix);
  mon->setSender(sender);
  mon->setDumpInterval(interval);
  mon->start();
#endif
}

template <class T>
inline void addToMonitor(int key, int64_t value = 0) {
#if ACC_MON_ENABLE
  Singleton<Monitor<T>>::get()->addToMonitor(key, value);
#endif
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

#if ACC_MON_ENABLE
#define ACCMON_ADD(T, key, value) acc::addToMonitor<T>(T::key, value)
#define ACCMON_CNT(T, key)        acc::addToMonitor<T>(T::key)
#else
#define ACCMON_ADD(T, key, value)
#define ACCMON_CNT(T, key)
#endif

