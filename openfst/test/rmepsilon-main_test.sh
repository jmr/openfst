#!/bin/bash
# Unit test for fstrmepsilon.

TEST_SRCDIR=.

set -eou pipefail

TST="$TEST_TMPDIR"
FST="$TEST_SRCDIR/openfst"

BIN="$FST/bin"
DAT="$FST/test/testdata/rmepsilon"

source "$BIN"/setup.sh

"$BIN"/fstrmepsilon -connect=false "$DAT"/r1.fst "$TST"/r2.fst
"$BIN"/fstequal -v=1 "$DAT"/r2.fst "$TST"/r2.fst

# pruned test
"$BIN"/fstrmepsilon -weight=1 -nstate=10 "$DAT"/r1.fst "$TST"/r4.fst
"$BIN"/fstequal -v=1 "$DAT"/r4.fst "$TST"/r4.fst

# non-default delta test
"$BIN"/fstrmepsilon -delta=1 "$DAT"/cycle.fst "$TST"/cycle_delta_1.fst
"$BIN"/fstequal -v=1 "$DAT"/cycle_delta_1.fst "$TST"/cycle_delta_1.fst

# from stdin
"$BIN"/fstrmepsilon -connect=false - "$TST"/r2.fst < "$DAT"/r1.fst
"$BIN"/fstequal -v=1 "$DAT"/r2.fst "$TST"/r2.fst

# to stdout
"$BIN"/fstrmepsilon -connect=false "$DAT"/r1.fst > "$TST"/r2.fst
"$BIN"/fstequal -v=1 "$DAT"/r2.fst "$TST"/r2.fst

# pipeline
"$BIN"/fstrmepsilon -connect=false < "$DAT"/r1.fst > "$TST"/r2.fst
"$BIN"/fstequal -v=1 "$DAT"/r2.fst "$TST"/r2.fst
