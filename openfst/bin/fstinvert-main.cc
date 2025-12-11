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
// Inverts a transduction.

#include <cstring>
#include <memory>
#include <string>

#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
#include "absl/flags/flag.h"
#include "absl/log/flags.h"
#include "openfst/script/fst-class.h"
#include "openfst/script/invert.h"

int fstinvert_main(int argc, char **argv) {
  namespace s = fst::script;
  using fst::script::MutableFstClass;

  std::string usage = "Inverts a transduction.\n\n  Usage: ";
  usage += argv[0];
  usage += " [in.fst [out.fst]]\n";

  absl::SetProgramUsageMessage(usage.c_str());
  const auto rest_args = absl::ParseCommandLine(argc, argv);
  
  
  if (rest_args.size() > 3) {
    LOG(INFO) << absl::ProgramUsageMessage();
    return 1;
  }

  const std::string in_name =
      (rest_args.size() > 1 && strcmp(rest_args[1], "-") != 0) ? rest_args[1] : "";
  const std::string out_name =
      (rest_args.size() > 2 && strcmp(rest_args[2], "-") != 0) ? rest_args[2] : "";

  std::unique_ptr<MutableFstClass> fst(MutableFstClass::Read(in_name, true));
  if (!fst) return 1;

  s::Invert(fst.get());

  return !fst->Write(out_name);
}
