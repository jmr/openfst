#!/bin/bash
# Unit test for fstinfo.

TEST_SRCDIR=.

set -e pipefail

TST="$TEST_TMPDIR"
FST="$TEST_SRCDIR/openfst"

BIN="$FST/bin"
DAT="$FST/test/testdata/info"

source "$BIN"/setup.sh

"$BIN"/fstinfo -v=1 "$DAT"/i1.fst > "$TST"/i1.info
cmp "$DAT"/i1.info "$TST"/i1.info

"$BIN"/fstinfo -v=1 < "$DAT"/i1.fst > "$TST"/i1.info
cmp "$DAT"/i1.info "$TST"/i1.info
