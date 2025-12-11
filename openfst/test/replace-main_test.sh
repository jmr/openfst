#!/bin/bash
# Unit test for fstreplace.

TEST_SRCDIR=.

set -eou pipefail

TST="$TEST_TMPDIR"
FST="$TEST_SRCDIR/openfst"

BIN="$FST/bin"
DAT="$FST/test/testdata/replace"

source "$BIN"/setup.sh

"$BIN"/fstreplace --call_arc_labeling="neither" "$DAT"/g1.fst 1 "$DAT"/g2.fst 2 \
  "$DAT"/g3.fst 3 "$DAT"/g4.fst 4 "$TST"/g_out.fst
"$BIN"/fstequal -v=1 "$DAT"/g_out.fst "$TST"/g_out.fst

"$BIN"/fstreplace --call_arc_labeling="neither" "$DAT"/g1.fst 1 "$DAT"/g2.fst 2 \
  "$DAT"/g3.fst 3 "$DAT"/g4.fst 4 > "$TST"/g_out.fst
"$BIN"/fstequal -v=1 "$DAT"/g_out.fst "$TST"/g_out.fst
