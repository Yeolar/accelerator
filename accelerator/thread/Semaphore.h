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

#include <semaphore.h>

#include "accelerator/Exception.h"

namespace acc {

class Semaphore {
 public:
  Semaphore(unsigned int value = 0) {
#if defined(__APPLE__)
    sem_t* sem = sem_open("anon", O_CREAT | O_EXCL, 0777, value);
    if (sem == SEM_FAILED) {
      throwSystemError("sem_open");
    }
    sem_ = *sem;
#else
    checkUnixError(sem_init(&sem_, 0, value), "sem_init");
#endif
  }

  ~Semaphore() {
#if defined(__APPLE__)
    checkUnixError(sem_close(&sem_), "sem_close");
#else
    checkUnixError(sem_destroy(&sem_), "sem_destroy");
    checkUnixError(sem_unlink("anon"), "sem_unlink");
#endif
  }

  void post() {
    checkUnixError(sem_post(&sem_), "sem_post");
  }

  void wait() {
    checkUnixError(sem_wait(&sem_), "sem_wait");
  }

  void trywait() {
    checkUnixError(sem_trywait(&sem_), "sem_trywait");
  }

 private:
  sem_t sem_;
};

} // namespace acc
