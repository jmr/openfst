#!/bin/bash
# Unit test for fstcompile.

TEST_SRCDIR=.

set -eou pipefail

TST="$TEST_TMPDIR"
FST="$TEST_SRCDIR/openfst"

BIN="$FST/bin"
DAT="$FST/test/testdata/compile"

source "$BIN"/setup.sh

"$BIN"/fstcompile --isymbols="$DAT"/isyms.map --osymbols "$DAT"/osyms.map \
  --keep_isymbols --keep_osymbols "$DAT"/fst.txt "$TST"/fst.compiled

"$BIN"/fstequal -v=1 "$DAT"/fst.compiled "$TST"/fst.compiled

/bin/rm "$TST"/fst.compiled

# stdin -> stdout
"$BIN"/fstcompile --isymbols="$DAT"/isyms.map --osymbols "$DAT"/osyms.map \
  --keep_isymbols --keep_osymbols < "$DAT"/fst.txt > "$TST"/fst.compiled
"$BIN"/fstequal -v=1 "$DAT"/fst.compiled "$TST"/fst.compiled

/bin/rm "$TST"/fst.compiled

# file -> stdout
"$BIN"/fstcompile --isymbols="$DAT"/isyms.map --osymbols "$DAT"/osyms.map \
  --keep_isymbols --keep_osymbols "$DAT"/fst.txt > "$TST"/fst.compiled
"$BIN"/fstequal -v=1 "$DAT"/fst.compiled "$TST"/fst.compiled
