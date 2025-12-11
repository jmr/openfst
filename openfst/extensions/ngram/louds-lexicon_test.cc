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

#include "openfst/extensions/ngram/louds-lexicon.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <sstream>
#include <string>

#include "gtest/gtest.h"
#include "openfst/lib/arc.h"
#include "openfst/lib/equal.h"
#include "openfst/lib/float-weight.h"
#include "openfst/lib/fst.h"
#include "openfst/lib/isomorphic.h"
#include "openfst/lib/mutable-fst.h"
#include "openfst/lib/vector-fst.h"

namespace fst {

using Compactor = LexiconCompactor<StdArc, uint16_t>;
using LexiconFst = CompactLexiconFst<StdArc, uint16_t>;

static std::string Mediumfile() {
  return std::string(".") +
      "/openfst/extensions/ngram/testdata/"
      "mediumlexicon.fst";
}

static std::string Largefile() {
  return std::string(".") +
      "/openfst/extensions/ngram/testdata/largelexicon.fst";
}

template <typename Writeable>
static std::size_t SerializedSize(const Writeable &w) {
  std::stringstream s;
  FstWriteOptions opts;
  opts.write_isymbols = false;
  opts.write_osymbols = false;
  CHECK(w.Write(s, opts));
  return s.str().size();
}

TEST(LoudsLexiconTest, EmptyTest) {
  fst::StdVectorFst vecfst;
  ASSERT_TRUE(HasCompactLexiconStructure(vecfst));
  LexiconFst lexicon(vecfst);

  EXPECT_TRUE(Isomorphic(vecfst, lexicon));
  EXPECT_EQ(229, SerializedSize(lexicon));
}

TEST(LoudsLexiconTest, BasicTest) {
  fst::StdVectorFst vecfst;
  vecfst.AddState();
  vecfst.AddState();
  vecfst.AddState();
  vecfst.AddState();

  vecfst.SetStart(0);
  vecfst.AddArc(0, StdArc(1, 0, StdArc::Weight::One(), 1));
  vecfst.AddArc(1, StdArc(2, 1, StdArc::Weight::One(), 2));
  vecfst.AddArc(2, StdArc(3, 0, StdArc::Weight::One(), 3));

  vecfst.SetFinal(3, StdArc::Weight::One());
  ASSERT_TRUE(HasCompactLexiconStructure(vecfst));
  LexiconFst lexicon(vecfst);

  EXPECT_TRUE(Isomorphic(vecfst, lexicon));
  EXPECT_EQ(162, SerializedSize(vecfst));
  EXPECT_EQ(271, SerializedSize(lexicon));

  // Check the copy constructor
  Compactor compactor(vecfst);
  const Compactor& compactor_copy(compactor);
  ASSERT_EQ(compactor.Start(), 0);
  ASSERT_EQ(compactor.Start(), compactor_copy.Start());
  ASSERT_EQ(compactor.NumStates(), 4);
  ASSERT_EQ(compactor.NumStates(), compactor_copy.NumStates());
  ASSERT_EQ(compactor.NumArcs(), 3);
  ASSERT_EQ(compactor.NumArcs(), compactor_copy.NumArcs());
  EXPECT_EQ(178, SerializedSize(compactor));
  EXPECT_EQ(178, SerializedSize(compactor_copy));
}

TEST(LoudsLexiconTest, StructureTestDAG) {
  fst::StdVectorFst vecfst;
  vecfst.AddState();
  vecfst.AddState();
  vecfst.AddState();
  vecfst.AddState();
  vecfst.AddState();
  vecfst.AddState();
  vecfst.AddState();
  vecfst.SetStart(0);
  vecfst.AddArc(0, StdArc(1, 0, StdArc::Weight::One(), 1));
  vecfst.AddArc(0, StdArc(2, 0, StdArc::Weight::One(), 2));
  vecfst.AddArc(1, StdArc(2, 0, StdArc::Weight::One(), 2));
  vecfst.AddArc(1, StdArc(5, 2, StdArc::Weight::One(), 4));
  vecfst.AddArc(2, StdArc(3, 1, StdArc::Weight::One(), 3));
  vecfst.AddArc(2, StdArc(4, 2, StdArc::Weight::One(), 4));
  vecfst.AddArc(3, StdArc(1, 0, StdArc::Weight::One(), 5));
  vecfst.AddArc(4, StdArc(2, 0, StdArc::Weight::One(), 5));
  vecfst.AddArc(5, StdArc(3, 0, StdArc::Weight::One(), 6));

  vecfst.SetFinal(6, StdArc::Weight::One());

  EXPECT_FALSE(HasCompactLexiconStructure(vecfst));
}

TEST(LoudsLexiconTest, StructureTestAccessible) {
  fst::StdVectorFst vecfst;
  vecfst.AddState();
  vecfst.AddState();
  vecfst.AddState();
  vecfst.AddState();
  vecfst.AddState();
  vecfst.AddState();
  vecfst.AddState();
  vecfst.SetStart(0);
  vecfst.AddArc(0, StdArc(1, 0, StdArc::Weight::One(), 1));
  vecfst.AddArc(0, StdArc(2, 0, StdArc::Weight::One(), 2));
  vecfst.AddArc(2, StdArc(3, 1, StdArc::Weight::One(), 3));
  vecfst.AddArc(3, StdArc(1, 0, StdArc::Weight::One(), 5));
  vecfst.AddArc(4, StdArc(2, 0, StdArc::Weight::One(), 5));
  vecfst.AddArc(5, StdArc(3, 0, StdArc::Weight::One(), 6));

  vecfst.SetFinal(6, StdArc::Weight::One());

  EXPECT_FALSE(HasCompactLexiconStructure(vecfst));
}

TEST(LoudsLexiconTest, StructureTestCoAccessible) {
  fst::StdVectorFst vecfst;
  vecfst.AddState();
  vecfst.AddState();
  vecfst.AddState();
  vecfst.AddState();
  vecfst.AddState();
  vecfst.AddState();
  vecfst.AddState();
  vecfst.SetStart(0);
  vecfst.AddArc(0, StdArc(1, 0, StdArc::Weight::One(), 1));
  vecfst.AddArc(0, StdArc(2, 0, StdArc::Weight::One(), 2));
  vecfst.AddArc(2, StdArc(3, 1, StdArc::Weight::One(), 3));
  vecfst.AddArc(2, StdArc(4, 2, StdArc::Weight::One(), 4));
  vecfst.AddArc(3, StdArc(1, 0, StdArc::Weight::One(), 5));
  vecfst.AddArc(4, StdArc(2, 0, StdArc::Weight::One(), 5));
  vecfst.AddArc(5, StdArc(3, 0, StdArc::Weight::One(), 6));

  vecfst.SetFinal(6, StdArc::Weight::One());

  EXPECT_FALSE(HasCompactLexiconStructure(vecfst));
}

TEST(LoudsLexiconTest, StructureTestEpsilonPath) {
  fst::StdVectorFst vecfst;
  vecfst.AddState();
  vecfst.AddState();
  vecfst.AddState();
  vecfst.AddState();
  vecfst.AddState();
  vecfst.AddState();
  vecfst.AddState();
  vecfst.SetStart(0);
  vecfst.AddArc(0, StdArc(1, 0, StdArc::Weight::One(), 1));
  vecfst.AddArc(0, StdArc(2, 0, StdArc::Weight::One(), 2));
  vecfst.AddArc(0, StdArc(6, 0, StdArc::Weight::One(), 5));
  vecfst.AddArc(1, StdArc(5, 2, StdArc::Weight::One(), 4));
  vecfst.AddArc(2, StdArc(3, 1, StdArc::Weight::One(), 3));
  vecfst.AddArc(2, StdArc(4, 2, StdArc::Weight::One(), 4));
  vecfst.AddArc(3, StdArc(1, 0, StdArc::Weight::One(), 5));
  vecfst.AddArc(4, StdArc(2, 0, StdArc::Weight::One(), 5));
  vecfst.AddArc(5, StdArc(3, 0, StdArc::Weight::One(), 6));

  vecfst.SetFinal(6, StdArc::Weight::One());
  EXPECT_FALSE(HasCompactLexiconStructure(vecfst));
}

TEST(LoudsLexiconTest, BasicTest2) {
  fst::StdVectorFst vecfst;
  vecfst.AddState();
  vecfst.AddState();
  vecfst.AddState();
  vecfst.AddState();
  vecfst.AddState();
  vecfst.AddState();
  vecfst.AddState();
  vecfst.SetStart(0);
  vecfst.AddArc(0, StdArc(1, 0, StdArc::Weight::One(), 1));
  vecfst.AddArc(0, StdArc(2, 0, StdArc::Weight::One(), 2));
  vecfst.AddArc(1, StdArc(5, 2, StdArc::Weight::One(), 4));
  vecfst.AddArc(2, StdArc(3, 1, StdArc::Weight::One(), 3));
  vecfst.AddArc(2, StdArc(4, 2, StdArc::Weight::One(), 4));
  vecfst.AddArc(3, StdArc(1, 0, StdArc::Weight::One(), 5));
  vecfst.AddArc(4, StdArc(2, 0, StdArc::Weight::One(), 5));
  vecfst.AddArc(5, StdArc(3, 0, StdArc::Weight::One(), 6));

  vecfst.SetFinal(6, StdArc::Weight::One());
  ASSERT_TRUE(HasCompactLexiconStructure(vecfst));
  LexiconFst lexicon(vecfst);

  EXPECT_TRUE(Isomorphic(vecfst, lexicon));
  EXPECT_EQ(275, SerializedSize(lexicon));
}

TEST(LoudsLexiconTest, ShuffledStateIdTest) {
  fst::StdVectorFst vecfst;
  vecfst.AddState();
  vecfst.AddState();
  vecfst.AddState();
  vecfst.AddState();
  vecfst.AddState();
  vecfst.AddState();
  vecfst.AddState();
  vecfst.SetStart(2);
  vecfst.AddArc(2, StdArc(1, 0, StdArc::Weight::One(), 1));
  vecfst.AddArc(2, StdArc(2, 0, StdArc::Weight::One(), 4));
  vecfst.AddArc(1, StdArc(5, 2, StdArc::Weight::One(), 3));
  vecfst.AddArc(4, StdArc(3, 1, StdArc::Weight::One(), 5));
  vecfst.AddArc(4, StdArc(4, 2, StdArc::Weight::One(), 3));
  vecfst.AddArc(5, StdArc(1, 0, StdArc::Weight::One(), 6));
  vecfst.AddArc(3, StdArc(2, 0, StdArc::Weight::One(), 6));
  vecfst.AddArc(6, StdArc(3, 0, StdArc::Weight::One(), 0));
  vecfst.SetFinal(0, StdArc::Weight::One());
  ASSERT_TRUE(HasCompactLexiconStructure(vecfst));
  LexiconFst lexicon(vecfst);

  EXPECT_TRUE(Isomorphic(vecfst, lexicon));
  EXPECT_EQ(275, SerializedSize(lexicon));
}

TEST(LoudsLexiconTest, MediumTest) {
  std::unique_ptr<StdMutableFst> fst(StdMutableFst::Read(Mediumfile()));
  ASSERT_TRUE(HasCompactLexiconStructure(*fst));
  LexiconFst lexicon(*fst);
  EXPECT_TRUE(Isomorphic(*fst, lexicon));

  EXPECT_EQ(51010, SerializedSize(*fst));
  EXPECT_EQ(5753, SerializedSize(lexicon));
}

TEST(LoudsLexiconTest, LargeTest) {
  std::unique_ptr<StdMutableFst> fst(StdMutableFst::Read(Largefile()));
  ASSERT_TRUE(HasCompactLexiconStructure(*fst));
  LexiconFst lexicon(*fst);
  EXPECT_TRUE(Isomorphic(*fst, lexicon));

  EXPECT_EQ(351890, SerializedSize(*fst));
  EXPECT_EQ(42373, SerializedSize(lexicon));
}

TEST(LoudsLexiconTest, ClosureTest) {
  fst::StdVectorFst vecfst;
  vecfst.AddState();
  vecfst.AddState();
  vecfst.AddState();
  vecfst.AddState();
  vecfst.SetStart(0);
  vecfst.AddArc(0, StdArc(1, 0, StdArc::Weight::One(), 1));
  vecfst.AddArc(1, StdArc(2, 1, StdArc::Weight::One(), 2));
  vecfst.AddArc(1, StdArc(4, 2, StdArc::Weight::One(), 2));
  vecfst.AddArc(2, StdArc(3, 0, StdArc::Weight::One(), 3));
  vecfst.AddArc(3, StdArc(5, 5, StdArc::Weight::One(), 0));

  vecfst.SetFinal(3, StdArc::Weight::One());
  ASSERT_TRUE(HasCompactLexiconStructure(vecfst));
  LexiconFst lexicon(vecfst);

  EXPECT_TRUE(Isomorphic(vecfst, lexicon));
  EXPECT_EQ(287, SerializedSize(lexicon));
}

TEST(LoudsLexiconTest, CopyTest) {
  fst::StdVectorFst vecfst;
  vecfst.AddState();
  vecfst.AddState();
  vecfst.AddState();
  vecfst.AddState();
  vecfst.SetStart(0);
  vecfst.AddArc(0, StdArc(1, 0, StdArc::Weight::One(), 1));
  vecfst.AddArc(1, StdArc(2, 1, StdArc::Weight::One(), 2));
  vecfst.AddArc(1, StdArc(4, 2, StdArc::Weight::One(), 2));
  vecfst.AddArc(2, StdArc(3, 0, StdArc::Weight::One(), 3));

  vecfst.SetFinal(3, StdArc::Weight::One());
  ASSERT_TRUE(HasCompactLexiconStructure(vecfst));
  LexiconFst lexicon(vecfst);

  EXPECT_TRUE(Isomorphic(vecfst, lexicon));
  for (const bool safe : {false, true}) {
    std::unique_ptr<StdFst> lptr(lexicon.Copy(safe));
    EXPECT_TRUE(Equal(lexicon, *lptr));
  }
  EXPECT_EQ(271, SerializedSize(lexicon));
}

TEST(LoudsLexiconTest, MultipleClosureTest) {
  fst::StdVectorFst vecfst;
  vecfst.AddState();
  vecfst.AddState();
  vecfst.AddState();
  vecfst.AddState();
  vecfst.SetStart(0);
  vecfst.AddArc(0, StdArc(1, 0, StdArc::Weight::One(), 1));
  vecfst.AddArc(1, StdArc(2, 1, StdArc::Weight::One(), 2));
  vecfst.AddArc(1, StdArc(4, 2, StdArc::Weight::One(), 2));
  vecfst.AddArc(2, StdArc(3, 0, StdArc::Weight::One(), 3));

  for (int i = 5; i < 20; i++) {
    vecfst.AddArc(3, StdArc(i, i, StdArc::Weight::One(), 0));
  }

  vecfst.SetFinal(3, StdArc::Weight::One());
  ASSERT_TRUE(HasCompactLexiconStructure(vecfst));
  LexiconFst lexicon(vecfst);

  EXPECT_TRUE(Isomorphic(vecfst, lexicon));
  EXPECT_EQ(511, SerializedSize(lexicon));
}

TEST(LoudsLexiconTest, LargeClosureTest) {
  std::unique_ptr<StdMutableFst> fst(StdMutableFst::Read(Largefile()));

  auto finalstate = 0;

  for (StateIterator<StdMutableFst> siter(*fst); !siter.Done(); siter.Next()) {
    if (fst->Final(siter.Value()) == StdMutableFst::Arc::Weight::One()) {
      finalstate = siter.Value();
      break;
    }
  }

  fst->AddArc(finalstate,
              StdArc(0, 0, StdMutableFst::Arc::Weight::One(), fst->Start()));
  ASSERT_TRUE(HasCompactLexiconStructure(*fst));
  LexiconFst lexicon(*fst);
  EXPECT_TRUE(Isomorphic(*fst, lexicon));
  EXPECT_EQ(42389, SerializedSize(lexicon));
}
}  // namespace fst
