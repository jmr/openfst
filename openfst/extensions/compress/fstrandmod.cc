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
// Generates a random FST according to a class-specific transition model.

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <memory>
#include <string>

#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
#include "absl/flags/flag.h"
#include "absl/log/flags.h"
#include "openfst/extensions/compress/randmod.h"
#include "openfst/lib/arc.h"
#include "openfst/lib/float-weight.h"
#include "openfst/lib/fst-decl.h"
#include "openfst/lib/vector-fst.h"
#include "openfst/lib/weight.h"

ABSL_FLAG(int32_t, seed, time(0), "Random seed");
ABSL_FLAG(int32_t, states, 10, "# of states");
ABSL_FLAG(int32_t, labels, 2, "# of labels");
ABSL_FLAG(int32_t, classes, 1, "# of probability distributions");
ABSL_FLAG(bool, transducer, false, "Output a transducer");
ABSL_FLAG(bool, weights, false, "Output a weighted FST");

int main(int argc, char **argv) {
  using fst::StdArc;
  using fst::StdVectorFst;
  using fst::TropicalWeight;
  using fst::WeightGenerate;

  std::string usage = "Generates a random FST.\n\n  Usage: ";
  usage += argv[0];
  usage += "[out.fst]\n";

  absl::SetProgramUsageMessage(usage.c_str());
  const auto rest_args = absl::ParseCommandLine(argc, argv);
  
  if (rest_args.size() > 2) {
    LOG(INFO) << absl::ProgramUsageMessage();
    return 1;
  }

  std::string out_name =
      (rest_args.size() > 1 && (strcmp(rest_args[1], "-") != 0)) ? rest_args[1] : "";

  srand(absl::GetFlag(FLAGS_seed));

  int num_states = (rand() % absl::GetFlag(FLAGS_states)) + 1;
  int num_classes = (rand() % absl::GetFlag(FLAGS_classes)) + 1;
  int num_labels = (rand() % absl::GetFlag(FLAGS_labels)) + 1;

  StdVectorFst fst;
  using TropicalWeightGenerate = WeightGenerate<TropicalWeight>;
  std::unique_ptr<TropicalWeightGenerate> generate(
      absl::GetFlag(FLAGS_weights) ? new TropicalWeightGenerate(false)
                                   : nullptr);
  fst::RandMod<StdArc, TropicalWeightGenerate> rand_mod(
      num_states, num_classes, num_labels, absl::GetFlag(FLAGS_transducer),
      generate.get());
  rand_mod.Generate(&fst);

  return !fst.Write(out_name);
}
