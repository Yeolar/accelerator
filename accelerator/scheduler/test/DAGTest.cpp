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

#include <string>
#include <gtest/gtest.h>

#include "accelerator/String.h"
#include "accelerator/scheduler/DAG.h"

using namespace acc;

class InlineExecutor : public Executor {
 public:
  InlineExecutor() {}

  void add(VoidFunc func) override {
    func();
  }
};

void run(int key, std::string* out) {
  stringAppendf(out, "%d ", key);
}

TEST(DAG, all) {
  DAG dag(std::shared_ptr<Executor>(new InlineExecutor()));
  std::string out;

  dag.add(std::bind(run, 0, &out));
  dag.add(std::bind(run, 1, &out));
  dag.add(std::bind(run, 2, &out));
  dag.add(std::bind(run, 3, &out));
  dag.add(std::bind(run, 4, &out));
  /*       2 -+
   * 0 -+      \
   *     \      \
   * 1 --- 3 ---- 4
   */
  dag.dependency(0, 3);
  dag.dependency(1, 3);
  dag.dependency(2, 4);
  dag.dependency(3, 4);

  dag.go(std::bind(run, 5, &out));
  EXPECT_STREQ("0 1 3 2 4 5 ", out.c_str());
}
