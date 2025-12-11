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

#include "openfst/lib/power-weight-mappers.h"

#include <cstddef>
#include <cstdint>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "openfst/lib/float-weight.h"
#include "openfst/lib/fst-decl.h"
#include "openfst/lib/power-weight.h"
#include "openfst/lib/sparse-power-weight.h"
#include "openfst/test/power-weight-util.h"

namespace {

using ::fst::PowerWeight;
using ::fst::SparsePowerWeight;
using ::fst::TropicalWeight;
using ::fst::test::CreateWeight;
using ::fst::test::ToVector;
using ::testing::ElementsAre;
using ::testing::FloatEq;
using ::testing::Pair;

// Project test uses index 7, so make PowerWeight size 8.
using PowerWeightTypes =
    testing::Types<SparsePowerWeight<TropicalWeight, int32_t>,
                   PowerWeight<TropicalWeight, 8>>;

template <typename PowerWeightT>
class ToPowerWeightMapperTest : public testing::Test {
 public:
  using Mapper = fst::ToPowerWeightMapper<TropicalWeight, PowerWeightT>;
};
TYPED_TEST_SUITE(ToPowerWeightMapperTest, PowerWeightTypes);

TYPED_TEST(ToPowerWeightMapperTest, TropicalWeight) {
  const TropicalWeight w(1.23);
  EXPECT_THAT(ToVector(typename TestFixture::Mapper()(w)),
              ElementsAre(Pair(0, w.Value())));
}

TYPED_TEST(ToPowerWeightMapperTest, TropicalWeightWithIndex) {
  const TropicalWeight w(1.23);
  EXPECT_THAT(ToVector(typename TestFixture::Mapper(5)(w)),
              ElementsAre(Pair(5, w.Value())));
}

template <typename PowerWeightT>
class FromPowerWeightMapperTest : public testing::Test {
 public:
  using FromWeight = PowerWeightT;
  using Mapper = fst::FromPowerWeightMapper<PowerWeightT, TropicalWeight>;
};
TYPED_TEST_SUITE(FromPowerWeightMapperTest, PowerWeightTypes);

TYPED_TEST(FromPowerWeightMapperTest, TropicalWeight) {
  using FromWeight = typename TestFixture::FromWeight;
  const FromWeight w = CreateWeight<FromWeight>({{0, 1.23}, {2, 2.34}});
  EXPECT_EQ(1.23, typename TestFixture::Mapper()(w));
}

TYPED_TEST(FromPowerWeightMapperTest, TropicalWeightWithIndex) {
  using FromWeight = typename TestFixture::FromWeight;
  const FromWeight w = CreateWeight<FromWeight>({{0, 1.23}, {2, 2.34}});
  EXPECT_EQ(2.34, typename TestFixture::Mapper(2)(w));
}

TYPED_TEST(FromPowerWeightMapperTest, TropicalWeightDefault) {
  using FromWeight = typename TestFixture::FromWeight;
  const FromWeight w = CreateWeight<FromWeight>({{0, 1.23}, {2, 2.34}});
  EXPECT_EQ(TropicalWeight::Zero(), typename TestFixture::Mapper(1)(w));
}

template <typename PowerWeightT>
class ProjectPowerWeightMapperTest : public testing::Test {
 public:
  using Weight = PowerWeightT;
  using Mapper = fst::ProjectPowerWeightMapper<PowerWeightT>;
};
TYPED_TEST_SUITE(ProjectPowerWeightMapperTest, PowerWeightTypes);

TYPED_TEST(ProjectPowerWeightMapperTest, Default) {
  using Weight = typename TestFixture::Weight;
  const Weight w = CreateWeight<Weight>({{0, 1.23}, {2, 2.34}});
  EXPECT_THAT(ToVector(typename TestFixture::Mapper()(w)),
              ElementsAre(Pair(0, 1.23)));
}

TYPED_TEST(ProjectPowerWeightMapperTest, OneIndex) {
  using Weight = typename TestFixture::Weight;
  const Weight w = CreateWeight<Weight>({{0, 1.23}, {2, 2.34}});
  EXPECT_THAT(ToVector(typename TestFixture::Mapper(2)(w)),
              ElementsAre(Pair(0, 2.34)));
}

TYPED_TEST(ProjectPowerWeightMapperTest, TwoIndex) {
  using Weight = typename TestFixture::Weight;
  const Weight w = CreateWeight<Weight>({{0, 1.23}, {2, 2.34}});
  EXPECT_THAT(ToVector(typename TestFixture::Mapper(2, 7)(w)),
              ElementsAre(Pair(7, 2.34)));
}

template <typename PowerWeightT>
class TransformPowerWeightMapperTest : public testing::Test {
 public:
  using Weight = PowerWeightT;
};
TYPED_TEST_SUITE(TransformPowerWeightMapperTest, PowerWeightTypes);

template <typename W, typename K>
SparsePowerWeight<W, K> Transform(const SparsePowerWeight<W, K> &w) {
  SparsePowerWeight<W, K> result;
  using Iterator = typename SparsePowerWeight<W, K>::Iterator;
  for (Iterator iter(w); !iter.Done(); iter.Next()) {
    result.SetValue(iter.Value().first + 1, iter.Value().second.Value() * 2);
  }
  return result;
}

template <typename W, size_t n>
PowerWeight<W, n> Transform(const PowerWeight<W, n> &w) {
  PowerWeight<W, n> result;
  for (size_t i = 0; i + 1 < n; ++i) {
    result.SetValue(i + 1, w.Value(i).Value() * 2);
  }
  return result;
}

TYPED_TEST(TransformPowerWeightMapperTest, Transform) {
  using Weight = typename TestFixture::Weight;
  const Weight w = CreateWeight<Weight>({{0, 1.23}, {2, 2.34}});
  auto transform = [](const Weight &a) { return Transform(a); };
  EXPECT_THAT(
      ToVector(fst::TransformPowerWeightMapper<Weight, decltype(transform)>(
          transform)(w)),
      ElementsAre(Pair(1, FloatEq(2 * 1.23)), Pair(3, FloatEq(2 * 2.34))));
}

}  // namespace
