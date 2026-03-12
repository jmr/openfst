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

set(COMPAT_HEADERS
    compat.h
    compat_memory.h
    file_path.h
    flags.h
    init.h
    lock.h
    log.h
    status_macros.h
    unaligned.h
)
openfst_install(FILES ${COMPAT_HEADERS}
                DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/openfst/compat
)
