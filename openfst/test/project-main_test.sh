#!/bin/bash
# Unit test for fstproject.

TEST_SRCDIR=.

set -eou pipefail

TST="$TEST_TMPDIR"
FST="$TEST_SRCDIR/openfst"

BIN="$FST/bin"
DAT="$FST/test/testdata/project"

source "${BIN}/setup.sh"

# Project onto input.
"${BIN}/fstproject" --project_type=input "${DAT}/p1.fst" "${TST}/p2.fst"
"${BIN}/fstequal" -v=1 "${DAT}/p2.fst" "${TST}/p2.fst"

# Project onto output.
"${BIN}/fstproject" -project_type=output "${DAT}/p1.fst" "${TST}/p3.fst"
"${BIN}/fstequal" -v=1 "${DAT}/p3.fst" "${TST}/p3.fst"

# From stdin.
"${BIN}/fstproject" --project_type=output - "${TST}/p3.fst" < "${DAT}/p1.fst"
"${BIN}/fstequal" -v=1 "${DAT}/p3.fst" "${TST}/p3.fst"

# To stdout.
"${BIN}/fstproject" --project_type=output "${DAT}/p1.fst" > "${TST}/p3.fst"
"${BIN}/fstequal" -v=1 "${DAT}/p3.fst" "${TST}/p3.fst"

# From stdin to stdout
"${BIN}/fstproject" --project_type=output < "${DAT}/p1.fst" > "${TST}/p3.fst"
"${BIN}/fstequal" -v=1 "${DAT}/p3.fst" "${TST}/p3.fst"
