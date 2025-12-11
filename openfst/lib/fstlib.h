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
// This is a library for constructing, combining, optimizing, and searching
// "weighted finite-state transducers" (FSTs). Weighted finite-state transducers
// are automata where each transition has an input label, an output label, and a
// weight. The more familiar finite-state acceptor is represented as a
// transducer with each transition's input and output the same. Finite-state
// acceptors are used to represent sets of strings (specifically, "regular" or
// "rational sets"); finite-state transducers are used to represent binary
// relations between pairs of strings (specifically, "rational transductions").
// The weights can be used to represent the cost of taking a particular
// transition.
//
// In this library, transducers are templated on the Arc (transition)
// definition, which allows changing the label, weight, and state ID sets.
// Labels and state IDs are restricted to signed integral types but the weight
// can be an arbitrary type whose members satisfy certain algebraic ("semiring")
// properties.
//
// This convenience file includes all other FST header files.

#ifndef OPENFST_LIB_FSTLIB_H_
#define OPENFST_LIB_FSTLIB_H_

// Do not let Include-What-You-Use suggest this file.
// IWYU pragma: private
#include "openfst/lib/accumulator.h"
#include "openfst/lib/add-on.h"
#include "openfst/lib/arc-map.h"
#include "openfst/lib/arc.h"
#include "openfst/lib/arcfilter.h"
#include "openfst/lib/arcsort.h"
#include "openfst/lib/cache.h"
#include "openfst/lib/cc-visitors.h"
#include "openfst/lib/closure.h"
#include "openfst/lib/compact-fst.h"
#include "openfst/lib/complement.h"
#include "openfst/lib/compose-filter.h"
#include "openfst/lib/compose.h"
#include "openfst/lib/concat.h"
#include "openfst/lib/connect.h"
#include "openfst/lib/const-fst.h"
#include "openfst/lib/determinize.h"
#include "openfst/lib/dfs-visit.h"
#include "openfst/lib/difference.h"
#include "openfst/lib/disambiguate.h"
#include "openfst/lib/edit-fst.h"
#include "openfst/lib/encode.h"
#include "openfst/lib/epsnormalize.h"
#include "openfst/lib/equal.h"
#include "openfst/lib/equivalent.h"
#include "openfst/lib/expanded-fst.h"
#include "openfst/lib/expectation-weight.h"
#include "openfst/lib/factor-weight.h"
#include "openfst/lib/float-weight.h"
#include "openfst/lib/fst.h"
#include "openfst/lib/generic-register.h"
#include "openfst/lib/heap.h"
#include "openfst/lib/impl-to-fst.h"
#include "openfst/lib/intersect.h"
#include "openfst/lib/interval-set.h"
#include "openfst/lib/invert.h"
#include "openfst/lib/isomorphic.h"
#include "openfst/lib/label-reachable.h"
#include "openfst/lib/lexicographic-weight.h"
#include "openfst/lib/lookahead-filter.h"
#include "openfst/lib/lookahead-matcher.h"
#include "openfst/lib/matcher-fst.h"
#include "openfst/lib/matcher.h"
#include "openfst/lib/minimize.h"
#include "openfst/lib/mutable-fst.h"
#include "openfst/lib/pair-weight.h"
#include "openfst/lib/partition.h"
#include "openfst/lib/power-weight.h"
#include "openfst/lib/product-weight.h"
#include "openfst/lib/project.h"
#include "openfst/lib/properties.h"
#include "openfst/lib/prune.h"
#include "openfst/lib/push.h"
#include "openfst/lib/queue.h"
#include "openfst/lib/randequivalent.h"
#include "openfst/lib/randgen.h"
#include "openfst/lib/rational.h"
#include "openfst/lib/register.h"
#include "openfst/lib/relabel.h"
#include "openfst/lib/replace-util.h"
#include "openfst/lib/replace.h"
#include "openfst/lib/reverse.h"
#include "openfst/lib/reweight.h"
#include "openfst/lib/rmepsilon.h"
#include "openfst/lib/rmfinalepsilon.h"
#include "openfst/lib/shortest-distance.h"
#include "openfst/lib/shortest-path.h"
#include "openfst/lib/signed-log-weight.h"
#include "openfst/lib/sparse-power-weight.h"
#include "openfst/lib/sparse-tuple-weight.h"
#include "openfst/lib/state-map.h"
#include "openfst/lib/state-reachable.h"
#include "openfst/lib/state-table.h"
#include "openfst/lib/statesort.h"
#include "openfst/lib/string-weight.h"
#include "openfst/lib/string.h"
#include "openfst/lib/symbol-table-ops.h"
#include "openfst/lib/symbol-table.h"
#include "openfst/lib/synchronize.h"
#include "openfst/lib/topsort.h"
#include "openfst/lib/tuple-weight.h"
#include "openfst/lib/union-find.h"
#include "openfst/lib/union.h"
#include "openfst/lib/util.h"
#include "openfst/lib/vector-fst.h"
#include "openfst/lib/verify.h"
#include "openfst/lib/visit.h"
#include "openfst/lib/weight.h"

#endif  // OPENFST_LIB_FSTLIB_H_
