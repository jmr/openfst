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
// Draws a binary FSTs in the Graphviz dot text format.

#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>

#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
#include "absl/flags/declare.h"
#include "absl/flags/flag.h"
#include "absl/log/flags.h"
#include "absl/log/log.h"
#include "openfst/lib/file-util.h"
#include "openfst/lib/symbol-table.h"
#include "openfst/script/draw.h"
#include "openfst/script/fst-class.h"

ABSL_DECLARE_FLAG(bool, acceptor);
ABSL_DECLARE_FLAG(std::string, isymbols);
ABSL_DECLARE_FLAG(std::string, osymbols);
ABSL_DECLARE_FLAG(std::string, ssymbols);
ABSL_DECLARE_FLAG(bool, numeric);
ABSL_DECLARE_FLAG(int32_t, precision);
ABSL_DECLARE_FLAG(std::string, float_format);
ABSL_DECLARE_FLAG(bool, show_weight_one);
ABSL_DECLARE_FLAG(std::string, title);
ABSL_DECLARE_FLAG(bool, portrait);
ABSL_DECLARE_FLAG(bool, vertical);
ABSL_DECLARE_FLAG(int32_t, fontsize);
ABSL_DECLARE_FLAG(double, height);
ABSL_DECLARE_FLAG(double, width);
ABSL_DECLARE_FLAG(double, nodesep);
ABSL_DECLARE_FLAG(double, ranksep);

int fstdraw_main(int argc, char **argv) {
  namespace s = fst::script;
  using fst::SymbolTable;
  using fst::script::FstClass;

  std::string usage = "Prints out binary FSTs in dot text format.\n\n  Usage: ";
  usage += argv[0];
  usage += " [binary.fst [text.dot]]\n";

  absl::SetProgramUsageMessage(usage.c_str());
  const auto rest_args = absl::ParseCommandLine(argc, argv);
  
  
  if (rest_args.size() > 3) {
    LOG(INFO) << absl::ProgramUsageMessage();
    return 1;
  }

  const std::string in_name =
      argc > 1 && strcmp(rest_args[1], "-") != 0 ? rest_args[1] : "";

  std::unique_ptr<FstClass> fst(FstClass::Read(in_name));
  if (!fst) return 1;

  const std::string out_name =
      argc > 2 && strcmp(rest_args[2], "-") != 0 ? rest_args[2] : "";
  file::FileOutStream fstrm;
  if (!out_name.empty()) {
    fstrm.open(out_name);
    if (!fstrm) {
      LOG(ERROR) << argv[0] << ": Open failed, file = " << out_name;
      return 1;
    }
  }
  std::ostream &ostrm = fstrm.is_open() ? fstrm : std::cout;

  std::unique_ptr<const SymbolTable> isyms;
  if (!absl::GetFlag(FLAGS_isymbols).empty() && !absl::GetFlag(FLAGS_numeric)) {
    isyms.reset(
        SymbolTable::ReadText(absl::GetFlag(FLAGS_isymbols),
                              absl::GetFlag(FLAGS_fst_field_separator)));
    if (!isyms) return 1;
  }

  std::unique_ptr<const SymbolTable> osyms;
  if (!absl::GetFlag(FLAGS_osymbols).empty() && !absl::GetFlag(FLAGS_numeric)) {
    osyms.reset(
        SymbolTable::ReadText(absl::GetFlag(FLAGS_osymbols),
                              absl::GetFlag(FLAGS_fst_field_separator)));
    if (!osyms) return 1;
  }

  std::unique_ptr<const SymbolTable> ssyms;
  if (!absl::GetFlag(FLAGS_ssymbols).empty() && !absl::GetFlag(FLAGS_numeric)) {
    ssyms.reset(
        SymbolTable::ReadText(absl::GetFlag(FLAGS_osymbols),
                              absl::GetFlag(FLAGS_fst_field_separator)));
    if (!ssyms) return 1;
  }

  if (!isyms && !absl::GetFlag(FLAGS_numeric) && fst->InputSymbols()) {
    isyms.reset(fst->InputSymbols()->Copy());
  }

  if (!osyms && !absl::GetFlag(FLAGS_numeric) && fst->OutputSymbols()) {
    osyms.reset(fst->OutputSymbols()->Copy());
  }

  // "dest" is only used for the name of the file in error messages.
  const std::string dest = out_name.empty() ? "stdout" : out_name;
  s::Draw(*fst, isyms.get(), osyms.get(), ssyms.get(),
          absl::GetFlag(FLAGS_acceptor), absl::GetFlag(FLAGS_title),
          absl::GetFlag(FLAGS_width), absl::GetFlag(FLAGS_height),
          absl::GetFlag(FLAGS_portrait), absl::GetFlag(FLAGS_vertical),
          absl::GetFlag(FLAGS_ranksep), absl::GetFlag(FLAGS_nodesep),
          absl::GetFlag(FLAGS_fontsize), absl::GetFlag(FLAGS_precision),
          absl::GetFlag(FLAGS_float_format),
          absl::GetFlag(FLAGS_show_weight_one), ostrm, dest);

  return 0;
}
