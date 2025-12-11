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

#ifndef OPENFST_TEST_POWER_WEIGHT_UTIL_H_
#define OPENFST_TEST_POWER_WEIGHT_UTIL_H_

#include <cstddef>
#include <utility>
#include <vector>

#include "absl/types/span.h"
#include "openfst/lib/power-weight.h"
#include "openfst/lib/sparse-power-weight.h"

namespace fst {
namespace test {

template <typename W, typename K>
std::vector<std::pair<int, float>> ToVector(
    const SparsePowerWeight<W, K> &w, const W &default_value = W::Zero()) {
  // For compatibility with PowerWeight version, check that the default value is
  // the correct one.
  CHECK_EQ(default_value, w.DefaultValue());  // Crash OK
  std::vector<std::pair<int, float>> result;
  using Iterator = typename SparsePowerWeight<W, K>::Iterator;
  for (Iterator iter(w); !iter.Done(); iter.Next()) {
    result.emplace_back(iter.Value().first, iter.Value().second.Value());
  }
  return result;
}

// Convert the weight to a vector ignoring the specified `default_value`.
// TODO: If PowerWeight had an Iterator with the same interface,
// we wouldn't need these overloads.
template <typename W, size_t n>
std::vector<std::pair<int, float>> ToVector(
    const PowerWeight<W, n> &w, const W &default_value = W::Zero()) {
  std::vector<std::pair<int, float>> result;
  for (int i = 0; i < n; ++i) {
    if (w.Value(i) != default_value) result.emplace_back(i, w.Value(i).Value());
  }
  return result;
}

template <typename PowerWeightT>
PowerWeightT CreateWeight(absl::Span<const std::pair<int, float>> w) {
  using ComponentWeight = typename PowerWeightT::Weight;
  PowerWeightT result;
  for (const auto &p : w) result.SetValue(p.first, ComponentWeight(p.second));
  return result;
}

}  // namespace test
}  // namespace fst

#endif  // OPENFST_TEST_POWER_WEIGHT_UTIL_H_
