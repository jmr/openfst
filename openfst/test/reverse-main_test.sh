#!/bin/bash
# Unit test for fstreverse.

TEST_SRCDIR=.

set -eou pipefail

TST="$TEST_TMPDIR"
FST="$TEST_SRCDIR/openfst"

BIN="$FST/bin"
DAT="$FST/test/testdata/reverse"

source "$BIN"/setup.sh

"$BIN"/fstreverse "$DAT"/r1.fst "$TST"/r2.fst
"$BIN"/fstequal -v=1 "$DAT"/r2.fst "$TST"/r2.fst

# no superinitial state
"$BIN"/fstreverse --require_superinitial=false "$DAT"/r3.fst "$TST"/r4.fst
"$BIN"/fstequal -v=1 "$DAT"/r4.fst "$TST"/r4.fst

# to stdout
"$BIN"/fstreverse "$DAT"/r1.fst > "$TST"/r2.fst
"$BIN"/fstequal -v=1 "$DAT"/r2.fst "$TST"/r2.fst

# from stdin
"$BIN"/fstreverse - "$TST"/r2.fst < "$DAT"/r1.fst
"$BIN"/fstequal -v=1 "$DAT"/r2.fst "$TST"/r2.fst

# pipe
"$BIN"/fstreverse < "$DAT"/r1.fst > "$TST"/r2.fst
"$BIN"/fstequal -v=1 "$DAT"/r2.fst "$TST"/r2.fst
