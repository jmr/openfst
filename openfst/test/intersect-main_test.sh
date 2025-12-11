#!/bin/bash
# Unit test for fstintersect.

TEST_SRCDIR=.

set -eou pipefail

TST="$TEST_TMPDIR"
FST="$TEST_SRCDIR/openfst"

BIN="$FST/bin"
DAT="$FST/test/testdata/intersect"

source "$BIN"/setup.sh

"$BIN"/fstintersect "$DAT"/i1.fst "$DAT"/i2.fst "$TST"/i3.fst
"$BIN"/fstequal -v=1 "$DAT"/i3.fst "$TST"/i3.fst

"$BIN"/fstintersect --connect=false "$DAT"/i1.fst "$DAT"/i4.fst "$TST"/i5.fst
"$BIN"/fstequal -v=1 "$DAT"/i5.fst "$TST"/i5.fst

# stdout
"$BIN"/fstintersect --connect=false "$DAT"/i1.fst "$DAT"/i4.fst > "$TST"/i5.fst
"$BIN"/fstequal -v=1 "$DAT"/i5.fst "$TST"/i5.fst
