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

add_library(fstmpdtscript mpdtscript.cc)
set_target_properties(
  fstmpdtscript
  PROPERTIES SOVERSION ${OPENFST_SOVERSION}
)

target_include_directories(
  fstmpdtscript PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
                       $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(fstmpdtscript PUBLIC fstscript fstpdtscript absl::log)

set(MPDT_BINS
    mpdtcompose
    mpdtexpand
    mpdtinfo
    mpdtreverse
)

foreach(BIN ${MPDT_BINS})

  add_library(${BIN}-main ${BIN}-main.cc)

  target_link_libraries(${BIN}-main PUBLIC fstmpdtscript absl::flags absl::log)

  add_executable(${BIN} ${BIN}.cc)

  target_link_libraries(${BIN} PRIVATE ${BIN}-main)

  openfst_install(TARGETS ${BIN} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

endforeach()

openfst_install(
  TARGETS fstmpdtscript
  EXPORT OpenFstTargets
  DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

set(MPDT_HEADERS
    compose.h
    expand.h
    info.h
    mpdt.h
    mpdtlib.h
    mpdtscript.h
    read_write_utils.h
    reverse.h
)

openfst_install(FILES ${MPDT_HEADERS}
                DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/openfst/extensions/mpdt
)
