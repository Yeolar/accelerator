/*
 * Copyright (C) 1996, 1997, 1998 Theodore Ts'o.
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

#include <array>
#include <cstdint>
#include <string>

namespace acc {

class Uuid {
 public:
  typedef std::array<uint8_t, 16> value_type;

  std::string generateTime();

 private:
  int generateTime(value_type& out);

  value_type uuid_;
};

inline std::string generateUuid(const std::string& upstreamUuid,
                                const std::string& prefix) {
  Uuid uuid;
  return upstreamUuid.empty() ? prefix + ':' + uuid.generateTime()
                              : upstreamUuid;
}

} // namespace acc
