#!/bin/bash
# Unit test for fstsynchronize.

TEST_SRCDIR=.

set -eou pipefail

TST="$TEST_TMPDIR"
FST="$TEST_SRCDIR/openfst"

BIN="$FST/bin"
DAT="$FST/test/testdata/synchronize"

source "$BIN"/setup.sh

"$BIN"/fstsynchronize "$DAT"/s1.fst "$TST"/s2.fst
"$BIN"/fstequal -v=1 "$DAT"/s2.fst "$TST"/s2.fst

# stdin
"$BIN"/fstsynchronize - "$TST"/s2.fst < "$DAT"/s1.fst
"$BIN"/fstequal -v=1 "$DAT"/s2.fst "$TST"/s2.fst

# stdout
"$BIN"/fstsynchronize "$DAT"/s1.fst > "$TST"/s2.fst
"$BIN"/fstequal -v=1 "$DAT"/s2.fst "$TST"/s2.fst

# pipe
"$BIN"/fstsynchronize < "$DAT"/s1.fst > "$TST"/s2.fst
"$BIN"/fstequal -v=1 "$DAT"/s2.fst "$TST"/s2.fst
