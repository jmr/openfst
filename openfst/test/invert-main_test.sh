#!/bin/bash
# Unit test for fstinvert.

TEST_SRCDIR=.

set -eou pipefail

TST="$TEST_TMPDIR"
FST="$TEST_SRCDIR/openfst"

BIN="$FST/bin"
DAT="$FST/test/testdata/invert"

source "$BIN"/setup.sh

"$BIN"/fstinvert "$DAT"/i1.fst "$TST"/i2.fst
"$BIN"/fstequal -v=1 "$DAT"/i2.fst "$TST"/i2.fst

"$BIN"/fstinvert "$DAT"/i1.fst > "$TST"/i2.fst
"$BIN"/fstequal -v=1 "$DAT"/i2.fst "$TST"/i2.fst

"$BIN"/fstinvert > "$TST"/i2.fst < "$DAT"/i1.fst
"$BIN"/fstequal -v=1 "$DAT"/i2.fst "$TST"/i2.fst
