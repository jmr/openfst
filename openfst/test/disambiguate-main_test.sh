#!/bin/bash
# Unit test for fstdisambiguate.

TEST_SRCDIR=.

set -eou pipefail

TST="$TEST_TMPDIR"
FST="$TEST_SRCDIR/openfst"

BIN="$FST/bin"
DAT="$FST/test/testdata/disambiguate"

source "$BIN"/setup.sh

# disambiguation
"$BIN"/fstdisambiguate "$DAT"/d1.fst "$TST"/d2.fst
"$BIN"/fstequal -v=1 "$DAT"/d2.fst "$TST"/d2.fst
