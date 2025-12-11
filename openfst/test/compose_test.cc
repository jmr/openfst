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
// Unit test for Compose.

#include "openfst/test/compose_test.h"

#include "gtest/gtest.h"
#include "openfst/lib/arc.h"
#include "openfst/lib/const-fst.h"
#include "openfst/lib/vector-fst.h"

namespace fst {
namespace {

using ComposeTypes = ::testing::Types<VectorFst<StdArc>, ConstFst<StdArc>>;
INSTANTIATE_TYPED_TEST_SUITE_P(Compose, ComposeTest, ComposeTypes);

}  // namespace
}  // namespace fst
