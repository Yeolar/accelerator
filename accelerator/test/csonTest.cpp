/*
 * Copyright 2020 Yeolar
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

#include <gtest/gtest.h>

#include "accelerator/cson.h"

using namespace acc;

TEST(cson, to) {
  dynamic obj = dynamic::object
    ("key1", "value")
    ("key2", 100)
    ("key3", 1.2345)
    ("key4", true);
  EXPECT_STREQ("{key1=value,key2=100,key3=1.2345,key4=true}",
               toCson(obj).c_str());
  EXPECT_STREQ(
R"({
  key1=value,
  key2=100,
  key3=1.2345,
  key4=true
})", toPrettyCson(obj).c_str());
}

TEST(cson, parse) {
  dynamic expect = dynamic::object
    ("key1", "value")
    ("key2", "100")
    ("key3", "1.2345")
    ("key4", "true");
  EXPECT_EQ(expect, parseCson("{key1=value,key2=100,key3=1.2345,key4=true}"));
  EXPECT_EQ(expect, parseCson(
R"({
  key1=value,
  key2=100,
  key3=1.2345,
  key4=true
})"));
}
