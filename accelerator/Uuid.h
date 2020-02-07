/*
 * Copyright (C) 1996, 1997, 1998 Theodore Ts'o.
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

#include <array>
#include <cstdint>
#include <cstring>
#include <string>

/* UUID Variant definitions */
#define UUID_VARIANT_NCS 0
#define UUID_VARIANT_DCE 1
#define UUID_VARIANT_MICROSOFT 2
#define UUID_VARIANT_OTHER 3

/* UUID Type definitions */
#define UUID_TYPE_DCE_TIME 1
#define UUID_TYPE_DCE_RANDOM 4

namespace acc {

class Uuid {
 public:
  typedef std::array<uint8_t, 16> value_type;

  std::string generateTime();

 private:
  int generateTime(uuid_t out);

  value_type uuid_;
};

typedef unsigned char uuid_t[16];

void uuidPack(const struct uuid *uu, uuid_t ptr);
void uuidUnpack(const uuid_t in, struct uuid *uu);

inline void uuidClear(uuid_t uu) {
  memset(uu, 0, 16);
}

int uuidGenerateTime(uuid_t out);

std::string uuidGenerateTime();

inline std::string generateUuid(const std::string& upstreamUuid,
                                const std::string& prefix) {
  return upstreamUuid.empty() ? prefix + ':' + uuidGenerateTime()
                              : upstreamUuid;
}

} // namespace acc
