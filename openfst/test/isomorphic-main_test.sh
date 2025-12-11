#!/bin/bash
# Unit test for fstisomorphic.

TEST_SRCDIR=.

set -eou pipefail

TST="$TEST_TMPDIR"
FST="$TEST_SRCDIR/openfst"

BIN="$FST/bin"
DAT="$FST/test/testdata/isomorphic"

source "$BIN"/setup.sh

"$BIN"/fstisomorphic -v=1 "$DAT"/i1.fst "$DAT"/i2.fst
"$BIN"/fstisomorphic -v=1 - "$DAT"/i1.fst < "$DAT"/i2.fst
"$BIN"/fstisomorphic -v=1 "$DAT"/i1.fst - < "$DAT"/i2.fst
