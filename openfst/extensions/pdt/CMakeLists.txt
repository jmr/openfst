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

add_library(fstpdtscript getters.cc pdtscript.cc)
set_target_properties(
  fstpdtscript
  PROPERTIES SOVERSION ${OPENFST_SOVERSION}
)

target_include_directories(
  fstpdtscript PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
                      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(
  fstpdtscript
  PUBLIC fstscript
         absl::strings
         absl::status
         absl::statusor
)

openfst_install(
  TARGETS fstpdtscript
  EXPORT OpenFstTargets
  DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

set(PDT_HEADERS
    collection.h
    compose.h
    expand.h
    getters.h
    info.h
    paren.h
    pdt.h
    pdtlib.h
    pdtscript.h
    replace.h
    reverse.h
    shortest-path.h
)
openfst_install(FILES ${PDT_HEADERS}
                DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/openfst/extensions/pdt
)

set(PDT_BINS
    pdtcompose
    pdtexpand
    pdtinfo
    pdtreplace
    pdtreverse
    pdtshortestpath
)

foreach(BIN ${PDT_BINS})
  add_library(${BIN}-main ${BIN}-main.cc)
  target_link_libraries(
    ${BIN}-main
    PUBLIC fstpdtscript
           absl::flags
           absl::log
           absl::status
           absl::statusor
  )

  add_executable(${BIN} ${BIN}.cc)
  target_link_libraries(${BIN} PRIVATE ${BIN}-main absl::status absl::statusor)
  openfst_install(TARGETS ${BIN} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endforeach()
