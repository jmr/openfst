// Copyright 2026 The OpenFst Authors.
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

#include "openfst/compat/unaligned.h"

#include <cstdint>
#include <cstring>

#include "gtest/gtest.h"

namespace fst_internal {
namespace {

TEST(UnalignedTest, UnalignedLoad) {
  constexpr uint32_t kValue = 0x12345678;
  char buf[8] = {0};
  std::memcpy(buf + 1, &kValue, sizeof(kValue));
  EXPECT_EQ(UnalignedLoad<uint32_t>(buf + 1), kValue);
}

}  // namespace
}  // namespace fst_internal
