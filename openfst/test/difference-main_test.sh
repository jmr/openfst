#!/bin/bash
# Unit test for fstdifference.

TEST_SRCDIR=.

set -eou pipefail

TST="$TEST_TMPDIR"
FST="$TEST_SRCDIR/openfst"

BIN="$FST/bin"
DAT="$FST/test/testdata/difference"

source "$BIN"/setup.sh

"$BIN"/fstdifference --connect=false "$DAT"/d1.fst "$DAT"/d2.fst "$TST"/d3.fst
"$BIN"/fstequal -v=1 "$DAT"/d3.fst "$TST"/d3.fst

"$BIN"/fstdifference --connect=true "$DAT"/d4.fst "$DAT"/d2.fst "$TST"/d5.fst
"$BIN"/fstequal -v=1 "$DAT"/d5.fst "$TST"/d5.fst

/bin/rm "$TST"/d5.fst

# output to stdout
"$BIN"/fstdifference --connect=true "$DAT"/d4.fst "$DAT"/d2.fst > "$TST"/d5.fst
"$BIN"/fstequal -v=1 "$DAT"/d5.fst "$TST"/d5.fst
