#!/bin/bash
# Unit test for fstepsnormalize.

TEST_SRCDIR=.

set -eou pipefail

TST="$TEST_TMPDIR"
FST="$TEST_SRCDIR/openfst"

BIN="$FST/bin"
DAT="$FST/test/testdata/epsnormalize"

source "$BIN"/setup.sh

# normalize input
"$BIN"/fstepsnormalize "$DAT"/e1.fst "$TST"/e2.fst
"$BIN"/fstequal -v=1 "$DAT"/e2.fst "$TST"/e2.fst

# normalize output
"$BIN"/fstepsnormalize -eps_norm_type=output "$DAT"/e3.fst "$TST"/e4.fst
"$BIN"/fstequal -v=1 "$DAT"/e4.fst "$TST"/e4.fst

/bin/rm "$TST"/e2.fst

# normalize input -> stdout
"$BIN"/fstepsnormalize "$DAT"/e1.fst > "$TST"/e2.fst
"$BIN"/fstequal -v=1 "$DAT"/e2.fst "$TST"/e2.fst

/bin/rm "$TST"/e2.fst

# normalize input -> stdin, stdout
"$BIN"/fstepsnormalize < "$DAT"/e1.fst > "$TST"/e2.fst
"$BIN"/fstequal -v=1 "$DAT"/e2.fst "$TST"/e2.fst

# normalize input, non functional case
"$BIN"/fstepsnormalize "$DAT"/e5.fst "$TST"/e6.fst
"$BIN"/fstequal -v=1 "$DAT"/e6.fst "$TST"/e6.fst
