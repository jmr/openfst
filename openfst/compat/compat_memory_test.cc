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

#include "openfst/compat/compat_memory.h"

#include <type_traits>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/memory/memory.h"
#include "openfst/compat/compat_memory.h"

using ::testing::Eq;
using ::testing::Pointee;

namespace fst {
namespace {

// unique_ptr tests.

TEST(UniquePtrTest, MakeUniqueForOverwriteScalar) {
  auto expected = make_unique_for_overwrite<char>();
  auto actual = fst::make_unique_for_overwrite<char>();
  using ExpectedT = decltype(expected);
  using ActualT = decltype(actual);
  EXPECT_TRUE((std::is_same_v<ExpectedT, ActualT>));
}

TEST(UniquePtrTest, MakeUniqueForOverwriteArray) {
  auto expected = make_unique_for_overwrite<char[]>(10);
  auto actual = fst::make_unique_for_overwrite<char[]>(10);
  using ExpectedT = decltype(expected);
  using ActualT = decltype(actual);
  EXPECT_TRUE((std::is_same_v<ExpectedT, ActualT>));
}

TEST(UniquePtrTest, WrapUnique) {
  using T = std::vector<int>;
  auto expected =
      absl::WrapUnique(new T());
  auto actual = fst::WrapUnique(new T());
  using ExpectedT = decltype(expected);
  using ActualT = decltype(actual);
  ASSERT_TRUE((std::is_same_v<ExpectedT, ActualT>));
  EXPECT_THAT(actual, Pointee(Eq(*expected)));
}

}  // namespace
}  // namespace fst
