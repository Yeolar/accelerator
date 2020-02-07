/*
 * Copyright 2017 Facebook, Inc.
 * Copyright 2017-present Yeolar
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

#include <cassert>
#include <cstddef>
#include <system_error>
#include <unistd.h>
#include <sys/mman.h>

namespace acc {

class MMapAlloc {
 private:
  size_t computeSize(size_t size) {
    long pagesize = sysconf(_SC_PAGESIZE);
    size_t mmapLength = ((size - 1) & ~(pagesize - 1)) + pagesize;
    assert(size <= mmapLength && mmapLength < size + pagesize);
    assert((mmapLength % pagesize) == 0);
    return mmapLength;
  }

 public:
  void* allocate(size_t size) {
    auto len = computeSize(size);

    // MAP_HUGETLB is a perf win, but requires cooperation from the
    // deployment environment (and a change to computeSize()).
    void* mem = static_cast<void*>(mmap(
        nullptr,
        len,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE,
        -1,
        0));
    if (mem == reinterpret_cast<void*>(-1)) {
      throw std::system_error(errno, std::system_category());
    }
    return mem;
  }

  void deallocate(void* p, size_t size) {
    auto len = computeSize(size);
    munmap(p, len);
  }
};

template <typename Allocator>
struct GivesZeroFilledMemory : public std::false_type {};

template <>
struct GivesZeroFilledMemory<MMapAlloc> : public std::true_type {};

} // namespace acc
