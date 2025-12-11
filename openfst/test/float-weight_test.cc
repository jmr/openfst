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

#include "openfst/lib/float-weight.h"

#include <cstddef>
#include <limits>

#include "gtest/gtest.h"
#include "openfst/lib/weight.h"

namespace fst {
namespace {

template <class W>
class FloatWeightTest : public testing::Test {
 protected:
  FloatWeightTest() = default;

  void BehavesAlmostLikePOD() {
    EXPECT_TRUE(std::is_standard_layout_v<W>);
    StandardOperationsAreNoexcept();
    StandardOperationsAreTrivial();
  }

  void StandardOperationsAreNoexcept() {
    EXPECT_TRUE(std::is_nothrow_destructible_v<W>);

    EXPECT_TRUE(std::is_nothrow_default_constructible_v<W>);

    EXPECT_TRUE(std::is_nothrow_copy_constructible_v<W>);
    EXPECT_TRUE(std::is_nothrow_copy_assignable_v<W>);

    EXPECT_TRUE(std::is_nothrow_move_constructible_v<W>);
    EXPECT_TRUE(std::is_nothrow_move_assignable_v<W>);
  }

  void StandardOperationsAreTrivial() {
    EXPECT_TRUE(std::is_trivially_destructible_v<W>);

    // not true: std::is_trivially_default_constructible_v<W>

    EXPECT_TRUE(std::is_trivially_copy_constructible_v<W>);
    EXPECT_TRUE(std::is_trivially_copy_assignable_v<W>);

    EXPECT_TRUE(std::is_trivially_move_constructible_v<W>);
    EXPECT_TRUE(std::is_trivially_move_assignable_v<W>);

    EXPECT_TRUE(std::is_trivially_copyable_v<W>);

    // not true: std::is_trivial_v<W>
  }
};

using TestTypes = testing::Types<FloatWeight, TropicalWeight, LogWeight,
                                 Log64Weight, MinMaxWeight>;
TYPED_TEST_SUITE(FloatWeightTest, TestTypes);

TYPED_TEST(FloatWeightTest, TypeTraits) { this->BehavesAlmostLikePOD(); }

// Macro that uses the same syntax as static_assert(condition, message). This
// will allow changing run-time assertions to compile-time assertions easily in
// the future, if compiler support improves. As noted in float-weight.h,
// compiler support for compile-time evaluation of float-valued constant
// expressions is currently buggy and varies considerably by compiler, compiler
// version, ordering of expressions, and expression values.
#define DYNAMIC_ASSERT(condition, message) EXPECT_TRUE(condition) << message

#define STATIC_ASSERT(condition, message) \
  static_assert(condition, message);      \
  EXPECT_TRUE(condition) << message

template <class W>
void TestBasicIdentities() {
  constexpr W no_weight = W::NoWeight();
  constexpr W zero = W::Zero();
  constexpr W one = W::One();

  STATIC_ASSERT(!no_weight.Member(), "NoWeight is not a member");
  STATIC_ASSERT(zero.Member(), "Zero is a member");
  STATIC_ASSERT(one.Member(), "One is a member");

  STATIC_ASSERT(no_weight != no_weight, "NoWeight != NoWeight");
  STATIC_ASSERT(no_weight != zero, "NoWeight != Zero");
  STATIC_ASSERT(no_weight != one, "NoWeight != One");
  STATIC_ASSERT(zero != no_weight, "Zero != NoWeight");
  STATIC_ASSERT(zero == zero, "Zero == Zero");
  STATIC_ASSERT(zero != one, "Zero != One");
  STATIC_ASSERT(one != no_weight, "One != NoWeight");
  STATIC_ASSERT(one != zero, "One != Zero");
  STATIC_ASSERT(one == one, "One == One");
}

template <class W>
void TestLogMultiplicativeOperations() {
  constexpr W no_weight = W::NoWeight();
  constexpr W nan = std::numeric_limits<typename W::ValueType>::signaling_NaN();
  constexpr W neg_inf = -std::numeric_limits<typename W::ValueType>::infinity();
  constexpr W zero = W::Zero();
  constexpr W one = W::One();

  STATIC_ASSERT(!nan.Member(), "nan is not a member");
  STATIC_ASSERT(!neg_inf.Member(), "-inf is not a member");

  // Times involving non-Members.
  for (const auto &non_member : {no_weight, nan, neg_inf}) {
    DYNAMIC_ASSERT(!Times(non_member, no_weight).Member(),
                   "Times(non_member, NoWeight) is not a member");
    DYNAMIC_ASSERT(!Times(non_member, nan).Member(),
                   "Times(non_member, nan) is not a member");
    DYNAMIC_ASSERT(!Times(non_member, neg_inf).Member(),
                   "Times(non_member, -inf) is not a member");
    DYNAMIC_ASSERT(!Times(non_member, zero).Member(),
                   "Times(non_member, Zero) is not a member");
    DYNAMIC_ASSERT(!Times(non_member, one).Member(),
                   "Times(non_member, One) is not a member");

    DYNAMIC_ASSERT(!Times(no_weight, non_member).Member(),
                   "Times(NoWeight, non_member) is not a member");
    DYNAMIC_ASSERT(!Times(nan, non_member).Member(),
                   "Times(nan, non_member) is not a member");
    DYNAMIC_ASSERT(!Times(neg_inf, non_member).Member(),
                   "Times(-inf, non_member) is not a member");
    DYNAMIC_ASSERT(!Times(zero, non_member).Member(),
                   "Times(Zero, non_member) is not a member");
    DYNAMIC_ASSERT(!Times(one, non_member).Member(),
                   "Times(One, non_member) is not a member");
  }

  // Multiplication by Zero.
  DYNAMIC_ASSERT(Times(zero, zero) == zero, "Times(Zero, Zero) == Zero");
  DYNAMIC_ASSERT(Times(zero, one) == zero, "Times(Zero, One) == Zero");
  DYNAMIC_ASSERT(Times(one, zero) == zero, "Times(One, Zero) == Zero");

  STATIC_ASSERT(Times(one, one) == one, "Times(One, One) == One");

  // Divide involving non-Members.
  for (const auto &non_member : {no_weight, nan, neg_inf}) {
    DYNAMIC_ASSERT(!Divide(non_member, no_weight).Member(),
                   "Divide(non_member, NoWeight) is not a member");
    DYNAMIC_ASSERT(!Divide(non_member, nan).Member(),
                   "Divide(non_member, nan) is not a member");
    DYNAMIC_ASSERT(!Divide(non_member, neg_inf).Member(),
                   "Divide(non_member, -inf) is not a member");
    DYNAMIC_ASSERT(!Divide(non_member, zero).Member(),
                   "Divide(non_member, Zero) is not a member");
    DYNAMIC_ASSERT(!Divide(non_member, one).Member(),
                   "Divide(non_member, One) is not a member");

    DYNAMIC_ASSERT(!Divide(no_weight, non_member).Member(),
                   "Divide(NoWeight, non_member) is not a member");
    DYNAMIC_ASSERT(!Divide(nan, non_member).Member(),
                   "Divide(nan, non_member) is not a member");
    DYNAMIC_ASSERT(!Divide(neg_inf, non_member).Member(),
                   "Divide(-inf, non_member) is not a member");
    DYNAMIC_ASSERT(!Divide(zero, non_member).Member(),
                   "Divide(Zero, non_member) is not a member");
    DYNAMIC_ASSERT(!Divide(one, non_member).Member(),
                   "Divide(One, non_member) is not a member");
  }
  STATIC_ASSERT(!Divide(no_weight, no_weight).Member(),
                "Divide(NoWeight, NoWeight) is not a member");
  STATIC_ASSERT(!Divide(nan, no_weight).Member(),
                "Divide(nan, NoWeight) is not a member");
  STATIC_ASSERT(!Divide(neg_inf, no_weight).Member(),
                "Divide(-inf, NoWeight) is not a member");
  STATIC_ASSERT(!Divide(zero, no_weight).Member(),
                "Divide(Zero, NoWeight) is not a member");
  STATIC_ASSERT(!Divide(one, no_weight).Member(),
                "Divide(One, NoWeight) is not a member");

  // Division by Zero.
  DYNAMIC_ASSERT(!Divide(zero, zero).Member(),
                 "Divide(Zero, Zero) is not a member");
  DYNAMIC_ASSERT(!Divide(one, zero).Member(),
                 "Divide(One, Zero) is not a member");

  DYNAMIC_ASSERT(Divide(zero, one) == zero, "Divide(Zero, One) == Zero");
  STATIC_ASSERT(Divide(one, one) == one, "Divide(One, One) == One");

  constexpr float finf = std::numeric_limits<float>::infinity();
  constexpr float fnan = std::numeric_limits<float>::quiet_NaN();
  constexpr std::size_t three = 3;

  // Power involving non-Members.
  for (W non_member : {no_weight, nan, neg_inf}) {
    DYNAMIC_ASSERT(!Power(non_member, fnan).Member(),
                   "Power(non_member, fnan) is not a member");
    DYNAMIC_ASSERT(!Power(non_member, -finf).Member(),
                   "Power(non_member, -finf) is not a member");
    DYNAMIC_ASSERT(!Power(non_member, -2.3).Member(),
                   "Power(non_member, -2.3) is not a member");
    DYNAMIC_ASSERT(!Power(non_member, -1).Member(),
                   "Power(non_member, -1) is not a member");
    DYNAMIC_ASSERT(!Power(non_member, 0).Member(),
                   "Power(non_member, 0) is not a member");
    DYNAMIC_ASSERT(!Power(non_member, 1).Member(),
                   "Power(non_member, 1) is not a member");
    DYNAMIC_ASSERT(!Power(non_member, 2.3).Member(),
                   "Power(non_member, 2.3) is not a member");
    DYNAMIC_ASSERT(!Power(non_member, three).Member(),
                   "Power(non_member, 3) is not a member");
    DYNAMIC_ASSERT(!Power(non_member, finf).Member(),
                   "Power(non_member, finf) is not a member");
  }

  STATIC_ASSERT(!Power(zero, fnan).Member(),
                "Power(Zero, fnan) is not a member");
  DYNAMIC_ASSERT(!Power(zero, -finf).Member(),
                 "Power(Zero, -finf) is not a member");
  DYNAMIC_ASSERT(!Power(zero, -2.3).Member(),
                 "Power(Zero, -2.3) is not a member");
  DYNAMIC_ASSERT(!Power(zero, -1).Member(), "Power(Zero, -1) is not a member");
  STATIC_ASSERT(Power(zero, 0) == one, "Power(Zero, 0) == One");
  DYNAMIC_ASSERT(Power(zero, 1) == zero, "Power(Zero, 1) == Zero");
  DYNAMIC_ASSERT(Power(zero, 2.3) == zero, "Power(Zero, 2.3) == Zero");
  DYNAMIC_ASSERT(Power(zero, three) == zero, "Power(Zero, 3) == Zero");
  DYNAMIC_ASSERT(Power(zero, finf) == zero, "Power(Zero, finf) == Zero");

  STATIC_ASSERT(!Power(one, fnan).Member(), "Power(One, fnan) is not a member");
  STATIC_ASSERT(Power(one, -finf) == one, "Power(One, -finf) == One");
  STATIC_ASSERT(Power(one, -2.3) == one, "Power(One, -2.3) == One");
  STATIC_ASSERT(Power(one, -1) == one, "Power(One, -1) == One");
  STATIC_ASSERT(Power(one, 0) == one, "Power(One, 0) == One");
  STATIC_ASSERT(Power(one, 1) == one, "Power(One, 1) == One");
  STATIC_ASSERT(Power(one, 2.3) == one, "Power(One, 2.3) == One");
  STATIC_ASSERT(Power(one, three) == one, "Power(One, 3) == One");
  STATIC_ASSERT(Power(one, finf) == one, "Power(One, finf) == One");

  constexpr W x = M_LN2;
  STATIC_ASSERT(!Power(x, fnan).Member(), "Power(x, fnan) is not a member");
  DYNAMIC_ASSERT(!Power(x, -finf).Member(), "Power(x, -finf) is not a member");
  STATIC_ASSERT(Power(x, -1) == TropicalWeight(-M_LN2), "Power(x, -1) == 1/x");
  STATIC_ASSERT(Power(x, 0) == one, "Power(x, 0) == One");
  STATIC_ASSERT(Power(x, 1) == x, "Power(x, 1) == x");
  STATIC_ASSERT(Power(x, 2) == TropicalWeight(M_LN2 * 2), "Power(x, 2) == x^2");
  STATIC_ASSERT(Power(x, three) == Power(x, 3.0),
                "Power(x, 3) == Power(x, 3.0)");
  DYNAMIC_ASSERT(Power(x, finf) == zero, "Power(x, finf) == Zero");

  constexpr W y = -M_LN2;
  STATIC_ASSERT(!Power(y, fnan).Member(), "Power(y, fnan) is not a member");
  DYNAMIC_ASSERT(Power(y, -finf) == zero, "Power(y, -finf) == Zero");
  STATIC_ASSERT(Power(y, -1) == TropicalWeight(M_LN2), "Power(y, -1) == 1/y");
  STATIC_ASSERT(Power(y, 0) == one, "Power(y, 0) == One");
  STATIC_ASSERT(Power(y, 1) == y, "Power(y, 1) == y");
  STATIC_ASSERT(Power(y, 2) == TropicalWeight(-M_LN2 * 2),
                "Power(y, 2) == y^2");
  STATIC_ASSERT(Power(y, three) == Power(y, 3.0),
                "Power(y, 3) == Power(y, 3.0)");
  DYNAMIC_ASSERT(!Power(y, finf).Member(), "Power(y, finf) is not a member");
}

TEST(FloatWeightTest, ConstantExpressionsTropical) {
  TestBasicIdentities<TropicalWeight>();

  constexpr TropicalWeight no_weight = TropicalWeight::NoWeight();
  constexpr TropicalWeight zero = TropicalWeight::Zero();
  constexpr TropicalWeight one = TropicalWeight::One();

  // TODO: Discuss how ApproxEqual(Zero(), x) should behave, then fix
  // its implementation to allow all checks to be asserted statically.
  DYNAMIC_ASSERT(!ApproxEqual(no_weight, no_weight), "NoWeight !~= NoWeight");
  DYNAMIC_ASSERT(!ApproxEqual(no_weight, zero), "NoWeight !~= Zero");
  STATIC_ASSERT(!ApproxEqual(no_weight, one), "NoWeight !~= One");
  DYNAMIC_ASSERT(!ApproxEqual(zero, no_weight), "Zero !~= NoWeight");
  DYNAMIC_ASSERT(ApproxEqual(zero, zero), "Zero ~= Zero");
  STATIC_ASSERT(!ApproxEqual(zero, one), "Zero !~= One");
  DYNAMIC_ASSERT(!ApproxEqual(one, no_weight), "One !~= NoWeight");
  DYNAMIC_ASSERT(!ApproxEqual(one, zero), "One !~= Zero");
  STATIC_ASSERT(ApproxEqual(one, one), "One ~= One");

  STATIC_ASSERT(!Plus(no_weight, no_weight).Member(),
                "Plus(NoWeight, NoWeight) is not a member");
  STATIC_ASSERT(!Plus(no_weight, zero).Member(),
                "Plus(NoWeight, Zero) is not a member");
  STATIC_ASSERT(!Plus(zero, no_weight).Member(),
                "Plus(Zero, NoWeight) is not a member");

  STATIC_ASSERT(Plus(zero, zero) == zero, "Plus(Zero, Zero) == Zero");
  STATIC_ASSERT(Plus(zero, one) == one, "Plus(Zero, One) == One");
  STATIC_ASSERT(Plus(one, zero) == one, "Plus(One, Zero) == One");
  STATIC_ASSERT(Plus(one, one) == one, "Plus(One, One) == One");

  TestLogMultiplicativeOperations<TropicalWeight>();
}

TEST(FloatWeightTest, ConstantExpressionsLog) {
  TestBasicIdentities<LogWeight>();
  TestLogMultiplicativeOperations<LogWeight>();
}

TEST(FloatApproxEqualTest, FloatApproxEqual) {
  const float kMax = std::numeric_limits<float>::max();
  const float kMin = -std::numeric_limits<float>::max();
  const float kPosInf = std::numeric_limits<float>::infinity();
  const float kNegInf = -std::numeric_limits<float>::infinity();
  const float kNaN = std::numeric_limits<float>::quiet_NaN();

  EXPECT_TRUE(FloatApproxEqual(0.0f, 0.0f));
  EXPECT_TRUE(FloatApproxEqual(1.0f, 1.0f));
  EXPECT_TRUE(FloatApproxEqual(kMax, kMax));
  EXPECT_TRUE(FloatApproxEqual(kMin, kMin));
  EXPECT_TRUE(FloatApproxEqual(kPosInf, kPosInf));
  EXPECT_TRUE(FloatApproxEqual(kNegInf, kNegInf));
  EXPECT_FALSE(FloatApproxEqual(kNaN, kNaN));

  EXPECT_TRUE(FloatApproxEqual(0.0f, kDelta));
  EXPECT_TRUE(FloatApproxEqual(kDelta, 0.0f));
  EXPECT_FALSE(FloatApproxEqual(0.0f, kDelta * 1.01f));
  EXPECT_FALSE(FloatApproxEqual(kDelta * 1.01f, 0.0f));

  EXPECT_TRUE(FloatApproxEqual(kMax - kDelta, kMax));
  EXPECT_TRUE(FloatApproxEqual(kMax, kMax - kDelta));
  EXPECT_FALSE(FloatApproxEqual(kMax * (1 - kDelta), kMax));
  EXPECT_FALSE(FloatApproxEqual(kMax, kMax * (1 - kDelta)));

  EXPECT_TRUE(FloatApproxEqual(kMin + kDelta, kMin));
  EXPECT_TRUE(FloatApproxEqual(kMin, kMin + kDelta));
  EXPECT_FALSE(FloatApproxEqual(kMin * (1 - kDelta), kMin));
  EXPECT_FALSE(FloatApproxEqual(kMin, kMin * (1 - kDelta)));

  EXPECT_FALSE(FloatApproxEqual(kPosInf, kNegInf));
  EXPECT_FALSE(FloatApproxEqual(kNegInf, kPosInf));
}

}  // namespace
}  // namespace fst
