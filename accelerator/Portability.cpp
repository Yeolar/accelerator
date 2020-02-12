/*
 * Copyright 2018-present Yeolar
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

#include "accelerator/Portability.h"

#include <errno.h>
#include <unistd.h>

#if !ACC_HAVE_MEMRCHR
extern "C" void* memrchr(const void* s, int c, size_t n) {
  for (auto p = ((const char*)s) + n - 1; p >= (const char*)s; p--) {
    if (*p == (char)c) {
      return (void*)p;
    }
  }
  return nullptr;
}
#endif

template <class F, class... Args>
static int wrapPositional(F f, int fd, off_t offset, Args... args) {
  off_t origLoc = lseek(fd, 0, SEEK_CUR);
  if (origLoc == off_t(-1)) {
    return -1;
  }
  if (lseek(fd, offset, SEEK_SET) == off_t(-1)) {
    return -1;
  }

  int res = (int)f(fd, args...);

  int curErrNo = errno;
  if (lseek(fd, origLoc, SEEK_SET) == off_t(-1)) {
    if (res == -1) {
      errno = curErrNo;
    }
    return -1;
  }
  errno = curErrNo;

  return res;
}

#if !ACC_HAVE_PREADV
extern "C" ssize_t preadv(int fd, const iovec* iov, int count, off_t offset) {
  return wrapPositional(readv, fd, offset, iov, count);
}
#endif

#if !ACC_HAVE_PWRITEV
extern "C" ssize_t pwritev(int fd, const iovec* iov, int count, off_t offset) {
  return wrapPositional(writev, fd, offset, iov, count);
}
#endif
