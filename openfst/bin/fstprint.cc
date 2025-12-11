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

ABSL_FLAG(bool, acceptor, false, "Input in acceptor format?");
ABSL_FLAG(std::string, isymbols, "", "Input label symbol table");
ABSL_FLAG(std::string, osymbols, "", "Output label symbol table");
ABSL_FLAG(std::string, ssymbols, "", "State label symbol table");
ABSL_FLAG(bool, numeric, false, "Print numeric labels?");
ABSL_FLAG(std::string, save_isymbols, "", "Save input symbol table to file");
ABSL_FLAG(std::string, save_osymbols, "", "Save output symbol table to file");
ABSL_FLAG(bool, show_weight_one, false,
          "Print/draw arc weights and final weights equal to semiring One?");
ABSL_FLAG(std::string, missing_symbol, "",
          "Symbol to print when lookup fails (default raises error)");

int fstprint_main(int argc, char **argv);

int main(int argc, char **argv) { return fstprint_main(argc, argv); }
