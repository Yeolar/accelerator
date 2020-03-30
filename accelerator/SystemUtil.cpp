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

#include "accelerator/SystemUtil.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef __linux__

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <sched.h>
#include <sys/sysinfo.h>

namespace acc {

bool setCpuAffinity(int cpu, pid_t pid) {
  cpu_set_t mask;
  CPU_ZERO(&mask);
  CPU_SET(cpu, &mask);
  return sched_setaffinity(pid, sizeof(mask), &mask) == 0;
}

int getCpuAffinity(pid_t pid) {
  cpu_set_t mask;
  CPU_ZERO(&mask);
  if (sched_setaffinity(pid, sizeof(mask), &mask) == 0) {
    int n = getCpuNum();
    for (int i = 0; i < n; ++i) {
      if (CPU_ISSET(i, &mask)) {
        return i;
      }
    }
  }
  return -1;
}

SystemMemory getSystemMemory() {
  SystemMemory mem;

  struct sysinfo info;
  sysinfo(&info);

  mem.total = info.totalram;
  mem.free = info.freeram;
  return mem;
}

ProcessMemory getProcessMemory() {
  ProcessMemory mem;

  auto extractNumeric = [](char* p) -> size_t {
    p[strlen(p) - 3] = '\0';
    while (*p < '0' || *p > '9') p++;
    return atol(p) * 1024;
  };

  FILE* file = fopen("/proc/self/status", "r");
  char line[128];
  while (fgets(line, 128, file) != nullptr) {
    if (strncmp(line, "VmRSS:", 6) == 0) {
      mem.rss = extractNumeric(line);
    }
    if (strncmp(line, "VmSize:", 7) == 0) {
      mem.total = extractNumeric(line);
    }
  }
  fclose(file);
  return mem;
}

} // namespace acc

#endif

#include <sys/statvfs.h>

namespace acc {

FsInfo getFsInfo(const char* path) {
  FsInfo info;

  struct statvfs stats;
  statvfs(path, &stats);

  info.freeBlocks = stats.f_bsize * stats.f_bfree;
  info.availableBlocks = stats.f_bsize * stats.f_bavail;
  return info;
}

} // namespace acc
