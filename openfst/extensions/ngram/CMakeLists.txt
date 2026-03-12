# Copyright 2026 The OpenFst Authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

add_library(fstngram bitmap-index.cc ngram-fst.cc nthbit.cc)
set_target_properties(
  fstngram
  PROPERTIES SOVERSION ${OPENFST_SOVERSION}
)

target_include_directories(
  fstngram PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
                  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(
  fstngram
  PUBLIC fst
         absl::base
         absl::log
         absl::memory
)

openfst_install(
  TARGETS fstngram
  EXPORT OpenFstTargets
  DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

set(NGRAM_HEADERS
    bitmap-index.h
    compact-louds-tree-fst.h
    louds-tree.h
    ngram-fst.h
    nthbit.h
)
openfst_install(FILES ${NGRAM_HEADERS}
                DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/openfst/extensions/ngram
)
