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

#include <cstdint>
#include <string>

#include "absl/flags/flag.h"

ABSL_FLAG(std::string, filename_prefix, "", "Prefix to append to filenames");
ABSL_FLAG(std::string, filename_suffix, "", "Suffix to append to filenames");
ABSL_FLAG(int32_t, generate_filenames, 0,
          "Generate N digit numeric filenames (def: use keys)");
ABSL_FLAG(std::string, begin_key, "",
          "First key to extract (def: first key in archive)");
ABSL_FLAG(std::string, end_key, "",
          "Last key to extract (def: last key in archive)");
// PrintStringsMain specific flag definitions.
ABSL_FLAG(bool, print_key, false, "Prefix each std::string by its key");
ABSL_FLAG(bool, print_weight, false, "Suffix each std::string by its weight");
ABSL_FLAG(std::string, entry_type, "line",
          "Entry type: one of : "
          "\"file\" (one FST per file), \"line\" (one FST per line)");
ABSL_FLAG(std::string, token_type, "symbol",
          "Token type: one of : "
          "\"symbol\", \"byte\", \"utf8\"");
ABSL_FLAG(std::string, symbols, "", "Label symbol table");
ABSL_FLAG(bool, initial_symbols, true,
          "Uses symbol table from the first Fst in archive for all entries.");

int farprintstrings_main(int argc, char **argv);

int main(int argc, char **argv) { return farprintstrings_main(argc, argv); }
