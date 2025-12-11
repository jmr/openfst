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
// Unit test for Replcae.

#include "openfst/lib/replace.h"

#include <cstddef>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "gtest/gtest.h"
#include "openfst/lib/arc.h"
#include "openfst/lib/arcsort.h"
#include "openfst/lib/compose.h"
#include "openfst/lib/connect.h"
#include "openfst/lib/equal.h"
#include "openfst/lib/equivalent.h"
#include "openfst/lib/fst.h"
#include "openfst/lib/matcher.h"
#include "openfst/lib/mutable-fst.h"
#include "openfst/lib/properties.h"
#include "openfst/lib/replace-util.h"
#include "openfst/lib/reverse.h"
#include "openfst/lib/rmepsilon.h"
#include "openfst/lib/vector-fst.h"
#include "openfst/lib/verify.h"

namespace fst {
namespace {

using Arc = StdArc;
using Label = Arc::Label;
using StateId = Arc::StateId;

class ReplaceTest : public testing::Test {
 protected:
  using FstTuple = std::pair<Label, const Fst<Arc> *>;
  using MutableFstTuple = std::pair<Label, MutableFst<Arc> *>;

  void SetUp() override {
    const std::string path =
        std::string(".") + "/openfst/test/testdata/replace/";
    const std::string g1_name = path + "g1.fst";
    const std::string g2_name = path + "g2.fst";
    const std::string g3_name = path + "g3.fst";
    const std::string g4_name = path + "g4.fst";
    const std::string gout_name = path + "g_out.fst";
    const std::string lattice_name = path + "lattice.fst";
    const std::string path_name = path + "path.fst";
    const std::string r1_name = path + "r1.fst";
    const std::string r2_name = path + "r2.fst";
    const std::string r3_name = path + "r3.fst";
    const std::string r4_name = path + "r4.fst";
    const std::string r5_name = path + "r5.fst";
    const std::string i1_name = path + "i1.fst";
    const std::string i2_name = path + "i2.fst";
    const std::string i3_name = path + "i3.fst";
    const std::string s1_name = path + "s1.fst";
    const std::string s2_name = path + "s2.fst";
    const std::string s3_name = path + "s3.fst";

    g1_.reset(VectorFst<Arc>::Read(g1_name));
    g2_.reset(VectorFst<Arc>::Read(g2_name));
    g3_.reset(VectorFst<Arc>::Read(g3_name));
    g4_.reset(VectorFst<Arc>::Read(g4_name));
    gout_.reset(VectorFst<Arc>::Read(gout_name));
    lattice_.reset(VectorFst<Arc>::Read(lattice_name));
    path_.reset(VectorFst<Arc>::Read(path_name));
    r1_.reset(VectorFst<Arc>::Read(r1_name));
    r2_.reset(VectorFst<Arc>::Read(r2_name));
    r3_.reset(VectorFst<Arc>::Read(r3_name));
    r4_.reset(VectorFst<Arc>::Read(r4_name));
    r5_.reset(VectorFst<Arc>::Read(r5_name));
    i1_.reset(VectorFst<Arc>::Read(i1_name));
    i2_.reset(VectorFst<Arc>::Read(i2_name));
    i3_.reset(VectorFst<Arc>::Read(i3_name));
    s1_.reset(VectorFst<Arc>::Read(s1_name));
    s2_.reset(VectorFst<Arc>::Read(s2_name));
    s3_.reset(VectorFst<Arc>::Read(s3_name));
  }

  std::unique_ptr<VectorFst<Arc>> g1_;
  std::unique_ptr<VectorFst<Arc>> g2_;
  std::unique_ptr<VectorFst<Arc>> g3_;
  std::unique_ptr<VectorFst<Arc>> g4_;
  std::unique_ptr<VectorFst<Arc>> gout_;
  std::unique_ptr<VectorFst<Arc>> lattice_;
  std::unique_ptr<VectorFst<Arc>> path_;
  std::unique_ptr<VectorFst<Arc>> r1_;
  std::unique_ptr<VectorFst<Arc>> r2_;
  std::unique_ptr<VectorFst<Arc>> r3_;
  std::unique_ptr<VectorFst<Arc>> r4_;
  std::unique_ptr<VectorFst<Arc>> r5_;
  std::unique_ptr<VectorFst<Arc>> i1_;
  std::unique_ptr<VectorFst<Arc>> i2_;
  std::unique_ptr<VectorFst<Arc>> i3_;
  std::unique_ptr<VectorFst<Arc>> s1_;
  std::unique_ptr<VectorFst<Arc>> s2_;
  std::unique_ptr<VectorFst<Arc>> s3_;
};

TEST_F(ReplaceTest, ReplaceFstMatcherTest) {
  ArcSort(g1_.get(), StdILabelCompare());
  ArcSort(g2_.get(), StdILabelCompare());
  ArcSort(g3_.get(), StdILabelCompare());
  ArcSort(g4_.get(), StdILabelCompare());

  std::vector<FstTuple> fst_array;
  fst_array.push_back(FstTuple(1, g1_.get()));
  fst_array.push_back(FstTuple(2, g2_.get()));
  fst_array.push_back(FstTuple(3, g3_.get()));
  fst_array.push_back(FstTuple(4, g4_.get()));
  ReplaceFst<Arc> replace(fst_array, ReplaceFstOptions<Arc>(1, true));
  replace.Start();

  // Searches for first arc (dial).
  Matcher<Fst<Arc>> matcher(replace, MATCH_INPUT);
  matcher.SetState(0);
  EXPECT_TRUE(matcher.Find(5));  // Find (dial).
  const Arc arc1 = matcher.Value();
  EXPECT_EQ(5, arc1.ilabel);
  EXPECT_EQ(5, arc1.olabel);
  EXPECT_EQ(1, arc1.nextstate);

  // Searches for first epsilon.
  matcher.SetState(arc1.nextstate);
  EXPECT_TRUE(matcher.Find(0));
  const Arc arc2 = matcher.Value();  // Loop arcs.
  EXPECT_EQ(kNoLabel, arc2.ilabel);
  EXPECT_EQ(0, arc2.olabel);
  EXPECT_EQ(1, arc2.nextstate);
  matcher.Next();
  EXPECT_FALSE(matcher.Done());
  const Arc arc3 = matcher.Value();  // Epsilon arcs.
  EXPECT_EQ(0, arc3.ilabel);
  EXPECT_EQ(0, arc3.olabel);
  EXPECT_EQ(2, arc3.nextstate);
  matcher.Next();
  EXPECT_TRUE(matcher.Done());

  // First recursion into G2.
  matcher.SetState(arc3.nextstate);
  EXPECT_TRUE(matcher.Find(10));  // Find (michael).
  const Arc arc4 = matcher.Value();
  EXPECT_EQ(10, arc4.ilabel);
  EXPECT_EQ(10, arc4.olabel);
  EXPECT_EQ(3, arc4.nextstate);
  matcher.Next();
  EXPECT_TRUE(matcher.Done());

  // Second epsilon recursion.
  EXPECT_TRUE(matcher.Find(-1));  // True epsilon only.
  const Arc arc5 = matcher.Value();
  EXPECT_EQ(0, arc5.ilabel);
  EXPECT_EQ(0, arc5.olabel);
  EXPECT_EQ(4, arc5.nextstate);
  matcher.Next();
  EXPECT_TRUE(matcher.Done());

  // riley and pops out.
  matcher.SetState(arc4.nextstate);
  EXPECT_TRUE(matcher.Find(11));  // Find (riley).
  const Arc arc6 = matcher.Value();
  matcher.SetState(arc6.nextstate);
  EXPECT_TRUE(matcher.Find(-1));
  matcher.Next();
  EXPECT_TRUE(matcher.Done());
}

TEST_F(ReplaceTest, ReplaceClassTest) {
  std::vector<FstTuple> fst_array;
  fst_array.push_back(FstTuple(1, g1_.get()));
  fst_array.push_back(FstTuple(2, g2_.get()));
  fst_array.push_back(FstTuple(3, g3_.get()));
  fst_array.push_back(FstTuple(4, g4_.get()));
  ReplaceFst<Arc> replace(fst_array, ReplaceFstOptions<Arc>(1, true));
  ASSERT_FALSE(replace.CyclicDependencies());
  ASSERT_TRUE(Verify(replace));

  VectorFst<Arc> fst1(replace);
  ASSERT_TRUE(Equal(fst1, *gout_));

  for (const bool safe : {false, true}) {
    ReplaceFst<Arc> copy_replace(replace, safe);
    ASSERT_TRUE(Verify(copy_replace));
    VectorFst<Arc> fst2(copy_replace);
    ASSERT_TRUE(Equal(fst2, *gout_));
  }
}

TEST_F(ReplaceTest, NullFstTerminalTest) {
  std::vector<FstTuple> fst_array;

  // Tests for null FST replacement.
  VectorFst<Arc> nfst;
  nfst.SetInputSymbols(g1_->InputSymbols());
  nfst.SetOutputSymbols(g1_->OutputSymbols());

  fst_array.clear();
  fst_array.push_back(FstTuple(1, g1_.get()));
  fst_array.push_back(FstTuple(2, g2_.get()));
  fst_array.push_back(FstTuple(3, g3_.get()));
  fst_array.push_back(FstTuple(4, &nfst));
  ReplaceFst<Arc> replace_null(fst_array, 1);
  ASSERT_FALSE(replace_null.CyclicDependencies());
  ASSERT_TRUE(Verify(replace_null));
}

TEST_F(ReplaceTest, NullTopologyTest) {
  std::vector<FstTuple> fst_array;

  // Tests for null FST replacement.
  VectorFst<Arc> nfst;
  nfst.SetInputSymbols(g1_->InputSymbols());
  nfst.SetOutputSymbols(g1_->OutputSymbols());

  // Tests for null topology.
  fst_array.clear();
  fst_array.push_back(FstTuple(1, &nfst));
  fst_array.push_back(FstTuple(2, g2_.get()));
  fst_array.push_back(FstTuple(3, g3_.get()));
  ReplaceFst<Arc> replace_null(fst_array, 1);
  VectorFst<Arc> rfst(replace_null);
  ASSERT_TRUE(Verify(rfst));
  ASSERT_EQ(rfst.Start(), kNoStateId);
}

TEST_F(ReplaceTest, EmptyReplaceArrayTest) {
  std::vector<FstTuple> fst_array;

  ReplaceFst<Arc> replace_empty(fst_array, 0);
  VectorFst<Arc> rfst(replace_empty);
  ASSERT_TRUE(Verify(rfst));
  ASSERT_EQ(rfst.Start(), kNoStateId);
}

TEST_F(ReplaceTest, ILabelSortedTest) {
  {
    ASSERT_FALSE(g1_->Properties(kILabelSorted, true));
    std::vector<FstTuple> fst_array;
    fst_array.push_back(FstTuple(1, g1_.get()));
    fst_array.push_back(FstTuple(2, g2_.get()));
    fst_array.push_back(FstTuple(3, g3_.get()));
    fst_array.push_back(FstTuple(4, g4_.get()));
    ReplaceFst<Arc> replace(fst_array, ReplaceFstOptions<Arc>(1, true));
    ASSERT_FALSE(replace.Properties(kILabelSorted, false));
    VectorFst<Arc> fst(replace);
    ASSERT_FALSE(fst.Properties(kILabelSorted, false));
  }
  ArcSort(g1_.get(), ILabelCompare<Arc>());
  ArcSort(g2_.get(), ILabelCompare<Arc>());
  ArcSort(g3_.get(), ILabelCompare<Arc>());
  ArcSort(g4_.get(), ILabelCompare<Arc>());
  // If all components are input label-sorted and
  {
    // non terminals are positive and forms a dense range, then the resulting
    // FST is input label-sorted.
    std::vector<FstTuple> fst_array;
    fst_array.push_back(FstTuple(1, g1_.get()));
    fst_array.push_back(FstTuple(2, g2_.get()));
    fst_array.push_back(FstTuple(3, g3_.get()));
    fst_array.push_back(FstTuple(4, g4_.get()));
    ReplaceFst<Arc> replace(fst_array, ReplaceFstOptions<Arc>(1, true));
    ASSERT_TRUE(replace.Properties(kILabelSorted, false));
    VectorFst<Arc> fst(replace);
    ASSERT_TRUE(fst.Properties(kILabelSorted, false));
  }
  {
    // non terminals are negative, then the resulting FST is input label-sorted.
    std::vector<FstTuple> fst_array;
    fst_array.push_back(FstTuple(-1, g1_.get()));
    fst_array.push_back(FstTuple(-2, g2_.get()));
    fst_array.push_back(FstTuple(-3, g3_.get()));
    fst_array.push_back(FstTuple(-4, g4_.get()));
    ReplaceFst<Arc> replace(fst_array, ReplaceFstOptions<Arc>(-1, true));
    ASSERT_TRUE(replace.Properties(kILabelSorted, false));
  }
  {
    // 'epsilon_on_replace' is 'false', then the resulting FST is input
    // label-sorted.
    std::vector<FstTuple> fst_array;
    fst_array.push_back(FstTuple(1, g1_.get()));
    fst_array.push_back(FstTuple(-2, g2_.get()));
    fst_array.push_back(FstTuple(3, g3_.get()));
    fst_array.push_back(FstTuple(4, g4_.get()));
    ReplaceFst<Arc> replace(fst_array, ReplaceFstOptions<Arc>(1, false));
    ASSERT_TRUE(replace.Properties(kILabelSorted, false));
  }
  {
    // otherwise, the resulting FST is not known to be input label-sorted.
    std::vector<FstTuple> fst_array;
    fst_array.push_back(FstTuple(1, g1_.get()));
    fst_array.push_back(FstTuple(-2, g2_.get()));
    fst_array.push_back(FstTuple(3, g3_.get()));
    fst_array.push_back(FstTuple(4, g4_.get()));
    ReplaceFst<Arc> replace(fst_array, ReplaceFstOptions<Arc>(1, true));
    ASSERT_FALSE(replace.Properties(kILabelSorted, false));
  }
}

TEST_F(ReplaceTest, CyclicReplaceTest) {
  std::vector<FstTuple> fst_array;

  fst_array.push_back(FstTuple(1, g1_.get()));
  fst_array.push_back(FstTuple(2, g3_.get()));
  fst_array.push_back(FstTuple(3, g2_.get()));
  fst_array.push_back(FstTuple(4, g4_.get()));

  ReplaceUtil<Arc> replace_util(fst_array, ReplaceUtilOptions(1));
  ASSERT_TRUE(replace_util.CyclicDependencies());

  std::vector<MutableFstTuple> mutable_fst_array;
  mutable_fst_array.push_back(MutableFstTuple(1, g1_->Copy()));
  mutable_fst_array.push_back(MutableFstTuple(2, g3_->Copy()));
  mutable_fst_array.push_back(MutableFstTuple(3, g2_->Copy()));
  mutable_fst_array.push_back(MutableFstTuple(4, g4_->Copy()));

  ReplaceUtil<Arc> mutable_replace_util(mutable_fst_array,
                                        ReplaceUtilOptions(1));
  ASSERT_TRUE(mutable_replace_util.CyclicDependencies());

  ReplaceFst<Arc> replace(fst_array, ReplaceFstOptions<Arc>(1, true));
  ASSERT_TRUE(replace.CyclicDependencies());
}

TEST_F(ReplaceTest, SCCPropertiesReplaceTest) {
  {
    std::vector<FstTuple> fst_array;

    fst_array.push_back(FstTuple(4, s1_.get()));
    fst_array.push_back(FstTuple(5, s2_.get()));

    ReplaceUtil<Arc> replace_util(fst_array, ReplaceUtilOptions(4));
    StateId scc1 = replace_util.SCC(4);
    ASSERT_EQ(replace_util.SCCProperties(scc1),
              kReplaceSCCRightLinear | kReplaceSCCNonTrivial);
    StateId scc2 = replace_util.SCC(5);
    ASSERT_EQ(scc1, scc2);
    ASSERT_EQ(replace_util.SCCProperties(scc2),
              kReplaceSCCRightLinear | kReplaceSCCNonTrivial);
  }

  {
    std::vector<FstTuple> fst_array;

    VectorFst<Arc> rfst1, rfst2;
    Reverse(*s1_, &rfst1);
    RmEpsilon(&rfst1);
    Reverse(*s2_, &rfst2);
    RmEpsilon(&rfst2);
    fst_array.push_back(FstTuple(4, &rfst1));
    fst_array.push_back(FstTuple(5, &rfst2));

    ReplaceUtil<Arc> replace_util(fst_array, ReplaceUtilOptions(4));
    StateId scc1 = replace_util.SCC(4);
    ASSERT_EQ(replace_util.SCCProperties(scc1),
              kReplaceSCCLeftLinear | kReplaceSCCNonTrivial);
    StateId scc2 = replace_util.SCC(5);
    ASSERT_EQ(scc1, scc2);
    ASSERT_EQ(replace_util.SCCProperties(scc2),
              kReplaceSCCLeftLinear | kReplaceSCCNonTrivial);
  }

  {
    std::vector<FstTuple> fst_array;

    fst_array.push_back(FstTuple(4, s3_.get()));
    fst_array.push_back(FstTuple(5, s2_.get()));

    ReplaceUtil<Arc> replace_util(fst_array, ReplaceUtilOptions(4));
    StateId scc = replace_util.SCC(4);
    ASSERT_EQ(replace_util.SCCProperties(scc), kReplaceSCCNonTrivial);
  }

  {
    std::vector<FstTuple> fst_array;

    fst_array.push_back(FstTuple(4, s3_.get()));

    ReplaceUtil<Arc> replace_util(fst_array, ReplaceUtilOptions(4));
    StateId scc = replace_util.SCC(4);
    ASSERT_EQ(replace_util.SCCProperties(scc),
              kReplaceSCCLeftLinear | kReplaceSCCRightLinear);
  }
}

TEST_F(ReplaceTest, ConnectReplaceTest) {
  std::vector<MutableFstTuple> mutable_fst_array, connected_fst_array;
  mutable_fst_array.push_back(MutableFstTuple(1, g1_->Copy()));
  mutable_fst_array.push_back(MutableFstTuple(2, g3_->Copy()));
  mutable_fst_array.push_back(MutableFstTuple(3, g2_->Copy()));
  mutable_fst_array.push_back(MutableFstTuple(4, g4_->Copy()));

  ReplaceUtil<Arc> replace_util(mutable_fst_array, ReplaceUtilOptions(1));
  ASSERT_TRUE(!replace_util.Connected());
  replace_util.Connect();
  replace_util.GetMutableFstPairs(&connected_fst_array);

  ReplaceUtil<Arc> connected_replace_util(connected_fst_array,
                                          ReplaceUtilOptions(1));
  ASSERT_TRUE(connected_replace_util.Connected());
}

TEST_F(ReplaceTest, ReplaceLabelsTest) {
  std::vector<FstTuple> fst_array;
  fst_array.push_back(FstTuple(1, g1_.get()));
  fst_array.push_back(FstTuple(2, g2_.get()));
  fst_array.push_back(FstTuple(3, g3_.get()));
  fst_array.push_back(FstTuple(4, g4_.get()));
  VectorFst<Arc> replaced_fst;
  Replace(fst_array, &replaced_fst, 1);
  ReplaceUtil<Arc> replace_util(fst_array, ReplaceUtilOptions(1));

  std::vector<FstTuple> replaced_fst_array;
  std::vector<Label> replace_labels;
  replace_labels.push_back(2);
  replace_labels.push_back(3);
  replace_labels.push_back(4);
  replace_util.ReplaceLabels(replace_labels);
  replace_util.Connect();
  replace_util.GetFstPairs(&replaced_fst_array);

  ASSERT_EQ(replaced_fst_array.size(), 1);
  ASSERT_TRUE(Equal(replaced_fst, *replaced_fst_array[0].second));
}

TEST_F(ReplaceTest, ReplaceBySizeTest) {
  std::vector<FstTuple> fst_array;
  fst_array.push_back(FstTuple(1, r1_.get()));
  fst_array.push_back(FstTuple(2, r2_.get()));
  fst_array.push_back(FstTuple(3, r3_.get()));
  fst_array.push_back(FstTuple(4, r4_.get()));
  fst_array.push_back(FstTuple(5, r5_.get()));
  ReplaceUtil<Arc> replace_util(fst_array, ReplaceUtilOptions(1));

  replace_util.ReplaceBySize(3, 2, 2);
  replace_util.Connect();
  std::vector<FstTuple> replaced_fst_array;
  replace_util.GetFstPairs(&replaced_fst_array);
  ASSERT_EQ(replaced_fst_array.size(), 3);
  ASSERT_EQ(replaced_fst_array[0].first, 1);
  ASSERT_TRUE(Equal(*i1_, *replaced_fst_array[0].second));
  ASSERT_EQ(replaced_fst_array[1].first, 3);
  ASSERT_TRUE(Equal(*i2_, *replaced_fst_array[1].second));
  ASSERT_EQ(replaced_fst_array[2].first, 5);
  ASSERT_TRUE(Equal(*i3_, *replaced_fst_array[2].second));
}

TEST_F(ReplaceTest, ReplaceByInstanceTest) {
  std::vector<FstTuple> fst_array;
  fst_array.push_back(FstTuple(1, r1_.get()));
  fst_array.push_back(FstTuple(2, r2_.get()));
  fst_array.push_back(FstTuple(3, r3_.get()));
  fst_array.push_back(FstTuple(4, r4_.get()));
  fst_array.push_back(FstTuple(5, r5_.get()));
  ReplaceUtil<Arc> replace_util(fst_array, ReplaceUtilOptions(1));

  replace_util.ReplaceByInstances(2);
  replace_util.Connect();
  std::vector<FstTuple> replaced_fst_array;
  replace_util.GetFstPairs(&replaced_fst_array);
  ASSERT_EQ(replaced_fst_array.size(), 3);
  ASSERT_EQ(replaced_fst_array[0].first, 1);
  ASSERT_TRUE(Equal(*i1_, *replaced_fst_array[0].second));
  ASSERT_EQ(replaced_fst_array[1].first, 3);
  ASSERT_TRUE(Equal(*i2_, *replaced_fst_array[1].second));
  ASSERT_EQ(replaced_fst_array[2].first, 5);
  ASSERT_TRUE(Equal(*i3_, *replaced_fst_array[2].second));
}

TEST_F(ReplaceTest, ArcIteratorFlagTest) {
  std::vector<FstTuple> fst_array;
  fst_array.push_back(FstTuple(1, g1_.get()));
  fst_array.push_back(FstTuple(2, g2_.get()));
  fst_array.push_back(FstTuple(3, g3_.get()));
  fst_array.push_back(FstTuple(4, g4_.get()));

  ReplaceFstOptions<Arc> opts(1, true);
  ReplaceFst<Arc> replace(fst_array, opts);

  std::vector<StateId> queue;
  queue.push_back(replace.Start());
  while (!queue.empty()) {
    Arc::StateId s = queue.back();
    queue.pop_back();
    ArcIterator<ReplaceFst<Arc>> aiter(replace, s);
    aiter.SetFlags(kArcNoCache, kArcNoCache);
    std::vector<Arc> arcs;
    while (!aiter.Done()) {
      arcs.push_back(aiter.Value());
      aiter.Next();
    }

    aiter.Reset();
    aiter.SetFlags(0, kArcNoCache);
    size_t i = 0;
    while (!aiter.Done()) {
      const Arc &arc = aiter.Value();
      ASSERT_TRUE(i < arcs.size());
      ASSERT_EQ(arc.ilabel, arcs[i].ilabel);
      ASSERT_EQ(arc.olabel, arcs[i].olabel);
      ASSERT_EQ(arc.weight, arcs[i].weight);
      ASSERT_EQ(arc.nextstate, arcs[i].nextstate);
      queue.push_back(arc.nextstate);
      aiter.Next();
      ++i;
    }
  }
}

TEST_F(ReplaceTest, MatcherTest) {
  ArcSort(g1_.get(), OLabelCompare<Arc>());
  ArcSort(g2_.get(), OLabelCompare<Arc>());
  ArcSort(g3_.get(), OLabelCompare<Arc>());
  ArcSort(g4_.get(), OLabelCompare<Arc>());
  EXPECT_TRUE(g1_->Properties(kILabelSorted, true));
  EXPECT_TRUE(g1_->Properties(kOLabelSorted, false));
  EXPECT_TRUE(g2_->Properties(kILabelSorted, true));
  EXPECT_TRUE(g2_->Properties(kOLabelSorted, false));
  EXPECT_TRUE(g3_->Properties(kILabelSorted, true));
  EXPECT_TRUE(g3_->Properties(kOLabelSorted, false));
  EXPECT_TRUE(g4_->Properties(kILabelSorted, true));
  EXPECT_TRUE(g4_->Properties(kOLabelSorted, false));

  std::vector<FstTuple> fst_array;
  fst_array.push_back(FstTuple(1, g1_.get()));
  fst_array.push_back(FstTuple(2, g2_.get()));
  fst_array.push_back(FstTuple(3, g3_.get()));
  fst_array.push_back(FstTuple(4, g4_.get()));

  ReplaceFstOptions<Arc> opts(1, true);
  ReplaceFst<Arc> replace(fst_array, opts);

  EXPECT_TRUE(replace.Properties(kILabelSorted, false));
  EXPECT_TRUE(replace.Properties(kOLabelSorted, false));

  path_->SetProperties(0, kILabelSorted | kOLabelSorted);
  VectorFst<Arc> cpath(ComposeFst<Arc>(*path_, replace));
  Connect(&cpath);
  RmEpsilon(&cpath);
  EXPECT_TRUE(Equal(*path_, cpath));
  cpath = ComposeFst<Arc>(replace, *path_);
  Connect(&cpath);
  RmEpsilon(&cpath);
  EXPECT_TRUE(Equal(*path_, cpath));

  lattice_->SetProperties(0, kILabelSorted | kOLabelSorted);
  StdVectorFst rlattice(*lattice_);
  RmEpsilon(&rlattice);
  StdVectorFst clattice(ComposeFst<Arc>(*lattice_, replace));
  Connect(&clattice);
  RmEpsilon(&clattice);
  EXPECT_TRUE(Equivalent(rlattice, clattice));
  clattice = ComposeFst<Arc>(replace, *lattice_);
  Connect(&clattice);
  RmEpsilon(&clattice);
  EXPECT_TRUE(Equivalent(rlattice, clattice));
}

TEST_F(ReplaceTest, GetStateTable) {
  std::vector<FstTuple> fst_array;
  fst_array.push_back(FstTuple(1, g1_.get()));
  fst_array.push_back(FstTuple(2, g2_.get()));
  fst_array.push_back(FstTuple(3, g3_.get()));
  fst_array.push_back(FstTuple(4, g4_.get()));
  ReplaceFst<Arc> replace(fst_array, ReplaceFstOptions<Arc>(1, true));
  EXPECT_EQ(0, replace.GetStateTable().Size());
  replace.Start();
  EXPECT_EQ(1, replace.GetStateTable().Size());
}

}  // namespace
}  // namespace fst
