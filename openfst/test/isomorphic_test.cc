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
// Unit test for Isomorphic.

#include "openfst/lib/isomorphic.h"

#include <memory>
#include <string>

#include "gtest/gtest.h"
#include "openfst/lib/arc.h"
#include "openfst/lib/vector-fst.h"

namespace fst {
namespace {

using Arc = StdArc;

class IsomorphicTest : public testing::Test {
 protected:
  void SetUp() override {
    const std::string path =
        std::string(".") + "/openfst/test/testdata/isomorphic/";
    const std::string iso1_name = path + "i1.fst";
    const std::string iso2_name = path + "i2.fst";
    const std::string iso3_name = path + "i3.fst";
    const std::string iso4_name = path + "i4.fst";
    const std::string iso5_name = path + "i5.fst";
    const std::string iso6_name = path + "i6.fst";

    ifst1_.reset(VectorFst<Arc>::Read(iso1_name));
    ifst2_.reset(VectorFst<Arc>::Read(iso2_name));
    ifst3_.reset(VectorFst<Arc>::Read(iso3_name));
    ifst4_.reset(VectorFst<Arc>::Read(iso4_name));
    ifst5_.reset(VectorFst<Arc>::Read(iso5_name));
    ifst6_.reset(VectorFst<Arc>::Read(iso6_name));
  }

  std::unique_ptr<VectorFst<Arc>> ifst1_;
  std::unique_ptr<VectorFst<Arc>> ifst2_;
  std::unique_ptr<VectorFst<Arc>> ifst3_;
  std::unique_ptr<VectorFst<Arc>> ifst4_;
  std::unique_ptr<VectorFst<Arc>> ifst5_;
  std::unique_ptr<VectorFst<Arc>> ifst6_;
};

TEST_F(IsomorphicTest, Isomorphic) {
  ASSERT_TRUE(Isomorphic(*ifst1_, *ifst1_));
  ASSERT_TRUE(Isomorphic(*ifst1_, *ifst2_));
  ASSERT_FALSE(Isomorphic(*ifst1_, *ifst3_));
  ASSERT_FALSE(Isomorphic(*ifst1_, *ifst4_));
}

TEST_F(IsomorphicTest, NondetIsomorphic) {
  // Checks that nondet Fsts equal modulo arc ordering are isomorphic.
  ASSERT_TRUE(Isomorphic(*ifst5_, *ifst6_));
}

}  // namespace
}  // namespace fst
