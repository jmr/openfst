#!/bin/bash
# Unit test for fstshortestdistance.

TEST_SRCDIR=.

set -eou pipefail

TST="$TEST_TMPDIR"
FST="$TEST_SRCDIR/openfst"

BIN="$FST/bin"
DAT="$FST/test/testdata/shortest-distance"

source "$BIN"/setup.sh

"$BIN"/fstshortestdistance -reverse=false "$DAT"/sd1.fst "$TST"/sd1.dst
cmp "$DAT"/sd1.dst "$TST"/sd1.dst

"$BIN"/fstshortestdistance -reverse=true "$DAT"/sd1.fst "$TST"/sd1.rdst
cmp "$DAT"/sd1.rdst "$TST"/sd1.rdst

"$BIN"/fstshortestdistance -reverse=false "$DAT"/sd2.fst "$TST"/sd2.dst
cmp "$DAT"/sd2.dst "$TST"/sd2.dst

"$BIN"/fstshortestdistance -reverse=true "$DAT"/sd2.fst "$TST"/sd2.rdst
cmp "$DAT"/sd2.rdst "$TST"/sd2.rdst

for q in fifo lifo shortest state top
do
  "$BIN"/fstshortestdistance -queue_type=$q "$DAT"/sd2.fst "$TST"/sd2.dst
  cmp "$DAT"/sd2.dst "$TST"/sd2.dst
done

"$BIN"/fstshortestdistance --delta=1 "$DAT"/cycle.fst "$TST"/csd.txt
cmp "$DAT"/cycle_delta_1_shortest.txt "$TST"/csd.txt

# from stdin
"$BIN"/fstshortestdistance -reverse=true - "$TST"/sd2.rdst < "$DAT"/sd2.fst
cmp "$DAT"/sd2.rdst "$TST"/sd2.rdst

# to stdout
"$BIN"/fstshortestdistance -reverse=true "$DAT"/sd2.fst > "$TST"/sd2.rdst
cmp "$DAT"/sd2.rdst "$TST"/sd2.rdst

# pipe
"$BIN"/fstshortestdistance -reverse=true < "$DAT"/sd2.fst > "$TST"/sd2.rdst
cmp "$DAT"/sd2.rdst "$TST"/sd2.rdst
