#!/bin/bash
# Unit test for fstprune.

TEST_SRCDIR=.

set -eou pipefail

TST="$TEST_TMPDIR"
FST="$TEST_SRCDIR/openfst"

BIN="$FST/bin"
DAT="$FST/test/testdata/prune"

source "$BIN"/setup.sh

"$BIN"/fstprune -weight=0.5  "$DAT"/p1.fst "$TST"/p2.fst
"$BIN"/fstequal -v=1 "$DAT"/p2.fst "$TST"/p2.fst

"$BIN"/fstprune -nstate 2 "$DAT"/p3.fst > "$TST"/p4.fst
"$BIN"/fstequal -v=1 "$DAT"/p4.fst "$TST"/p4.fst

"$BIN"/fstprune -nstate 2 < "$DAT"/p3.fst > "$TST"/p4.fst
"$BIN"/fstequal -v=1 "$DAT"/p4.fst "$TST"/p4.fst
