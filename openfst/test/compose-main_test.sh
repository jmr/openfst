#!/bin/bash
# Unit test for fstcompose and two input FST argument file handling.

TEST_SRCDIR=.

set -eou pipefail

TST="$TEST_TMPDIR"
FST="$TEST_SRCDIR/openfst"

BIN="$FST/bin"
DAT="$FST/test/testdata/compose"

source "$BIN"/setup.sh

# file file file
"$BIN"/fstcompose "$DAT"/c1.fst "$DAT"/c2.fst "$TST"/c3.fst
"$BIN"/fstequal -v=1 "$DAT"/c3.fst "$TST"/c3.fst

# stdin("-") file file
"$BIN"/fstcompose - "$DAT"/c2.fst "$TST"/c3.fst <"$DAT"/c1.fst
"$BIN"/fstequal -v=1 "$DAT"/c3.fst "$TST"/c3.fst

# file stdin("-") file
"$BIN"/fstcompose "$DAT"/c1.fst - "$TST"/c3.fst <"$DAT"/c2.fst
"$BIN"/fstequal -v=1 "$DAT"/c3.fst "$TST"/c3.fst

# file file stdout
"$BIN"/fstcompose "$DAT"/c1.fst "$DAT"/c2.fst >"$TST"/c3.fst
"$BIN"/fstequal -v=1 "$DAT"/c3.fst "$TST"/c3.fst

# stdin("-") file stdout
"$BIN"/fstcompose - "$DAT"/c2.fst >"$TST"/c3.fst <"$DAT"/c1.fst
"$BIN"/fstequal -v=1 "$DAT"/c3.fst "$TST"/c3.fst

# file stdin("-") stdout
"$BIN"/fstcompose "$DAT"/c1.fst - >"$TST"/c3.fst <"$DAT"/c2.fst
"$BIN"/fstequal -v=1 "$DAT"/c3.fst "$TST"/c3.fst

# check that the connect and filter flag are obeyed
"$BIN"/fstcompose -connect=false --compose_filter=null \
  "$DAT"/c4.fst "$DAT"/c5.fst "$TST"/c7.fst
"$BIN"/fstequal -v=1 "$DAT"/c7.fst "$TST"/c7.fst

# check that all filters are equivalent in epsilon-free case
for f in alt_sequence match null trivial; do
  "$BIN"/fstcompose --compose_filter=$f "$DAT"/c1.fst "$DAT"/c2.fst "$TST"/c3.fst
  "$BIN"/fstequal -v=1 "$DAT"/c3.fst "$TST"/c3.fst
done
