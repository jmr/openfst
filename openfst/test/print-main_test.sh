#!/bin/bash
# Unit test for fstprint.

TEST_SRCDIR=.

set -eou pipefail

TST="$TEST_TMPDIR"
FST="$TEST_SRCDIR/openfst"

BIN="$FST/bin"
DAT="$FST/test/testdata/print"

source "$BIN"/setup.sh

## No symbol tables.

"$BIN"/fstprint "$DAT"/p1.fst "$TST"/p1-nosyms.txt
cmp "$DAT"/p1-nosyms.txt "$TST"/p1-nosyms.txt

"$BIN"/fstprint "$DAT"/p1.fst > "$TST"/p1-nosyms.txt
cmp "$DAT"/p1-nosyms.txt "$TST"/p1-nosyms.txt

"$BIN"/fstprint - "$TST"/p1-nosyms.txt < "$DAT"/p1.fst
cmp "$DAT"/p1-nosyms.txt "$TST"/p1-nosyms.txt

## With symbol tables.

declare -a SYMARGS
SYMARGS=( --isymbols=$DAT/p1.map --osymbols=$DAT/p1.map)

"$BIN"/fstprint "${SYMARGS[@]}" "$DAT"/p1.fst "$TST"/p1-syms.txt
cmp "$DAT"/p1-syms.txt "$TST"/p1-syms.txt

"$BIN"/fstprint "${SYMARGS[@]}" "$DAT"/p1.fst > "$TST"/p1-syms.txt
cmp "$DAT"/p1-syms.txt "$TST"/p1-syms.txt

"$BIN"/fstprint "${SYMARGS[@]}" - "$TST"/p1-syms.txt < "$DAT"/p1.fst
cmp "$DAT"/p1-syms.txt "$TST"/p1-syms.txt

## With an attached symbol table.

"$BIN"/fstprint "$DAT"/p2.fst "$TST"/p2.txt
cmp "$DAT"/p2.txt "$TST"/p2.txt

"$BIN"/fstprint "$DAT"/p2.fst > "$TST"/p2.txt
cmp "$DAT"/p2.txt "$TST"/p2.txt

"$BIN"/fstprint - "$TST"/p2.txt < "$DAT"/p2.fst
cmp "$DAT"/p2.txt "$TST"/p2.txt
