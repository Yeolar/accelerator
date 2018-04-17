/*
 * Copyright 2018 Yeolar
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

#include <vector>
#include <gflags/gflags.h>

#include "accelerator/String.h"
#include "accelerator/Time.h"
#include "accelerator/event/EventUtil.h"

DECLARE_uint64(event_lp_timeout);

namespace acc {

#define ACC_EVENT_GEN(x) \
  x(Init),               \
  x(Connect),            \
  x(Listen),             \
  x(ToRead),             \
  x(Reading),            \
  x(Readed),             \
  x(ToWrite),            \
  x(Writing),            \
  x(Writed),             \
  x(Next),               \
  x(Fail),               \
  x(Timeout),            \
  x(Error),              \
  x(Unknown)

#define ACC_EVENT_ENUM(state) k##state

class EventBase {
 public:
  enum State {
    ACC_EVENT_GEN(ACC_EVENT_ENUM)
  };

  EventBase(const TimeoutOption& timeoutOpt)
    : timeoutOpt_(timeoutOpt) {}

  virtual ~EventBase() {}

  /*
   * Event state
   */

  State state() const {
    return state_;
  }

  void setState(State state);

  const char* stateName() const;

  /*
   * Event timestamp and restart
   */

  void restart();

  uint64_t starttime() const {
    return timestamps_.front().stamp;
  }
  uint64_t cost() const {
    return timePassed(starttime());
  }
  std::string timestampStr() const {
    return join("-", timestamps_);
  }

  /*
   * Event timeout
   */

  const TimeoutOption& timeoutOption() const {
    return timeoutOpt_;
  }
  Timeout<EventBase> edeadline() {
    return Timeout<EventBase>(this, starttime() + FLAGS_event_lp_timeout, true);
  }
  Timeout<EventBase> cdeadline() {
    return Timeout<EventBase>(this, starttime() + timeoutOpt_.ctimeout);
  }
  Timeout<EventBase> rdeadline() {
    return Timeout<EventBase>(this, starttime() + timeoutOpt_.rtimeout);
  }
  Timeout<EventBase> wdeadline() {
    return Timeout<EventBase>(this, starttime() + timeoutOpt_.wtimeout);
  }

  bool isConnectTimeout() const {
    return cost() > timeoutOpt_.ctimeout;
  }
  bool isReadTimeout() const {
    return cost() > timeoutOpt_.rtimeout;
  }
  bool isWriteTimeout() const {
    return cost() > timeoutOpt_.wtimeout;
  }

 public:
  virtual int fd() const = 0;
  virtual std::string str() const = 0;

 protected:
  State state_;
  std::vector<Timestamp> timestamps_;
  TimeoutOption timeoutOpt_;
};

inline std::ostream& operator<<(std::ostream& os, const EventBase& event) {
  os << event.str();
  return os;
}

#undef ACC_EVENT_ENUM

} // namespace acc
