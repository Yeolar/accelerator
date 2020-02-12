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

#include <queue>

#include "accelerator/thread/Synchronized.h"

namespace acc {

template <class T, class Queue = std::queue<T>>
class LockedQueue {
 public:
  typedef typename Queue::value_type value_type;

  LockedQueue() {}

  void push(const value_type& value) {
    queue_.wlock()->push(value);
  }

  void push(value_type&& value) {
    queue_.wlock()->push(std::move(value));
  }

  bool pop(value_type& value) {
    auto ulockedQueue = queue_.ulock();
    if (!ulockedQueue->empty()) {
      auto wlockedQueue = ulockedQueue.moveFromUpgradeToWrite();
      value = std::move(wlockedQueue->front());
      wlockedQueue->pop();
      return true;
    }
    return false;
  }

  size_t size() const {
    return queue_.rlock()->size();
  }

  bool empty() const {
    return queue_.rlock()->empty();
  }

 private:
  Synchronized<Queue> queue_;
};

} // namespace acc
