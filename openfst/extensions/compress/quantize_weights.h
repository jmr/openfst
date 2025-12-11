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

#ifndef OPENFST_EXTENSIONS_COMPRESS_QUANTIZE_WEIGHTS_H_
#define OPENFST_EXTENSIONS_COMPRESS_QUANTIZE_WEIGHTS_H_

#include <algorithm>
#include <cmath>
#include <limits>
#include <type_traits>
#include <utility>
#include <vector>

#include "absl/log/check.h"
#include "absl/types/span.h"
#include "openfst/lib/float-weight.h"
#include "openfst/lib/fst.h"
#include "openfst/lib/mutable-fst.h"
#include "openfst/lib/weight.h"

namespace fst {

namespace internal {

// Runs one round of weighted k-median algorithm, with weights being
// exp(-input). This correponds to finding centers that minimize sum_{input}
// exp(-input) |input - center(input)|. If inputs are log-probabilities, then it
// is same as minimizing sum_p p | log p/quantized(p)|.
void RunWeightedKMedianOneStep(absl::Span<const double> inputs,
                               std::vector<double> *centers);

// As above, with arbirary weights.
void RunWeightedKMedianOneStep(absl::Span<const double> inputs,
                               absl::Span<const double> weights,
                               std::vector<double> *centers);

// Extracts all non-zero weights (both arc and final) from the FST,
// and returns them as a sorted vector.
template <typename Arc>
std::vector<double> ExtractWeights(const Fst<Arc> &fst) {
  using Weight = typename Arc::Weight;
  std::vector<double> weights;
  const WeightConvert<Weight, Log64Weight> to_log;
  // Read all labels and compute their corresponding weight values.
  for (StateIterator<Fst<Arc>> state_iter(fst); !state_iter.Done();
       state_iter.Next()) {
    const auto state = state_iter.Value();
    for (ArcIterator<Fst<Arc>> arc_iter(fst, state); !arc_iter.Done();
         arc_iter.Next()) {
      const auto &arc = arc_iter.Value();
      if (arc.weight != Weight::Zero()) {
        weights.emplace_back(to_log(arc.weight).Value());
      }
    }
    if (fst.Final(state) != Weight::Zero()) {
      weights.emplace_back(to_log(fst.Final(state)).Value());
    }
  }
  std::sort(weights.begin(), weights.end());
  return weights;
}

// Updates min_value and max_value if value is finite. Helper for
// GetMinMaxFiniteWeightValues.
inline void UpdateMinMax(const double value, double &min_value,
                         double &max_value) {
  if (std::isfinite(value)) {
    min_value = std::min(min_value, value);
    max_value = std::max(max_value, value);
  }
}

}  // namespace internal

// Returns a pair of the minimum and maximum finite weight values in the FST,
// returning (inf, -inf) if the FST is empty or otherwise has no finite weights.
// Requires that Arc::Weight has a Value() function returning a floating point
// type.
template <typename Arc>
std::pair<double, double> GetMinMaxFiniteWeightValues(const Fst<Arc> &fst) {
  static_assert(std::is_floating_point_v<
                std::decay_t<decltype(typename Arc::Weight().Value())>>);
  double min_value = std::numeric_limits<double>::infinity();
  double max_value = -std::numeric_limits<double>::infinity();
  for (StateIterator<Fst<Arc>> state_iter(fst); !state_iter.Done();
       state_iter.Next()) {
    const auto state = state_iter.Value();
    internal::UpdateMinMax(fst.Final(state).Value(), min_value, max_value);

    for (ArcIterator<Fst<Arc>> arc_iter(fst, state); !arc_iter.Done();
         arc_iter.Next()) {
      const double value = arc_iter.Value().weight.Value();
      internal::UpdateMinMax(value, min_value, max_value);
    }
  }
  return std::make_pair(min_value, max_value);
}

// Creates boundaries for uniform quantization. All values above max_level to
// max_level and smaller than min_level to min_level. Rest are quantized to
// accuracy (max_level - min_level) / num_levels.
void ComputeUniformQuantization(int num_levels, double min_level,
                                double max_level,
                                std::vector<double> *boundaries);

// Compute boundaries corresponding to weighted k-median algorithm.
template <typename Arc>
void ComputeWeightedkMedianQuantization(const Fst<Arc> &fst, int num_levels,
                                        std::vector<double> *boundaries,
                                        int num_steps = 100) {
  const std::vector<double> weights = internal::ExtractWeights(fst);
  // Computes the 99% percentile of weights to make the algorithm robust to
  // outlier weights.
  const int num_elements = weights.size();
  const int min_index = (num_elements - 1) * 0.01;
  const int max_index = (num_elements - 1) * 0.99;
  ComputeUniformQuantization(num_levels, weights[min_index], weights[max_index],
                             boundaries);
  for (int i = 0; i < num_steps; ++i) {
    internal::RunWeightedKMedianOneStep(weights, boundaries);
  }
}

// Given a input, computes the closest boundary value. If keep_zero is true,
// then if the input is 0, then it is not modified. If clip_weights_only,
// then it does not change values between boundaries[0] and boundaries.back().
double Quantize(absl::Span<const double> boundaries, double input,
                bool keep_zero = true, bool clip_weights_only = false);

// Quantizes the weights of the FST closest to the boundary specified. If
// quantize_zero_weight is true, then Weight::Zero() is quantized
// bounadries.back(). If clip_weights_only, then it does not change values
// between boundaries[0] and boundaries.back().
template <typename Arc>
void QuantizeWeights(absl::Span<const double> boundaries, MutableFst<Arc> *fst,
                     bool quantize_zero_weight = false,
                     bool clip_weights_only = false) {
  CHECK(!boundaries.empty());  // Crash OK
  using Weight = typename Arc::Weight;
  const WeightConvert<Weight, Log64Weight> to_log;
  for (StateIterator<Fst<Arc>> state_iter(*fst); !state_iter.Done();
       state_iter.Next()) {
    const auto state = state_iter.Value();
    for (MutableArcIterator<MutableFst<Arc>> arc_iter(fst, state);
         !arc_iter.Done(); arc_iter.Next()) {
      auto arc = arc_iter.Value();
      if (arc.weight != Weight::Zero()) {
        arc.weight = Quantize(boundaries, to_log(arc.weight).Value(),
                              /*keep_zero=*/true, clip_weights_only);
      } else if (quantize_zero_weight) {
        arc.weight = boundaries.back();
      }
      arc_iter.SetValue(arc);
    }
    if (fst->Final(state) != Weight::Zero()) {
      fst->SetFinal(state,
                    Quantize(boundaries, to_log(fst->Final(state)).Value(),
                             /*keep_zero=*/true, clip_weights_only));
    } else if (quantize_zero_weight) {
      fst->SetFinal(state, boundaries.back());
    }
  }
}

}  // namespace fst

#endif  // OPENFST_EXTENSIONS_COMPRESS_QUANTIZE_WEIGHTS_H_
