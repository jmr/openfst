#!/bin/bash
# Unit test for fstrelabel.

TEST_SRCDIR=.

set -eou pipefail

TST="$TEST_TMPDIR"
FST="$TEST_SRCDIR/openfst"

BIN="$FST/bin"
DAT="$FST/test/testdata/relabel"

source "$BIN"/setup.sh

"$BIN"/fstrelabel --relabel_isymbols="$DAT"/in2.map "$DAT"/r1.fst "$TST"/r2.fst
"$BIN"/fstequal -v=1 "$DAT"/r2.fst "$TST"/r2.fst

"$BIN"/fstrelabel --relabel_osymbols="$DAT"/out3.map "$DAT"/r1.fst "$TST"/r3.fst
"$BIN"/fstequal -v=1 "$DAT"/r3.fst "$TST"/r3.fst

"$BIN"/fstrelabel --relabel_ipairs="$DAT"/in5.pairs "$DAT"/r4.fst "$TST"/r5.fst
"$BIN"/fstequal -v=1 "$DAT"/r5.fst "$TST"/r5.fst

"$BIN"/fstrelabel --relabel_opairs="$DAT"/out6.pairs "$DAT"/r4.fst "$TST"/r6.fst
"$BIN"/fstequal -v=1 "$DAT"/r6.fst "$TST"/r6.fst
