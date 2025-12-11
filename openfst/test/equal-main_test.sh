#!/bin/bash
# Unit test for fstequal.

TEST_SRCDIR=.

set -eou pipefail

TST="$TEST_TMPDIR"
FST="$TEST_SRCDIR/openfst"

BIN="$FST/bin"
DAT="$FST/test/testdata/intersect"  # doesn't matter which directory.

source "$BIN"/setup.sh

"$BIN"/fstequal -v=1 "$DAT"/i1.fst "$DAT"/i1.fst
"$BIN"/fstequal -v=1 - "$DAT"/i1.fst < "$DAT"/i1.fst
"$BIN"/fstequal -v=1 "$DAT"/i1.fst - < "$DAT"/i1.fst
