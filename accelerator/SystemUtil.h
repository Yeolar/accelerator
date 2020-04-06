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

#include <string>
#include <unistd.h>

namespace acc {

inline int getCpuNum() {
  return sysconf(_SC_NPROCESSORS_CONF);
}

std::string getProcessName();

#ifdef __linux__

bool setCpuAffinity(int cpu, pid_t pid = 0);
int getCpuAffinity(pid_t pid = 0);

struct SystemMemory {
  size_t total;
  size_t free;
};

SystemMemory getSystemMemory();

struct ProcessMemory {
  size_t total;
  size_t rss;
};

ProcessMemory getProcessMemory();

#endif

struct FsInfo {
  size_t freeBlocks;
  size_t availableBlocks;
};

FsInfo getFsInfo(const char* path);

} // namespace acc
