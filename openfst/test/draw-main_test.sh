#!/bin/bash
# Unit test for fstdraw.

TEST_SRCDIR=.

set -eou pipefail

TST="$TEST_TMPDIR"
FST="$TEST_SRCDIR/openfst"

BIN="$FST/bin"
DAT="$FST/test/testdata/draw"

source "$BIN"/setup.sh

# From file.
"$BIN"/fstdraw --precision=2 "$DAT"/draw.fst "$TST"/draw-2p.dot
cmp "$DAT"/draw-2p.dot "$TST"/draw-2p.dot
"$BIN"/fstdraw --precision=5 "$DAT"/draw.fst "$TST"/draw-5p.dot
cmp "$DAT"/draw-5p.dot "$TST"/draw-5p.dot
/bin/rm -f "$TST"/draw-[25]p.dot

# To stdout.
"$BIN"/fstdraw --precision=2 "$DAT"/draw.fst > "$TST"/draw-2p.dot
cmp "$DAT"/draw-2p.dot "$TST"/draw-2p.dot
"$BIN"/fstdraw --precision=5 "$DAT"/draw.fst > "$TST"/draw-5p.dot
cmp "$DAT"/draw-5p.dot "$TST"/draw-5p.dot
/bin/rm -f "$TST"/draw-[25]p.dot

# From stdin to stdout.
"$BIN"/fstdraw --precision=2 < "$DAT"/draw.fst > "$TST"/draw-2p.dot
cmp "$DAT"/draw-2p.dot "$TST"/draw-2p.dot
"$BIN"/fstdraw --precision=5 < "$DAT"/draw.fst > "$TST"/draw-5p.dot
cmp "$DAT"/draw-5p.dot "$TST"/draw-5p.dot
