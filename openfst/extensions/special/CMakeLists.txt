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

add_library(fstphi-fst phi-fst.cc)
set_target_properties(
  fstphi-fst
  PROPERTIES SOVERSION ${OPENFST_SOVERSION}
)
target_include_directories(
  fstphi-fst PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
                    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(
  fstphi-fst
  PUBLIC fst
         absl::log
         absl::memory
         absl::flags
)

add_library(fstrho-fst rho-fst.cc)
set_target_properties(
  fstrho-fst
  PROPERTIES SOVERSION ${OPENFST_SOVERSION}
)
target_include_directories(
  fstrho-fst PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
                    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(
  fstrho-fst
  PUBLIC fst
         absl::log
         absl::memory
         absl::flags
)

add_library(fstsigma-fst sigma-fst.cc)
set_target_properties(
  fstsigma-fst
  PROPERTIES SOVERSION ${OPENFST_SOVERSION}
)
target_include_directories(
  fstsigma-fst PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
                      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(
  fstsigma-fst
  PUBLIC fst
         absl::log
         absl::memory
         absl::flags
)

add_executable(fstspecial fstspecial.cc)

# It depends on fstconvert-main which is in openfst/bin

target_link_libraries(
  fstspecial
  PRIVATE fstphi-fst
          fstrho-fst
          fstsigma-fst
          fstconvert-main
          absl::flags
          absl::log
)

openfst_install(
  TARGETS fstphi-fst fstrho-fst fstsigma-fst
  EXPORT OpenFstTargets
  DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

openfst_install(TARGETS fstspecial RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

set(SPECIAL_HEADERS
    phi-fst.h
    rho-fst.h
    sigma-fst.h
)

openfst_install(
  FILES ${SPECIAL_HEADERS}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/openfst/extensions/special
)
