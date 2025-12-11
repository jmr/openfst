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

#include <string>

#include "absl/flags/flag.h"

ABSL_FLAG(std::string, arc_type, "standard", "Output arc type");
ABSL_FLAG(std::string, epsilon_symbol, "<eps>", "Epsilon symbol");
ABSL_FLAG(std::string, unknown_symbol, "<unk>", "Unknown word symbol");
ABSL_FLAG(std::string, vocab, "", "Path to the vocabulary file");
ABSL_FLAG(std::string, out, "", "Path to the output binary");
ABSL_FLAG(std::string, save_isymbols, "", "Save input symbol table to file");
ABSL_FLAG(std::string, save_fsymbols, "", "Save feature symbol table to file");
ABSL_FLAG(std::string, save_osymbols, "", "Save output symbol table to file");

int fstlinear_main(int argc, char **argv);

int main(int argc, char **argv) { return fstlinear_main(argc, argv); }
