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

#ifndef OPENFST_LIB_FILE_UTIL_H_
#define OPENFST_LIB_FILE_UTIL_H_

#if defined(ANDROID) || defined(GOOGLE_UNSUPPORTED_OS_LOONIX) ||          \
    defined(NO_GOOGLE) || defined(OS_IOS) || defined(__ANDROID__) ||      \
    defined(__EMSCRIPTEN__) || defined(__Fuchsia__) || defined(_WIN32) || \
    defined(__native_client__)
#define NLP_FST_USE_PORTABLE_FILE 1
#endif  // defined...

#ifdef NLP_FST_USE_PORTABLE_FILE

#include <fstream>

namespace file {
using FileInStream = std::ifstream;
using FileOutStream = std::ofstream;
}  // namespace file

#else  // NLP_FST_USE_PORTABLE_FILE

#include "file/iostream/file_iostream.h"  // IWYU pragma: export

#endif  // NLP_FST_USE_PORTABLE_FILE

#endif  // OPENFST_LIB_FILE_UTIL_H_
