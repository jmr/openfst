#!/bin/bash
# Unit test for fstconcat.

TEST_SRCDIR=.

set -eou pipefail

TST="$TEST_TMPDIR"
FST="$TEST_SRCDIR/openfst"

BIN="$FST/bin"
DAT="$FST/test/testdata/concat"

source "$BIN"/setup.sh

# file file -> file
"$BIN"/fstconcat "$DAT"/c1.fst "$DAT"/c2.fst "$TST"/c3.fst
"$BIN"/fstequal -v=1 "$DAT"/c3.fst "$TST"/c3.fst

/bin/rm "$TST"/c3.fst

# file stdin -> file
"$BIN"/fstconcat "$DAT"/c1.fst - "$TST"/c3.fst < "$DAT"/c2.fst
"$BIN"/fstequal -v=1 "$DAT"/c3.fst "$TST"/c3.fst

/bin/rm "$TST"/c3.fst

# file file -> stdout
"$BIN"/fstconcat "$DAT"/c1.fst "$DAT"/c2.fst > "$TST"/c3.fst
"$BIN"/fstequal -v=1 "$DAT"/c3.fst "$TST"/c3.fst

/bin/rm "$TST"/c3.fst
