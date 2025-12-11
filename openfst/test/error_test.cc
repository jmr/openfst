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
// Unit test for non-fatal error handling.
// Applies when FLAGS_fst_error_fatal = false.

#include <memory>
#include <vector>

#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
#include "gtest/gtest.h"
#include "absl/flags/flag.h"
#include "openfst/lib/arc-map.h"
#include "openfst/lib/arc.h"
#include "openfst/lib/arcsort.h"
#include "openfst/lib/cache.h"
#include "openfst/lib/closure.h"
#include "openfst/lib/compact-fst.h"
#include "openfst/lib/compose.h"
#include "openfst/lib/concat.h"
#include "openfst/lib/const-fst.h"
#include "openfst/lib/determinize.h"
#include "openfst/lib/difference.h"
#include "openfst/lib/epsnormalize.h"
#include "openfst/lib/equivalent.h"
#include "openfst/lib/fst.h"
#include "openfst/lib/intersect.h"
#include "openfst/lib/invert.h"
#include "openfst/lib/matcher.h"
#include "openfst/lib/project.h"
#include "openfst/lib/properties.h"
#include "openfst/lib/randequivalent.h"
#include "openfst/lib/randgen.h"
#include "openfst/lib/rational.h"
#include "openfst/lib/relabel.h"
#include "openfst/lib/reverse.h"
#include "openfst/lib/rmepsilon.h"
#include "openfst/lib/shortest-distance.h"
#include "openfst/lib/shortest-path.h"
#include "openfst/lib/state-map.h"
#include "openfst/lib/symbol-table.h"
#include "openfst/lib/synchronize.h"
#include "openfst/lib/union.h"
#include "openfst/lib/util.h"
#include "openfst/lib/vector-fst.h"
#include "openfst/lib/verify.h"
#include "openfst/lib/weight.h"

namespace fst {
namespace {

using Arc = StdArc;
using StateId = Arc::StateId;
using Weight = Arc::Weight;
using Label = Arc::Label;

// TODO: modernize with EXPECT_* instead of ASSERT_*.
class ErrorTest : public testing::Test {
 protected:
  void SetUp() override {
    goodfst2_.AddState();
    goodfst2_.SetStart(0);
    goodfst2_.SetFinal(0, Weight::One());
    goodfst2_.AddArc(0, Arc(2, 1, Weight::One(), 0));
    goodfst2_.AddArc(0, Arc(2, 2, Weight::One(), 0));
    goodfst2_.AddArc(0, Arc(1, 3, Weight::One(), 0));

    syms1_ = std::make_unique<SymbolTable>("syms1");
    syms1_->AddSymbol("a", 1);
    syms1_->AddSymbol("b", 2);
    goodfst2_.SetInputSymbols(syms1_.get());

    goodfst3_ = goodfst2_;
    Project(&goodfst3_, ProjectType::INPUT);

    syms2_ = std::make_unique<SymbolTable>("syms2");
    syms2_->AddSymbol("c", 1);
    syms2_->AddSymbol("d", 2);
    goodfst3_.SetInputSymbols(syms2_.get());
    goodfst3_.SetOutputSymbols(syms2_.get());

    goodfst4_ = goodfst2_;
    Project(&goodfst4_, ProjectType::OUTPUT);

    badfst1_.SetProperties(kError, kError);

    badfst2_ = goodfst2_;
    badfst2_.SetProperties(kError, kError);

    badfst3_ = goodfst3_;
    badfst3_.SetProperties(kError, kError);

    badfst4_ = goodfst4_;
    badfst4_.SetProperties(kError, kError);

    nanfst1_.AddState();
    nanfst1_.SetStart(0);
    nanfst1_.SetFinal(0, Weight::NoWeight());

    nanfst2_.AddState();
    nanfst2_.SetStart(0);
    nanfst2_.SetFinal(0, Weight::One());
    nanfst2_.AddArc(0, Arc(1, 3, Weight::NoWeight(), 0));
  }

  template <class M>
  void ComposeMatcherTest(const Fst<Arc> &fst1, const Fst<Arc> &fst2,
                          Label special_label) {
    ComposeFstOptions<Arc, M> copts(CacheOptions(),
                                    new M(fst1, MATCH_NONE, kNoLabel),
                                    new M(fst2, MATCH_INPUT, special_label));
    ComposeFst<Arc> cfst(fst1, fst2, copts);
    // Not yet a problem since unexpanded.
    ASSERT_FALSE(cfst.Properties(kError, false));
    VectorFst<Arc> vfst(cfst);
    // Now a problem since expanded.
    ASSERT_TRUE(cfst.Properties(kError, false));
  }

  std::unique_ptr<SymbolTable> syms1_;
  std::unique_ptr<SymbolTable> syms2_;

  // TODO: more informative fst variable names.
  // Empty, no symbols.
  VectorFst<Arc> goodfst1_;

  // Non-empty transducer, ilabel-unsorted, cyclic, non-deterministic, with
  // symbols.
  VectorFst<Arc> goodfst2_;
  // Non-empty acceptor, ilabel-unsorted, cyclic, non-determistic, with
  // symbols.
  VectorFst<Arc> goodfst3_;
  // Non-empty acceptor, ilabel-sorted, cyclic, non-deterministic, with
  // symbols.
  VectorFst<Arc> goodfst4_;
  // Same as goodfst1_ but kError set.
  VectorFst<Arc> badfst1_;
  // Same as goodfst2_ but kError set.
  VectorFst<Arc> badfst2_;
  // Same as goodfst3_ but kError set.
  VectorFst<Arc> badfst3_;
  // Same as goodfst4_ but kError set.
  VectorFst<Arc> badfst4_;
  // Contains NoWeight() final weight.
  VectorFst<Arc> nanfst1_;
  // Contains NoWeight() arc weight.
  VectorFst<Arc> nanfst2_;
};

TEST_F(ErrorTest, VectorFstErrorTest) {
  ASSERT_TRUE(Verify(goodfst1_));
  ASSERT_TRUE(Verify(goodfst2_));
  ASSERT_TRUE(Verify(goodfst3_));
  ASSERT_TRUE(Verify(goodfst4_));
  ASSERT_FALSE(Verify(badfst1_));
  ASSERT_FALSE(Verify(badfst2_));
  ASSERT_FALSE(Verify(badfst3_));
  ASSERT_FALSE(Verify(badfst4_));
  ASSERT_FALSE(Verify(nanfst1_));
  ASSERT_FALSE(Verify(nanfst2_));

  // kError is sticky.
  badfst1_.SetProperties(0, kError);
  ASSERT_TRUE(badfst1_.Properties(kError, false));

  VectorFst<Arc> vfst(badfst1_);
  ASSERT_TRUE(vfst.Properties(kError, false));
}

TEST_F(ErrorTest, ConstFstErrorTest) {
  ConstFst<Arc> cfst(badfst1_);
  ASSERT_TRUE(cfst.Properties(kError, false));
}

TEST_F(ErrorTest, CompactFstErrorTest) {
  CompactAcceptorFst<Arc> c1fst(badfst1_);
  ASSERT_TRUE(c1fst.Properties(kError, false));
  CompactAcceptorFst<Arc> c2fst(badfst2_);
  ASSERT_TRUE(c2fst.Properties(kError, false));
}

TEST_F(ErrorTest, ArcMapErrorTest) {
  VectorFst<Arc> ofst1, ofst2;
  IdentityArcMapper<Arc> mapper;
  ArcMap(badfst1_, &ofst1, mapper);
  ASSERT_TRUE(ofst1.Properties(kError, false));

  ArcMap(badfst2_, &ofst2, mapper);
  ASSERT_TRUE(ofst2.Properties(kError, false));

  ArcMapFst afst(badfst1_, mapper);
  ASSERT_TRUE(afst.Properties(kError, false));
}

TEST_F(ErrorTest, ArcSortErrorTest) {
  ArcSortFst<Arc, ILabelCompare<Arc>> afst(badfst1_, ILabelCompare<Arc>());
  ASSERT_TRUE(afst.Properties(kError, false));
}

TEST_F(ErrorTest, ComposeErrorTest) {
  VectorFst<Arc> ofst1, ofst2, ofst3, ofst4, ofst5;
  Compose(goodfst1_, badfst1_, &ofst1);
  ASSERT_TRUE(ofst1.Properties(kError, false));

  Compose(badfst1_, goodfst1_, &ofst2);
  ASSERT_TRUE(ofst2.Properties(kError, false));

  // Missing symbol table (OK).
  Compose(goodfst1_, goodfst3_, &ofst3);
  ASSERT_FALSE(ofst3.Properties(kError, false));

  // Non-matching symbol tables (not OK).
  Compose(goodfst3_, goodfst2_, &ofst3);
  ASSERT_TRUE(ofst3.Properties(kError, false));

  // Unsorted.
  Compose(goodfst3_, goodfst3_, &ofst4);
  ASSERT_TRUE(ofst4.Properties(kError, false));

  // Sigma matcher that matches on existing label.
  using TestSigmaMatcher = SigmaMatcher<Matcher<Fst<Arc>>>;
  ComposeMatcherTest<TestSigmaMatcher>(goodfst4_, goodfst4_, 1);

  // Rho matcher that matches on existing label.
  using TestRhoMatcher = RhoMatcher<Matcher<Fst<Arc>>>;
  ComposeMatcherTest<TestRhoMatcher>(goodfst4_, goodfst4_, 1);

  // Phi matcher that matches on existing label.
  using TestPhiMatcher = PhiMatcher<Matcher<Fst<Arc>>>;
  ComposeMatcherTest<TestPhiMatcher>(goodfst4_, goodfst4_, 1);
}

TEST_F(ErrorTest, ClosureErrorTest) {
  ClosureFst<Arc> cfst(badfst1_, CLOSURE_STAR);
  ASSERT_TRUE(cfst.Properties(kError, false));
}

TEST_F(ErrorTest, ConcatErrorTest) {
  VectorFst<Arc> ofst1, ofst2(goodfst2_), ofst3(goodfst2_);
  VectorFst<Arc> ofst4, ofst5(goodfst1_);
  Concat(&ofst1, badfst1_);
  ASSERT_TRUE(ofst1.Properties(kError, false));

  Concat(badfst2_, &ofst2);
  ASSERT_TRUE(ofst2.Properties(kError, false));

  Concat(&ofst3, badfst2_);
  ASSERT_TRUE(ofst3.Properties(kError, false));

  Concat(badfst2_, &ofst4);
  ASSERT_TRUE(ofst4.Properties(kError, false));

  ConcatFst<Arc> cfst1(badfst1_, goodfst1_);
  ASSERT_TRUE(cfst1.Properties(kError, false));

  ConcatFst<Arc> cfst2(goodfst1_, badfst1_);
  ASSERT_TRUE(cfst2.Properties(kError, false));

  // TODO: restore when voice actions issues fixed
  // non-matching symbols
  // Concat(&ofst5, goodfst3_);
  // ASSERT_TRUE(ofst3.Properties(kError, false));

  // Missing symbol table (OK).
  ConcatFst<Arc> cfst3(goodfst1_, goodfst3_);
  ASSERT_FALSE(cfst3.Properties(kError, false));

  // Non-matching symbol tables (not OK).
  ConcatFst<Arc> cfst4(goodfst2_, goodfst3_);
  ASSERT_TRUE(cfst4.Properties(kError, false));
}

TEST_F(ErrorTest, DeterminizeErrorTest) {
  VectorFst<Arc> ofst1, ofst2;
  Determinize(badfst1_, &ofst1);
  ASSERT_TRUE(ofst1.Properties(kError, false));

  // Non-functional.
  Determinize(goodfst2_, &ofst2);
  ASSERT_TRUE(ofst2.Properties(kError, false));
}

TEST_F(ErrorTest, DifferenceErrorTest) {
  VectorFst<Arc> ofst1, ofst2, ofst3(goodfst3_), ofst4;
  Difference(goodfst1_, badfst1_, &ofst1);
  ASSERT_TRUE(ofst1.Properties(kError, false));

  Difference(badfst1_, goodfst1_, &ofst2);
  ASSERT_TRUE(ofst2.Properties(kError, false));

  // Non-matching symbols.
  Difference(goodfst1_, goodfst3_, &ofst3);
  ASSERT_TRUE(ofst3.Properties(kError, false));

  // Unsorted and non-deterministic.
  Difference(goodfst3_, goodfst3_, &ofst4);
  ASSERT_TRUE(ofst4.Properties(kError, false));
}

TEST_F(ErrorTest, EpsNormalizeErrorTest) {
  VectorFst<Arc> ofst;
  EpsNormalize(badfst1_, &ofst, EPS_NORM_INPUT);
  ASSERT_TRUE(ofst.Properties(kError, false));
}

TEST_F(ErrorTest, EquivalentErrorTest) {
  bool error;
  ASSERT_FALSE(Equivalent(badfst1_, goodfst1_, kDelta, &error));
  ASSERT_TRUE(error);

  // Non-matching symbols.
  ASSERT_FALSE(Equivalent(goodfst1_, goodfst3_, kDelta, &error));
  ASSERT_TRUE(error);

  // Non-deteterministic.
  ASSERT_FALSE(Equivalent(goodfst3_, goodfst3_, kDelta, &error));
  ASSERT_TRUE(error);
}

TEST_F(ErrorTest, IntersectErrorTest) {
  VectorFst<Arc> ofst1, ofst2, ofst3, ofst4;
  Intersect(goodfst1_, badfst1_, &ofst1);
  ASSERT_TRUE(ofst1.Properties(kError, false));

  Intersect(badfst1_, goodfst1_, &ofst2);
  ASSERT_TRUE(ofst2.Properties(kError, false));

  // Missing symbol table (OK).
  Intersect(goodfst1_, goodfst3_, &ofst3);
  ASSERT_FALSE(ofst3.Properties(kError, false));

  // Non-matching symbol tables (not OK).
  Intersect(goodfst3_, goodfst2_, &ofst3);
  ASSERT_TRUE(ofst3.Properties(kError, false));

  // Unsorted.
  Intersect(goodfst3_, goodfst3_, &ofst4);
  ASSERT_TRUE(ofst4.Properties(kError, false));
}

TEST_F(ErrorTest, InvertErrorTest) {
  InvertFst<Arc> ifst(badfst1_);
  ASSERT_TRUE(ifst.Properties(kError, false));
}

TEST_F(ErrorTest, ProjectErrorTest) {
  ProjectFst<Arc> pfst(badfst1_, ProjectType::INPUT);
  ASSERT_TRUE(pfst.Properties(kError, false));
}

// Prune with non-path weight is a compile-time error.

TEST_F(ErrorTest, RandEquivalentErrorTest) {
  bool error;

  ASSERT_FALSE(RandEquivalent(badfst1_, goodfst1_, 1, kDelta, 1, 1, &error));
  ASSERT_TRUE(error);

  // Missing symbol table (OK).
  ASSERT_TRUE(RandEquivalent(goodfst1_, goodfst3_, 1, kDelta, 1, 1, &error));
  ASSERT_FALSE(error);

  // Non-matching symbol tables (not OK).
  ASSERT_FALSE(RandEquivalent(goodfst2_, goodfst3_, 1, kDelta, 1, 1, &error));
  ASSERT_TRUE(error);
}

TEST_F(ErrorTest, RandGenErrorTest) {
  VectorFst<Arc> ofst;
  RandGen(badfst1_, &ofst);
  ASSERT_TRUE(ofst.Properties(kError, false));
}

TEST_F(ErrorTest, RelabelErrorTest) {
  RelabelFst<Arc> rfst(badfst1_, syms1_.get(), syms1_.get());
  ASSERT_TRUE(rfst.Properties(kError, false));
}

TEST_F(ErrorTest, ReverseErrorTest) {
  VectorFst<Arc> ofst;
  Reverse(badfst1_, &ofst);
  ASSERT_TRUE(ofst.Properties(kError, false));
}

TEST_F(ErrorTest, RmEpsilonErrorTest) {
  // Bad inputs.
  RmEpsilonFst<Arc> rfst1(badfst1_);
  ASSERT_TRUE(rfst1.Properties(kError, false));
  RmEpsilonFst<Arc> rfst2(badfst2_);
  ASSERT_TRUE(rfst2.Properties(kError, false));
  VectorFst<Arc> ofst2(badfst2_);
  RmEpsilon(&ofst2);
  ASSERT_TRUE(ofst2.Properties(kError, false));

  // NaN on an epsilon transition.
  VectorFst<Arc> nanfst(nanfst1_);
  Concat(&nanfst, goodfst2_);
  RmEpsilonFst<Arc> rfst3(nanfst);
  ASSERT_FALSE(rfst3.Properties(kError, false));
  rfst3.NumArcs(rfst3.Start());
  ASSERT_TRUE(rfst3.Properties(kError, false));
  VectorFst<Arc> ofst3(nanfst);
  RmEpsilon(&ofst3);
  ASSERT_TRUE(ofst3.Properties(kError, false));
}

TEST_F(ErrorTest, ShortestDistanceErrorTest) {
  for (const bool reverse : {false, true}) {
    std::vector<Weight> distance;
    ShortestDistance(badfst1_, &distance, reverse);
    ASSERT_TRUE(distance.size() == 1);
    ASSERT_FALSE(distance[0].Member());
    distance.clear();
    ShortestDistance(badfst2_, &distance, reverse);
    ASSERT_TRUE(distance.size() == 1);
    ASSERT_FALSE(distance[0].Member());
    distance.clear();
    ShortestDistance(nanfst1_, &distance, reverse);
    ASSERT_TRUE(distance.size() == 1);
    ASSERT_TRUE(reverse == 1 ? !distance[0].Member() : distance[0].Member());
    distance.clear();
    ShortestDistance(nanfst2_, &distance, reverse);
    ASSERT_TRUE(distance.size() == 1);
    ASSERT_FALSE(distance[0].Member());
  }

  ASSERT_FALSE(ShortestDistance(badfst1_).Member());
  ASSERT_FALSE(ShortestDistance(badfst2_).Member());
  ASSERT_FALSE(ShortestDistance(nanfst1_).Member());
  ASSERT_FALSE(ShortestDistance(nanfst2_).Member());
}

TEST_F(ErrorTest, ShortestPathErrorTest) {
  for (auto n = 1; n < 3; ++n) {
    // Bad inputs.
    VectorFst<Arc> ofst;
    ShortestPath(badfst1_, &ofst, n);
    ASSERT_TRUE(ofst.Properties(kError, false));
    ShortestPath(badfst2_, &ofst, n);
    ASSERT_TRUE(ofst.Properties(kError, false));
    ShortestPath(nanfst1_, &ofst, n);
    ASSERT_TRUE(ofst.Properties(kError, false));
    ShortestPath(nanfst2_, &ofst, n);
    ASSERT_TRUE(ofst.Properties(kError, false));
    // Log semiring is a compilation error, so don't try to test it.
  }
}

TEST_F(ErrorTest, StateMapErrorTest) {
  VectorFst<Arc> ofst1, ofst2;
  IdentityStateMapper<Arc> mapper1(badfst1_);
  IdentityStateMapper<Arc> mapper2(badfst2_);

  StateMap(badfst1_, &ofst1, mapper1);
  ASSERT_TRUE(ofst1.Properties(kError, false));

  StateMap(badfst2_, &ofst2, mapper2);
  ASSERT_TRUE(ofst2.Properties(kError, false));

  StateMapFst<Arc, Arc, IdentityStateMapper<Arc>> afst(badfst1_, mapper1);
  ASSERT_TRUE(afst.Properties(kError, false));
}

TEST_F(ErrorTest, SynchronizeErrorTest) {
  VectorFst<Arc> ofst;

  Synchronize(badfst1_, &ofst);
  ASSERT_TRUE(ofst.Properties(kError, false));

  SynchronizeFst<Arc> sfst(badfst1_);
  ASSERT_TRUE(sfst.Properties(kError, false));
}

TEST_F(ErrorTest, UnionErrorTest) {
  VectorFst<Arc> ofst1, ofst2, ofst3(goodfst1_);
  Union(&ofst1, badfst1_);
  ASSERT_TRUE(ofst1.Properties(kError, false));

  Union(&ofst2, badfst2_);
  ASSERT_TRUE(ofst2.Properties(kError, false));

  UnionFst<Arc> ufst1(badfst1_, goodfst1_);
  ASSERT_TRUE(ufst1.Properties(kError, false));

  UnionFst<Arc> ufst2(goodfst1_, badfst1_);
  ASSERT_TRUE(ufst2.Properties(kError, false));

  // TODO: restore when voice actions issues fixed
  // non-matching symbols
  // Union(&ofst3, goodfst3_);
  // ASSERT_TRUE(ofst2.Properties(kError, false));

  // Missing symbol table (OK).
  UnionFst<Arc> ufst3(goodfst1_, goodfst3_);
  ASSERT_FALSE(ufst3.Properties(kError, false));

  // Non-matching symbol tables (not OK).
  UnionFst<Arc> ufst4(goodfst2_, goodfst3_);
  ASSERT_TRUE(ufst4.Properties(kError, false));
}

}  // namespace
}  // namespace fst

int main(int argc, char **argv) {
  absl::SetFlag(&FLAGS_fst_error_fatal, false);
  absl::SetProgramUsageMessage(argv[0]);
  const auto rest_args = absl::ParseCommandLine(argc, argv);
  return RUN_ALL_TESTS();
}
