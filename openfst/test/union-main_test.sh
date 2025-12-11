#!/bin/bash
# Unit test for fstunion.

TEST_SRCDIR=.

set -eou pipefail

TST="$TEST_TMPDIR"
FST="$TEST_SRCDIR/openfst"

BIN="$FST/bin"
DAT="$FST/test/testdata/union"

source "$BIN"/setup.sh

"$BIN"/fstunion "$DAT"/u1.fst "$DAT"/u2.fst "$TST"/u3.fst
"$BIN"/fstequal -v=1 "$DAT"/u3.fst "$TST"/u3.fst

# stdin 1
"$BIN"/fstunion - "$DAT"/u2.fst "$TST"/u3.fst < "$DAT"/u1.fst
"$BIN"/fstequal -v=1 "$DAT"/u3.fst "$TST"/u3.fst

# stdin 2
"$BIN"/fstunion "$DAT"/u1.fst - "$TST"/u3.fst < "$DAT"/u2.fst
"$BIN"/fstequal -v=1 "$DAT"/u3.fst "$TST"/u3.fst

# stdout
"$BIN"/fstunion "$DAT"/u1.fst "$DAT"/u2.fst > "$TST"/u3.fst
"$BIN"/fstequal -v=1 "$DAT"/u3.fst "$TST"/u3.fst

# pipeline
"$BIN"/fstunion - "$DAT"/u2.fst < "$DAT"/u1.fst > "$TST"/u3.fst
"$BIN"/fstequal -v=1 "$DAT"/u3.fst "$TST"/u3.fst
