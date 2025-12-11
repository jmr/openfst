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

#include <cstdint>
#include <string>

#include "absl/flags/flag.h"

ABSL_FLAG(bool, acceptor, false, "Input in acceptor format");
ABSL_FLAG(std::string, isymbols, "", "Input label symbol table");
ABSL_FLAG(std::string, osymbols, "", "Output label symbol table");
ABSL_FLAG(std::string, ssymbols, "", "State label symbol table");
ABSL_FLAG(bool, numeric, false, "Print numeric labels");
ABSL_FLAG(int32_t, precision, 5, "Set precision (number of char/float)");
ABSL_FLAG(std::string, float_format, "g",
          "Floating-point format: one of \"e\", \"f\", or \"g\"");
ABSL_FLAG(bool, show_weight_one, false,
          "Print/draw arc weights and final weights equal to Weight::One()");
ABSL_FLAG(std::string, title, "", "Set figure title");
ABSL_FLAG(bool, portrait, false, "Portrait mode (def: landscape)");
ABSL_FLAG(bool, vertical, false, "Draw bottom-to-top instead of left-to-right");
ABSL_FLAG(int32_t, fontsize, 14, "Set fontsize");
ABSL_FLAG(double, height, 11, "Set height");
ABSL_FLAG(double, width, 8.5, "Set width");
ABSL_FLAG(double, nodesep, 0.25,
          "Set minimum separation between nodes (see dot documentation)");
ABSL_FLAG(double, ranksep, 0.40,
          "Set minimum separation between ranks (see dot documentation)");

int fstdraw_main(int argc, char **argv);

int main(int argc, char **argv) { return fstdraw_main(argc, argv); }
