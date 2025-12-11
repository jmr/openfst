#!/bin/bash
# Unit test for fstconnect.

TEST_SRCDIR=.

set -eou pipefail

TST="$TEST_TMPDIR"
FST="$TEST_SRCDIR/openfst"

BIN="$FST/bin"
DAT="$FST/test/testdata/connect"

source "$BIN"/setup.sh

# file -> file
"$BIN"/fstconnect "$DAT"/c1.fst "$TST"/c2.fst
"$BIN"/fstequal -v=1 "$DAT"/c2.fst "$TST"/c2.fst
/bin/rm "$TST"/c2.fst

# stdin -> file
"$BIN"/fstconnect - "$TST"/c2.fst < "$DAT"/c1.fst
"$BIN"/fstequal -v=1 "$DAT"/c2.fst "$TST"/c2.fst
/bin/rm "$TST"/c2.fst

# file -> stdout
"$BIN"/fstconnect "$DAT"/c1.fst > "$TST"/c2.fst
"$BIN"/fstequal -v=1 "$DAT"/c2.fst "$TST"/c2.fst
/bin/rm "$TST"/c2.fst

# stdin -> stdout
"$BIN"/fstconnect < "$DAT"/c1.fst > "$TST"/c2.fst
"$BIN"/fstequal -v=1 "$DAT"/c2.fst "$TST"/c2.fst
