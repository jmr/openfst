#!/bin/bash
# Unit test for fstequivalent.

TEST_SRCDIR=.

set -eou pipefail

TST="$TEST_TMPDIR"
FST="$TEST_SRCDIR/openfst"

BIN="$FST/bin"
DAT="$FST/test/testdata/equivalent"

source "$BIN"/setup.sh

# we assume that the basic workings of equivalent are tested; here
# we're just checking that command line arguments are passed correctly.

"$BIN"/fstequivalent "$DAT"/e1.fst "$DAT"/e2.fst
"$BIN"/fstequivalent - "$DAT"/e1.fst < "$DAT"/e2.fst
"$BIN"/fstequivalent "$DAT"/e1.fst - < "$DAT"/e2.fst
