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

#include <string>
#include <typeinfo>

namespace acc {

/**
 * Return the demangled (prettyfied) version of a C++ type.
 *
 * This function tries to produce a human-readable type, but the type name will
 * be returned unchanged in case of error or if demangling isn't supported on
 * your system.
 *
 * Use for debugging -- do not rely on demangle() returning anything useful.
 *
 * This function may allocate memory (and therefore throw std::bad_alloc).
 */
std::string demangle(const char* name);
inline std::string demangle(const std::type_info& type) {
  return demangle(type.name());
}

} // namespace acc
