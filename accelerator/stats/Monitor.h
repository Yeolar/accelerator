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
#include <map>
#include <mutex>
#include <string>
#include <thread>

#include "accelerator/Singleton.h"

namespace acc {

class MonitorValue {
 public:
  enum {
    MON_CNT = 1,
    MON_AVG = 2,
    MON_MIN = 4,
    MON_MAX = 8,
    MON_SUM = 16,
  };

  explicit MonitorValue(int type);

  void reset();

  int type() const { return type_; }
  bool isSet() const { return isset_; }

  void add(int value = 0);

  int value() const;

 private:
  int type_;
  bool isset_;
  int count_;
  int64_t value_;
};

class Monitor {
 public:
  typedef std::map<std::string, int> MonMap;

  class Sender {
   public:
    virtual bool send(const MonMap& data) = 0;
  };

  Monitor() {}

  void start();

  void stop() {
    open_ = false;
  }

  void run();

  void setPrefix(const std::string& prefix) {
    prefix_ = prefix;
  }

  void setSender(std::unique_ptr<Sender>&& sender) {
    sender_ = std::move(sender);
  }

  void addToMonitor(const std::string& name, int type, int value = 0);

 private:
  void dump(MonMap& data);

  std::string prefix_;
  std::unique_ptr<Sender> sender_;
  std::map<std::string, MonitorValue> mvalues_;
  std::mutex lock_;
  std::thread handle_;
  std::atomic<bool> open_{false};
};

} // namespace acc

#define ACCMON(name, type, value) \
  ::acc::Singleton< ::acc::Monitor>::get()->addToMonitor( \
    name, ::acc::MonitorValue::MON_##type, value)
#define ACCMON_CNT(name)        ACCMON(name, CNT, 0)
#define ACCMON_AVG(name, value) ACCMON(name, AVG, value)
#define ACCMON_MIN(name, value) ACCMON(name, MIN, value)
#define ACCMON_MAX(name, value) ACCMON(name, MAX, value)
#define ACCMON_SUM(name, value) ACCMON(name, SUM, value)
