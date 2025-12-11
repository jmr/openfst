# Copyright 2025 The OpenFst Authors.
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

""" Portable library target definition.

This serves as a central place to put all target dependent stuff.
Targeting Android and Windows.
"""

load("@rules_cc//cc:defs.bzl", "cc_library")

# Load build tooling macros
def register_extension_info(*args, **kwargs):
    return None

COMMON_DEPS = [
]

ANDROID_COPTS = [
    "-DHAVE_CONFIG_H",
    "-DNO_EXCEPTIONS",
    "-fexceptions",
]

IOS_COPTS = ANDROID_COPTS + [
    "-DFST_NO_DYNAMIC_LINKING",  # dynamic linking is not supported on ios
]

WINDOWS_COPTS = [
    "-U_HAS_EXCEPTIONS",
    "-D_HAS_EXCEPTIONS=1",
    "-fexceptions",
]

# Since much of the FST library is header based, the important defines should
# be required for all libraries who depend on an fst_cc_library.
COMMON_DEFINES = [
    "NO_GOOGLE",
]

def fst_cc_library(
        copts = [],
        deps = [],
        defines = [],
        **kwargs):
    portable_deps = [
        dep
        for dep in deps
        if dep not in COMMON_DEPS
    ]
    cc_library(
        deps = select({
            "@platforms//os:linux": deps,
            "@platforms//os:chromiumos": portable_deps,
            "@platforms//os:fuchsia": portable_deps,
            "//conditions:default": portable_deps,
        }),
        copts = copts + select({
            "@platforms//os:android": ANDROID_COPTS,
            "@platforms//os:ios": IOS_COPTS,
            "@platforms//os:chromiumos": ANDROID_COPTS,
            "@platforms//os:fuchsia": ANDROID_COPTS,
            "@platforms//os:windows": WINDOWS_COPTS,
            "//conditions:default": ["-Wall"],
        }),
        defines = defines + select({
            "@platforms//os:fuchsia": COMMON_DEFINES + ["FST_NO_DYNAMIC_LINKING"],
            "@platforms//os:linux": [],
            "@platforms//os:chromiumos": COMMON_DEFINES + ["FST_NO_DYNAMIC_LINKING"],
            "@platforms//os:windows": COMMON_DEFINES + ["FST_NO_DYNAMIC_LINKING"],
            "//conditions:default": COMMON_DEFINES,
        }),
        **kwargs
    )

register_extension_info(
    extension = fst_cc_library,
    label_regex_for_dep = "{extension_name}",
)
