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

#include "openfst/script/prune.h"

#include <cstdint>

#include "openfst/lib/properties.h"
#include "openfst/script/fst-class.h"
#include "openfst/script/script-impl.h"
#include "openfst/script/weight-class.h"

namespace fst {
namespace script {

void Prune(const FstClass &ifst, MutableFstClass *ofst,
           const WeightClass &weight_threshold, int64_t state_threshold,
           float delta) {
  if (!internal::ArcTypesMatch(ifst, *ofst, "Prune") ||
      !ofst->WeightTypesMatch(weight_threshold, "Prune")) {
    ofst->SetProperties(kError, kError);
    return;
  }
  FstPruneArgs1 args{ifst, ofst, weight_threshold, state_threshold, delta};
  Apply<Operation<FstPruneArgs1>>("Prune", ifst.ArcType(), &args);
}

void Prune(MutableFstClass *fst, const WeightClass &weight_threshold,
           int64_t state_threshold, float delta) {
  if (!fst->WeightTypesMatch(weight_threshold, "Prune")) {
    fst->SetProperties(kError, kError);
    return;
  }
  FstPruneArgs2 args{fst, weight_threshold, state_threshold, delta};
  Apply<Operation<FstPruneArgs2>>("Prune", fst->ArcType(), &args);
}

REGISTER_FST_OPERATION_3ARCS(Prune, FstPruneArgs1);
REGISTER_FST_OPERATION_3ARCS(Prune, FstPruneArgs2);

}  // namespace script
}  // namespace fst
