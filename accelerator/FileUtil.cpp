/*
 * Copyright 2017 Facebook, Inc.
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

#include "accelerator/FileUtil.h"

#include <cerrno>
#include <sys/file.h>
#include <sys/socket.h>

#include "accelerator/Portability.h"

namespace acc {

bool setCloseExec(int fd) {
  int r = fcntl(fd, F_GETFD, 0);
  return r != -1 && fcntl(fd, F_SETFD, r | FD_CLOEXEC) != -1;
}

bool setNonBlocking(int fd) {
  int r = fcntl(fd, F_GETFL, 0);
  return r != -1 && fcntl(fd, F_SETFL, r | O_NONBLOCK) != -1;
}

/**
 * Helper functions and templates for FileUtil.cpp.  Declared here so
 * they can be unittested.
 */
namespace fileutil_detail {

// Wrap call to f(args) in loop to retry on EINTR
template <class F, class... Args>
ssize_t wrapNoInt(F f, Args... args) {
  ssize_t r;
  do {
    r = f(args...);
  } while (r == -1 && errno == EINTR);
  return r;
}

inline void incr(ssize_t /* n */) {}
inline void incr(ssize_t n, off_t& offset) { offset += off_t(n); }

// Wrap call to read/pread/write/pwrite(fd, buf, count, offset?) to retry on
// incomplete reads / writes.  The variadic argument magic is there to support
// an additional argument (offset) for pread / pwrite; see the incr() functions
// above which do nothing if the offset is not present and increment it if it
// is.
template <class F, class... Offset>
ssize_t wrapFull(F f, int fd, void* buf, size_t count, Offset... offset) {
  char* b = static_cast<char*>(buf);
  ssize_t totalBytes = 0;
  ssize_t r;
  do {
    r = f(fd, b, count, offset...);
    if (r == -1) {
      if (errno == EINTR) {
        continue;
      }
      return r;
    }

    totalBytes += r;
    b += r;
    count -= r;
    incr(r, offset...);
  } while (r != 0 && count);  // 0 means EOF

  return totalBytes;
}

// Wrap call to readv/preadv/writev/pwritev(fd, iov, count, offset?) to
// retry on incomplete reads / writes.
template <class F, class... Offset>
ssize_t wrapvFull(F f, int fd, iovec* iov, int count, Offset... offset) {
  ssize_t totalBytes = 0;
  ssize_t r;
  do {
    r = f(fd, iov, std::min<int>(count, IOV_MAX), offset...);
    if (r == -1) {
      if (errno == EINTR) {
        continue;
      }
      return r;
    }

    if (r == 0) {
      break;  // EOF
    }

    totalBytes += r;
    incr(r, offset...);
    while (r != 0 && count != 0) {
      if (r >= ssize_t(iov->iov_len)) {
        r -= ssize_t(iov->iov_len);
        ++iov;
        --count;
      } else {
        iov->iov_base = static_cast<char*>(iov->iov_base) + r;
        iov->iov_len -= r;
        r = 0;
      }
    }
  } while (count);

  return totalBytes;
}

} // namespace fileutil_detail

using namespace fileutil_detail;

int openNoInt(const char* name, int flags, mode_t mode) {
  return int(wrapNoInt(open, name, flags, mode));
}

int openatNoInt(int dirfd, const char* name, int flags, mode_t mode) {
  return int(wrapNoInt(openat, dirfd, name, flags, mode));
}

int closeNoInt(int fd) {
  int r = close(fd);
  // Ignore EINTR.  On Linux, close() may only return EINTR after the file
  // descriptor has been closed, so you must not retry close() on EINTR --
  // in the best case, you'll get EBADF, and in the worst case, you'll end up
  // closing a different file (one opened from another thread).
  //
  // Interestingly enough, the Single Unix Specification says that the state
  // of the file descriptor is unspecified if close returns EINTR.  In that
  // case, the safe thing to do is also not to retry close() -- leaking a file
  // descriptor is definitely better than closing the wrong file.
  if (r == -1 && errno == EINTR) {
    r = 0;
  }
  return r;
}

int fsyncNoInt(int fd) {
  return int(wrapNoInt(fsync, fd));
}

int dupNoInt(int fd) {
  return int(wrapNoInt(dup, fd));
}

int dup2NoInt(int oldfd, int newfd) {
  return int(wrapNoInt(dup2, oldfd, newfd));
}

int fdatasyncNoInt(int fd) {
#if defined(__APPLE__)
  return int(wrapNoInt(fcntl, fd, F_FULLFSYNC));
#else
  return int(wrapNoInt(fdatasync, fd));
#endif
}

int ftruncateNoInt(int fd, off_t len) {
  return int(wrapNoInt(ftruncate, fd, len));
}

int truncateNoInt(const char* path, off_t len) {
  return int(wrapNoInt(truncate, path, len));
}

int flockNoInt(int fd, int operation) {
  return int(wrapNoInt(flock, fd, operation));
}

int shutdownNoInt(int fd, int how) {
  return int(wrapNoInt(shutdown, fd, how));
}

ssize_t readNoInt(int fd, void* buf, size_t count) {
  return wrapNoInt(read, fd, buf, count);
}

ssize_t preadNoInt(int fd, void* buf, size_t count, off_t offset) {
  return wrapNoInt(pread, fd, buf, count, offset);
}

ssize_t readvNoInt(int fd, const iovec* iov, int count) {
  return wrapNoInt(readv, fd, iov, count);
}

ssize_t writeNoInt(int fd, const void* buf, size_t count) {
  return wrapNoInt(write, fd, buf, count);
}

ssize_t pwriteNoInt(int fd, const void* buf, size_t count, off_t offset) {
  return wrapNoInt(pwrite, fd, buf, count, offset);
}

ssize_t writevNoInt(int fd, const iovec* iov, int count) {
  return wrapNoInt(writev, fd, iov, count);
}

ssize_t readFull(int fd, void* buf, size_t n) {
  return wrapFull(read, fd, buf, n);
}

ssize_t preadFull(int fd, void* buf, size_t count, off_t offset) {
  return wrapFull(pread, fd, buf, count, offset);
}

ssize_t writeFull(int fd, const void* buf, size_t n) {
  return wrapFull(write, fd, const_cast<void*>(buf), n);
}

ssize_t pwriteFull(int fd, const void* buf, size_t count, off_t offset) {
  return wrapFull(pwrite, fd, const_cast<void*>(buf), count, offset);
}

ssize_t readvFull(int fd, iovec* iov, int count) {
  return wrapvFull(readv, fd, iov, count);
}

ssize_t preadvFull(int fd, iovec* iov, int count, off_t offset) {
  return wrapvFull(preadv, fd, iov, count, offset);
}

ssize_t writevFull(int fd, iovec* iov, int count) {
  return wrapvFull(writev, fd, iov, count);
}

ssize_t pwritevFull(int fd, iovec* iov, int count, off_t offset) {
  return wrapvFull(pwritev, fd, iov, count, offset);
}

} // namespace acc
