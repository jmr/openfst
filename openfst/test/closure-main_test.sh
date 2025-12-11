#!/bin/bash
# Unit test for fstclosure.

TEST_SRCDIR=.

set -eou pipefail

TST="$TEST_TMPDIR"
FST="$TEST_SRCDIR/openfst"

BIN="$FST/bin"
DAT="$FST/test/testdata/closure"

source "$BIN"/setup.sh

# A*
"$BIN"/fstclosure "$DAT"/c1.fst "$TST"/c2.fst
"$BIN"/fstequal -v=1 "$DAT"/c2.fst "$TST"/c2.fst

# A+
"$BIN"/fstclosure -closure_type=plus "$DAT"/c1.fst "$TST"/c3.fst
"$BIN"/fstequal -v=1 "$DAT"/c3.fst "$TST"/c3.fst

/bin/rm "$TST"/c3.fst

# stdin -> file
"$BIN"/fstclosure -closure_type=plus - "$TST"/c3.fst < "$DAT"/c1.fst
"$BIN"/fstequal -v=1 "$DAT"/c3.fst "$TST"/c3.fst

/bin/rm "$TST"/c3.fst

# file -> stdout
"$BIN"/fstclosure -closure_type=plus "$DAT"/c1.fst > "$TST"/c3.fst
"$BIN"/fstequal -v=1 "$DAT"/c3.fst "$TST"/c3.fst

/bin/rm "$TST"/c3.fst

# stdin -> stdout
"$BIN"/fstclosure -closure_type=plus < "$DAT"/c1.fst > "$TST"/c3.fst
"$BIN"/fstequal -v=1 "$DAT"/c3.fst "$TST"/c3.fst
