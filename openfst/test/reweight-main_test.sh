#!/bin/bash
# Unit test for fstreweight.

TEST_SRCDIR=.

set -eou pipefail

TST="$TEST_TMPDIR"
FST="$TEST_SRCDIR/openfst"

BIN="$FST/bin"
DAT="$FST/test/testdata/reweight"

source "$BIN"/setup.sh

"$BIN"/fstreweight -reweight_type=to_initial "$DAT"/r1.fst "$DAT"/r1.pot "$TST"/r2.fst
"$BIN"/fstequal -v=1 "$DAT"/r2.fst "$TST"/r2.fst

"$BIN"/fstreweight -reweight_type=to_final "$DAT"/r1.fst "$DAT"/r1.pot "$TST"/r3.fst
"$BIN"/fstequal -v=1 "$DAT"/r3.fst "$TST"/r3.fst

# to stdout
"$BIN"/fstreweight -reweight_type=to_final "$DAT"/r1.fst "$DAT"/r1.pot > "$TST"/r3.fst
"$BIN"/fstequal -v=1 "$DAT"/r3.fst "$TST"/r3.fst
