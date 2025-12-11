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

#ifndef OPENFST_SCRIPT_DIFFERENCE_H_
#define OPENFST_SCRIPT_DIFFERENCE_H_

#include <tuple>

#include "openfst/lib/compose.h"
#include "openfst/lib/difference.h"
#include "openfst/lib/fst.h"
#include "openfst/lib/mutable-fst.h"
#include "openfst/script/compose.h"
#include "openfst/script/fst-class.h"

namespace fst {
namespace script {

using FstDifferenceArgs = std::tuple<const FstClass &, const FstClass &,
                                     MutableFstClass *, const ComposeOptions &>;

template <class Arc>
void Difference(FstDifferenceArgs *args) {
  const Fst<Arc> &ifst1 = *std::get<0>(*args).GetFst<Arc>();
  const Fst<Arc> &ifst2 = *std::get<1>(*args).GetFst<Arc>();
  MutableFst<Arc> *ofst = std::get<2>(*args)->GetMutableFst<Arc>();
  const auto &opts = std::get<3>(*args);
  Difference(ifst1, ifst2, ofst, opts);
}

void Difference(const FstClass &ifst1, const FstClass &ifst2,
                MutableFstClass *ofst,
                const ComposeOptions &opts = ComposeOptions());

}  // namespace script
}  // namespace fst

#endif  // OPENFST_SCRIPT_DIFFERENCE_H_
