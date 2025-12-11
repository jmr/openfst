#!/bin/bash
# Unit test for fstarcsort and one input FST argument file handling.

TEST_SRCDIR=.

set -eou pipefail

TST="$TEST_TMPDIR"
FST="$TEST_SRCDIR/openfst"

BIN="$FST/bin"
DAT="$FST/test/testdata/arcsort"

source "$BIN"/setup.sh

# input label sort test

# file file
"$BIN"/fstarcsort -sort_type=ilabel "$DAT"/a2.fst "$TST"/a1.fst
"$BIN"/fstequal -v=1 "$DAT"/a1.fst "$TST"/a1.fst

rm -f "$TST"/a1.fst

# stdin("-") file
"$BIN"/fstarcsort -sort_type=ilabel - "$TST"/a1.fst <"$DAT"/a2.fst
"$BIN"/fstequal -v=1 "$DAT"/a1.fst "$TST"/a1.fst

rm -f "$TST"/a1.fst

# file stdout
"$BIN"/fstarcsort -sort_type=ilabel "$DAT"/a2.fst >"$TST"/a1.fst
"$BIN"/fstequal -v=1 "$DAT"/a1.fst "$TST"/a1.fst

rm -f "$TST"/a1.fst

# # stdin("") stdout
"$BIN"/fstarcsort -sort_type=ilabel >"$TST"/a1.fst <"$DAT"/a2.fst
"$BIN"/fstequal -v=1 "$DAT"/a1.fst "$TST"/a1.fst

rm -f "$TST"/a1.fst

# stdin("-") stdout
"$BIN"/fstarcsort -sort_type=ilabel - >"$TST"/a1.fst <"$DAT"/a2.fst
"$BIN"/fstequal -v=1 "$DAT"/a1.fst "$TST"/a1.fst

rm -f "$TST"/a2.fst

# output label sort test
"$BIN"/fstarcsort -sort_type=olabel "$DAT"/a1.fst "$TST"/a2.fst
"$BIN"/fstequal -v=1 "$DAT"/a2.fst "$TST"/a2.fst
