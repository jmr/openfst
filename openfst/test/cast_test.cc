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

#include <cmath>

#include "gtest/gtest.h"
#include "openfst/lib/arc.h"
#include "openfst/lib/fst.h"
#include "openfst/lib/vector-fst.h"

namespace fst {
namespace {

// `Std*` aliases are defined for us, but not `Log*` ones.
using LogFst = Fst<LogArc>;
using LogVectorFst = VectorFst<LogArc>;

// Make a simple test fst with one state and two arcs, both
// with value `Weight(1)`.
template <typename Arc>
VectorFst<Arc> MakeTestFst() {
  using Weight = typename Arc::Weight;
  VectorFst<Arc> fst;
  fst.AddState();
  fst.AddArc(0, Arc(0, 0, Weight(1), 0));
  fst.AddArc(0, Arc(0, 0, Weight(1), 0));
  return fst;
}

// Sums all arcs for `state_id`.
template <typename Arc>
typename Arc::Weight SumArcWeightsForState(const Fst<Arc> &fst,
                                           typename Arc::StateId state_id) {
  using Weight = typename Arc::Weight;
  Weight w = Weight::Zero();
  for (ArcIterator<Fst<Arc>> iter(fst, state_id); !iter.Done(); iter.Next()) {
    w = Plus(w, iter.Value().weight);
  }
  return w;
}

TEST(CastTest, SumsAsCastedType) {
  StdVectorFst std_fst = MakeTestFst<StdArc>();
  LogVectorFst log_fst = MakeTestFst<LogArc>();

  EXPECT_FLOAT_EQ(SumArcWeightsForState(std_fst, 0).Value(), 1.0);
  EXPECT_FLOAT_EQ(SumArcWeightsForState(log_fst, 0).Value(),
                  1.0 - std::log(2.0));

  // Casting should give the answer as the casted-to type.
  LogVectorFst std_to_log_fst;
  Cast(std_fst, &std_to_log_fst);
  EXPECT_FLOAT_EQ(SumArcWeightsForState(std_to_log_fst, 0).Value(),
                  1.0 - std::log(2.0));

  StdVectorFst log_to_std_fst;
  Cast(log_fst, &log_to_std_fst);
  EXPECT_FLOAT_EQ(SumArcWeightsForState(log_to_std_fst, 0).Value(), 1.0);
}

TEST(CastTest, PropertiesDoesNotCrash) {
  StdVectorFst std_fst = MakeTestFst<StdArc>();
  LogVectorFst std_to_log_fst;
  Cast(std_fst, &std_to_log_fst);
  // We don't actually need to compute any properties, just calling
  // the function will be enough to find WPD check failures.
  EXPECT_EQ(std_fst.Properties(/*mask=*/0, /*test=*/true), 0);
  EXPECT_EQ(std_to_log_fst.Properties(/*mask=*/0, /*test=*/true), 0);
}

}  // namespace
}  // namespace fst
