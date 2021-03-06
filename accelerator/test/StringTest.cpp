/*
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

#include <gtest/gtest.h>

#include "accelerator/String.h"

using namespace acc;

TEST(split, all) {
  std::string s = "tair_db::tair_test::id";
  std::vector<std::string> v;
  split("::", s, v);
  EXPECT_EQ("tair_db", v[0]);
  EXPECT_EQ("tair_test", v[1]);
  EXPECT_EQ("id", v[2]);
}
