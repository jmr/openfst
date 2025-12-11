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

ABSL_FLAG(std::string, key_prefix, "", "Prefix to append to keys");
ABSL_FLAG(std::string, key_suffix, "", "Suffix to append to keys");
ABSL_FLAG(int32_t, generate_keys, 0,
          "Generate N digit numeric keys (def: use file basenames)");
ABSL_FLAG(std::string, far_type, "default",
          "FAR file format type: one of: \"default\", \"fst\", "
          "\"stlist\", \"sttable\"");
ABSL_FLAG(std::string, arc_type, "standard", "Output arc type");
ABSL_FLAG(std::string, entry_type, "line",
          "Entry type: one of : "
          "\"file\" (one FST per file), \"line\" (one FST per line)");
ABSL_FLAG(std::string, fst_type, "", "Output FST type");
ABSL_FLAG(std::string, token_type, "symbol",
          "Token type: one of : "
          "\"symbol\", \"byte\", \"utf8\"");
ABSL_FLAG(std::string, symbols, "",
          "Label symbol table. Only applies to \"symbol\" tokens.");
ABSL_FLAG(std::string, unknown_symbol, "", "");
ABSL_FLAG(bool, file_list_input, false,
          "Each input file contains a list of files to be processed");
ABSL_FLAG(bool, keep_symbols, false, "Store symbol table in the FAR file");
ABSL_FLAG(bool, initial_symbols, true,
          "When keep_symbols is true, stores symbol table only for the first"
          " FST in archive.");

int farcompilestrings_main(int argc, char **argv);

int main(int argc, char **argv) { return farcompilestrings_main(argc, argv); }
