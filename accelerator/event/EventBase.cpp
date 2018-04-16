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

#include "accelerator/event/EventBase.h"

DEFINE_uint64(event_lp_timeout, 600000000,
              "Long-polling timeout # of event.");

#define ACC_EVENT_STR(state) #state

namespace {
  static const char* stateStrings[] = {
    ACC_EVENT_GEN(ACC_EVENT_STR)
  };
}

namespace acc {

void EventBase::setState(State state) {
  state_ = state;
  timestamps_.push_back(Timestamp(state, timePassed(starttime())));
}

const char* EventBase::stateName() const {
  if (state_ < kInit || state_ >= kUnknown) {
    return stateStrings[kUnknown];
  } else {
    return stateStrings[state_];
  }
}

void EventBase::restart() {
  if (!timestamps_.empty()) {
    timestamps_.clear();
  }
  state_ = kInit;
  timestamps_.push_back(acc::Timestamp(kInit));
}

} // namespace acc
