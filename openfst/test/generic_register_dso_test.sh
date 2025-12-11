#!/bin/bash
# Unit test that tests the generic behavior for registries loading DSOs.

TEST_SRCDIR=.

set -eou pipefail

FST="$TEST_SRCDIR/openfst"
LIB="$FST/test"

source "$FST/bin/setup.sh"

# Adds path to shared objects used below.
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$LIB
export LD_LIBRARY_PATH

"$LIB"/generic_register_dso_test_helper
