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
// Converts an RTN represented by FSTs and non-terminal labels into a PDT.

#include <cstdint>
#include <string>

#include "absl/flags/flag.h"
#include "openfst/lib/fst.h"

ABSL_FLAG(std::string, pdt_parentheses, "", "PDT parenthesis label pairs");
ABSL_FLAG(std::string, pdt_parser_type, "left",
          "Construction method, one of: \"left\", \"left_sr\"");
ABSL_FLAG(int64_t, start_paren_labels, fst::kNoLabel,
          "Index to use for the first inserted parentheses; if not "
          "specified, the next available label beyond the highest output "
          "label is used");
ABSL_FLAG(std::string, left_paren_prefix, "(_",
          "Prefix to attach to SymbolTable "
          "labels for inserted left parentheses");
ABSL_FLAG(std::string, right_paren_prefix, ")_",
          "Prefix to attach to SymbolTable "
          "labels for inserted right parentheses");

int pdtreplace_main(int argc, char **argv);

int main(int argc, char **argv) { return pdtreplace_main(argc, argv); }
