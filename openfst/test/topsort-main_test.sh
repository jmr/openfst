#!/bin/bash
# Unit test for fsttopsort.

TEST_SRCDIR=.

set -eou pipefail

TST="$TEST_TMPDIR"
FST="$TEST_SRCDIR/openfst"

BIN="$FST/bin"
DAT="$FST/test/testdata/topsort"

source "$BIN"/setup.sh

"$BIN"/fsttopsort "$DAT"/t1.fst "$TST"/t2.fst
"$BIN"/fstequal -v=1 "$DAT"/t2.fst "$TST"/t2.fst

# stdin
"$BIN"/fsttopsort - "$TST"/t2.fst < "$DAT"/t1.fst
"$BIN"/fstequal -v=1 "$DAT"/t2.fst "$TST"/t2.fst

# stdout
"$BIN"/fsttopsort "$DAT"/t1.fst > "$TST"/t2.fst
"$BIN"/fstequal -v=1 "$DAT"/t2.fst "$TST"/t2.fst

# pipe
"$BIN"/fsttopsort < "$DAT"/t1.fst > "$TST"/t2.fst
"$BIN"/fstequal -v=1 "$DAT"/t2.fst "$TST"/t2.fst
