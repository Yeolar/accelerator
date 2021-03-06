/*
 * Copyright 2017 Facebook, Inc.
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

#include "accelerator/concurrency/ThreadedExecutor.h"

#include <chrono>

#include "accelerator/Logging.h"

namespace acc {

ThreadedExecutor::ThreadedExecutor(std::shared_ptr<ThreadFactory> threadFactory)
    : threadFactory_(std::move(threadFactory)) {
  controlt_ = std::thread([this] { control(); });
}

ThreadedExecutor::~ThreadedExecutor() {
  stopping_.store(true, std::memory_order_release);
  controlw_.notifyOne();
  controlt_.join();
  ACCCHECK(running_.empty());
  ACCCHECK(finished_.empty());
}

void ThreadedExecutor::add(VoidFunc func) {
  ACCCHECK(!stopping_.load(std::memory_order_acquire));
  {
    std::unique_lock<std::mutex> lock(enqueuedm_);
    enqueued_.push_back(std::move(func));
  }
  controlw_.notifyOne();
}

std::shared_ptr<ThreadFactory> ThreadedExecutor::newDefaultThreadFactory() {
  return std::make_shared<ThreadFactory>("Threaded");
}

void ThreadedExecutor::control() {
  setCurrentThreadName("ThreadedCtrl");
  constexpr auto kMaxWait = 10000000; // 10s
  auto looping = true;
  while (looping) {
    controlw_.wait(kMaxWait);
    looping = controlPerformAll();
  }
}

void ThreadedExecutor::work(VoidFunc& func) {
  func();
  auto id = std::this_thread::get_id();
  {
    std::unique_lock<std::mutex> lock(finishedm_);
    finished_.push_back(id);
  }
  controlw_.notifyOne();
}

void ThreadedExecutor::controlJoinFinishedThreads() {
  std::deque<std::thread::id> finishedt;
  {
    std::unique_lock<std::mutex> lock(finishedm_);
    std::swap(finishedt, finished_);
  }
  for (auto id : finishedt) {
    running_[id].join();
    running_.erase(id);
  }
}

void ThreadedExecutor::controlLaunchEnqueuedTasks() {
  std::deque<VoidFunc> enqueuedt;
  {
    std::unique_lock<std::mutex> lock(enqueuedm_);
    std::swap(enqueuedt, enqueued_);
  }
  for (auto& f : enqueuedt) {
    auto th = threadFactory_->newThread([&]() mutable { work(f); });
    auto id = th.get_id();
    running_[id] = std::move(th);
  }
}

bool ThreadedExecutor::controlPerformAll() {
  auto stopping = stopping_.load(std::memory_order_acquire);
  controlJoinFinishedThreads();
  controlLaunchEnqueuedTasks();
  return !stopping || !running_.empty();
}
} // namespace acc
