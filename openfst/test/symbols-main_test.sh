#!/bin/bash
# Unit test for fstsymbols.

TEST_SRCDIR=.

set -eou pipefail

TST="$TEST_TMPDIR"
FST="$TEST_SRCDIR/openfst"

BIN="$FST/bin"
DAT="$FST/test/testdata/symbols"

source "$BIN"/setup.sh

"$BIN"/fstsymbols --isymbols="$DAT"/syms.map --osymbols="$DAT"/syms.map \
  "$DAT"/s1.fst "$TST"/s2.fst
"$BIN"/fstequal -v=1 "$DAT"/s2.fst "$TST"/s2.fst

"$BIN"/fstsymbols --clear_isymbols --clear_osymbols "$TST"/s2.fst "$TST"/s1.fst
"$BIN"/fstequal -v=1 "$DAT"/s1.fst "$TST"/s1.fst

"$BIN"/fstsymbols --relabel_ipairs="$DAT"/relabel.pairs \
                --relabel_opairs="$DAT"/relabel.pairs \
                "$TST"/s2.fst "$TST"/s3.fst
"$BIN"/fstequal -v=1 "$DAT"/s3.fst "$TST"/s3.fst

"$BIN"/fstsymbols --clear_isymbols --clear_osymbols "$TST"/s3.fst "$TST"/s1.fst
"$BIN"/fstequal -v=1 "$DAT"/s1.fst "$TST"/s1.fst

"$BIN"/fstprint --save_isymbols="$TST"/relabel.map "$TST"/s3.fst /dev/null
diff -q "$DAT"/relabel.map "$TST"/relabel.map

"$BIN"/fstprint --save_osymbols="$TST"/relabel.map "$TST"/s3.fst /dev/null
diff -q "$DAT"/relabel.map "$TST"/relabel.map

# from stdin
"$BIN"/fstsymbols --isymbols="$DAT"/syms.map --osymbols="$DAT"/syms \
  - "$TST"/s2.fst < "$DAT"/s1.fst
"$BIN"/fstequal -v=1 "$DAT"/s2.fst "$TST"/s2.fst

# to stdout
"$BIN"/fstsymbols --isymbols="$DAT"/syms.map --osymbols="$DAT"/syms \
  "$DAT"/s1.fst > "$TST"/s2.fst
"$BIN"/fstequal -v=1 "$DAT"/s2.fst "$TST"/s2.fst

# pipe
"$BIN"/fstsymbols --isymbols="$DAT"/syms.map --osymbols="$DAT"/syms \
  < "$DAT"/s1.fst > "$TST"/s2.fst
"$BIN"/fstequal -v=1 "$DAT"/s2.fst "$TST"/s2.fst
