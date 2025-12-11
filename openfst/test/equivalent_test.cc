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
// Unit test for Equivalent.

#include "openfst/lib/equivalent.h"

#include <array>
#include <initializer_list>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "gtest/gtest.h"
#include "absl/flags/flag.h"
#include "absl/memory/memory.h"
#include "openfst/compat/compat_memory.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"
#include "openfst/lib/arc.h"
#include "openfst/lib/symbol-table.h"
#include "openfst/lib/vector-fst.h"

namespace fst {
namespace {

using Arc = StdArc;
using StateId = Arc::StateId;
using Weight = Arc::Weight;
using Label = Arc::Label;

// Names of files containing FAs. Each collection implements a different regular
// language. The unittest checks that:
//
//   - Equivalent(fa1, fa2) for every pair (fa1, fa2) in each
//   collection;
//
//   - ! Equivalent(fa1, fa2) for every pair (fa1, fa2) such that fa1
//   and fa2 belong to different collections.
static constexpr std::array fst_names{
    std::initializer_list<absl::string_view>{"e1", "e2", "e3"},
    std::initializer_list<absl::string_view>{"e4"},
    std::initializer_list<absl::string_view>{"e5", "e6", "e7"},
    std::initializer_list<absl::string_view>{"e8"},
    std::initializer_list<absl::string_view>{"e9", "e10"},
    std::initializer_list<absl::string_view>{"e11", "e12"}};

class EquivTest : public testing::Test {
 public:
  // Map from string IDs to FSTs.
  using Name2FstMap =
      std::map<absl::string_view, std::unique_ptr<VectorFst<Arc>>>;

 protected:
  void SetUp() override {
    const std::string kTestPath =
        std::string(".") + "/openfst/test/testdata/equivalent/";
    for (const auto &init_list : fst_names) {
      equiv_fsts_.push_back(Name2FstMap());
      for (absl::string_view s : init_list) {
        const std::string source = absl::StrCat(kTestPath, s, ".fst");
        equiv_fsts_.back()[s] = absl::WrapUnique(VectorFst<Arc>::Read(source));
      }
    }
    // Extra set of FA's for the empty language: constructed explicitly
    // to implement some weird/pathological cases.
    equiv_fsts_.push_back(Name2FstMap());

    // FA without states.
    auto empty_fa = std::make_unique<VectorFst<Arc>>();
    equiv_fsts_.back()["empty1"] = std::move(empty_fa);

    // FA containing a start state and a transition, but without final
    // states.
    empty_fa = std::make_unique<VectorFst<Arc>>();
    empty_fa->SetStart(empty_fa->AddState());
    empty_fa->AddState();
    empty_fa->AddArc(0, Arc(1, 1, Weight(0), 1));
    equiv_fsts_.back()["empty2"] = std::move(empty_fa);

    // FA containing a start and a final state connected by a zero-weight
    // transition.
    empty_fa = std::make_unique<VectorFst<Arc>>();
    empty_fa->SetStart(empty_fa->AddState());
    empty_fa->SetFinal(empty_fa->AddState(), Weight::One());
    empty_fa->AddArc(0, Arc(1, 1, Weight::Zero(), 1));
    equiv_fsts_.back()["empty3"] = std::move(empty_fa);
  }

  void TearDown() override {
    equiv_fsts_.clear();
  }
  // Collections of equivalent FSTs. Each collection implements a
  // different regular language.
  std::vector<Name2FstMap> equiv_fsts_;
};

// Checks FA equivalence within collections.
TEST_F(EquivTest, Equiv) {
  for (auto i = 0; i < equiv_fsts_.size(); ++i) {
    for (Name2FstMap::const_iterator it = equiv_fsts_[i].begin();
         it != equiv_fsts_[i].end(); ++it) {
      for (Name2FstMap::const_iterator it2 = it; it2 != equiv_fsts_[i].end();
           ++it2) {
        ASSERT_TRUE(Equivalent(*it->second, *it2->second));
        ASSERT_TRUE(Equivalent(*it2->second, *it->second));
      }
    }
  }
}

// Makes sure acceptors picked from different collections are not
// equivalent.
TEST_F(EquivTest, Nequiv) {
  // Disable symbol table compatibility check
  absl::SetFlag(&FLAGS_fst_compat_symbols, false);

  for (auto i = 0; i < equiv_fsts_.size(); ++i) {
    for (auto j = i + 1; j < equiv_fsts_.size(); ++j) {
      for (Name2FstMap::const_iterator it = equiv_fsts_[i].begin();
           it != equiv_fsts_[i].end(); ++it) {
        for (Name2FstMap::const_iterator it2 = equiv_fsts_[j].begin();
             it2 != equiv_fsts_[j].end(); ++it2) {
          ASSERT_FALSE(Equivalent(*it->second, *it2->second));
          ASSERT_FALSE(Equivalent(*it2->second, *it->second));
        }
      }
    }
  }
}

}  // namespace
}  // namespace fst
