#!/bin/bash
# Unit test for fstrangen.

TEST_SRCDIR=.

set -eou pipefail

TST="$TEST_TMPDIR"
FST="$TEST_SRCDIR/openfst"

BIN="$FST/bin"
DAT="$FST/test/testdata/randgen"

source "$BIN"/setup.sh

"$BIN"/fstrandgen --seed 2 "$DAT"/r1.fst "$TST"/r2.fst
"$BIN"/fstequal -v 1 "$DAT"/r2.fst "$TST"/r2.fst

"$BIN"/fstrandgen --seed 2 "$DAT"/r1.fst > "$TST"/r2.fst
"$BIN"/fstequal -v 1 "$DAT"/r2.fst "$TST"/r2.fst

"$BIN"/fstrandgen --seed 2 < "$DAT"/r1.fst > "$TST"/r2.fst
"$BIN"/fstequal -v 1 "$DAT"/r2.fst "$TST"/r2.fst
