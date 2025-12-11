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

#include "openfst/extensions/compress/quantize_weights.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <tuple>
#include <vector>

#include "absl/log/check.h"
#include "absl/log/log.h"
#include "absl/log/vlog_is_on.h"
#include "absl/types/span.h"
#include "openfst/lib/weight.h"

namespace fst {

namespace internal {

// Value to be quantized, along with weight.
struct WeightedInput {
  double input;
  double weight;

  // Comparison operator for sorting.
  friend bool operator<(const WeightedInput &x, const WeightedInput &y) {
    // Lexicographic comparison via tuple compare.
    return std::forward_as_tuple(x.input, x.weight) <
           std::forward_as_tuple(y.input, y.weight);
  }
};

void RunWeightedKMedianOneStep(absl::Span<const double> inputs,
                               absl::Span<const double> weights,
                               std::vector<double> *centers) {
  // Compute input to center mapping.
  std::vector<int> num_ids(centers->size(), 0);
  std::vector<std::vector<WeightedInput>> sum_ids(centers->size());
  for (int j = 0; j < inputs.size(); ++j) {
    const double input = inputs.at(j);
    const double weight = weights.at(j);
    int closest_id = 0;
    double closest_distance = std::abs(input - (*centers)[0]);
    for (int i = 1; i < centers->size(); ++i) {
      if (std::abs(input - (*centers)[i]) < closest_distance) {
        closest_id = i;
        closest_distance = std::abs(input - (*centers)[i]);
      }
    }
    num_ids[closest_id] += 1;
    sum_ids[closest_id].push_back(WeightedInput{input, weight});
  }
  // Compute the median for each center.
  double total_error = 0.0;
  for (int i = 0; i < centers->size(); ++i) {
    if (num_ids[i] == 0) {
      LOG(WARNING) << "dropped center " << i;
      continue;
    }
    std::sort(sum_ids[i].begin(), sum_ids[i].end());
    // Compute the total sum;
    double total_sum = 0.0;
    for (const auto &p : sum_ids[i]) {
      total_sum += p.weight;
    }
    // Compute the partial sum and the new center.
    double partial_sum = 0.0;
    double new_center = 0.0;
    for (const auto &p : sum_ids[i]) {
      partial_sum += p.weight;
      if (partial_sum >= total_sum / 2) {
        new_center = p.input;
        break;
      }
    }
    (*centers)[i] = new_center;

    if (VLOG_IS_ON(2)) {
      for (const auto &p : sum_ids[i]) {
        total_error += p.weight * std::abs(p.input - new_center);
      }
    }
  }

  VLOG(2) << "RunWeightedKMedianOneStep: total_error=" << total_error;
}

void RunWeightedKMedianOneStep(absl::Span<const double> inputs,
                               std::vector<double> *centers) {
  std::vector<double> weights(inputs.size());
  std::transform(inputs.begin(), inputs.end(), weights.begin(),
                 [](const double v) { return std::exp(-v); });
  return RunWeightedKMedianOneStep(inputs, weights, centers);
}

}  // namespace internal

void ComputeUniformQuantization(int num_levels, double min_level,
                                double max_level,
                                std::vector<double> *boundaries) {
  CHECK_GT(num_levels, 0);  // Crash OK
  boundaries->clear();
  boundaries->resize(num_levels, 0);
  const double quantization_delta = (max_level - min_level) / (num_levels - 1);
  for (int i = 0; i < num_levels; ++i) {
    (*boundaries)[i] = min_level + i * quantization_delta;
  }
}

double Quantize(absl::Span<const double> boundaries, double input,
                bool keep_zero, bool quantize_extreme_only) {
  if (keep_zero && std::abs(input) < fst::kDelta) {
    return 0.0;
  }
  if (input <= boundaries[0]) {
    return boundaries[0];
  } else if (input >= boundaries.back()) {
    return boundaries.back();
  } else if (quantize_extreme_only) {
    return input;
  } else {
    const auto it =
        std::upper_bound(boundaries.begin(), boundaries.end(), input);
    // upper_bound returns end or an element *it > input, but we already
    // checked for input >= back, so we always find something.
    DCHECK(it != boundaries.end());
    DCHECK(it != boundaries.begin());  // Similarly for begin.
    DCHECK_GT(*it, input);
    if (input - *(it - 1) < *it - input) {
      return *(it - 1);
    } else {
      return *it;
    }
  }
}

}  // namespace fst
