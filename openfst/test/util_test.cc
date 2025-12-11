// Copyright 2025 The OpenFst Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// See www.openfst.org for extensive documentation on this weighted
// finite-state transducer library.
//
// Tests STL serialization.

#include "openfst/lib/util.h"

#include <array>
#include <cstdint>
#include <map>
#include <sstream>
#include <vector>

#include "gtest/gtest.h"

namespace fst {
namespace {

template <class T>
void WriteRead(const T &src, T *dst) {
  std::ostringstream out;
  WriteType(out, src);
  std::istringstream in(out.str());
  ReadType(in, dst);
}

TEST(WriteReadTest, Int) {
  int v = 0;
  WriteRead(1, &v);
  EXPECT_EQ(v, 1);
}

enum Enum { kZero, kOne };

TEST(WriteReadTest, Enum) {
  Enum v = kZero;
  WriteRead(kOne, &v);
  EXPECT_EQ(v, kOne);
}

enum class EnumClass { kZero, kOne };

TEST(WriteReadTest, EnumClass) {
  EnumClass v = EnumClass::kZero;
  WriteRead(EnumClass::kOne, &v);
  EXPECT_EQ(v, EnumClass::kOne);
}

TEST(WriteReadTest, MapIntVectorInt) {
  std::map<int, std::vector<int>> a, b;
  for (int i = 0; i < 32; ++i) {
    a[1 << i].push_back(i);
  }
  EXPECT_NE(a, b);
  WriteRead(a, &b);
  EXPECT_EQ(a, b);
}

TEST(WriteReadTest, VectorMapIntInt) {
  std::vector<std::map<int, int>> a, b;
  for (int i = 0; i < 32; ++i) {
    std::map<int, int> m;
    m[1 << i] = i;
    a.push_back(m);
  }
  EXPECT_NE(a, b);
  WriteRead(a, &b);
  EXPECT_EQ(a, b);
}

TEST(WriteReadTest, Array) {
  std::array<int32_t, 3> a{10, 20, 30}, b{0, 0, 0};
  EXPECT_NE(a, b);
  WriteRead(a, &b);
  EXPECT_EQ(a, b);
}
}  // namespace
}  // namespace fst
