#!/bin/bash
# Unit test for fstshortestpath.sh

TEST_SRCDIR=.

set -eou pipefail

TST="$TEST_TMPDIR"
FST="$TEST_SRCDIR/openfst"

BIN="$FST/bin"
DAT="$FST/test/testdata/shortest-path"

source "$BIN"/setup.sh

"$BIN"/fstshortestpath "$DAT"/sp1.fst "$TST"/sp2.fst
"$BIN"/fstequal -v=1 "$DAT"/sp2.fst "$TST"/sp2.fst

"$BIN"/fstshortestpath -nshortest=4 "$DAT"/sp1.fst "$TST"/sp3.fst
"$BIN"/fstequal -v=1 "$DAT"/sp3.fst "$TST"/sp3.fst

"$BIN"/fstshortestpath -unique -nshortest=4 "$DAT"/sp1.fst "$TST"/sp4.fst
"$BIN"/fstequal -v=1 "$DAT"/sp4.fst "$TST"/sp4.fst

"$BIN"/fstshortestpath -nshortest=4 -weight=1.0 -nstate=20 "$DAT"/sp5.fst \
  "$TST"/sp9.fst
"$BIN"/fstequal -v=1 "$DAT"/sp9.fst "$TST"/sp9.fst

for q in fifo lifo shortest state top
do
  "$BIN"/fstshortestpath -queue_type=$q "$DAT"/sp5.fst "$TST"/sp6.fst
  "$BIN"/fstequal -v=1 "$DAT"/sp6.fst "$TST"/sp6.fst
 done

# from stdin
"$BIN"/fstshortestpath - "$TST"/sp2.fst < "$DAT"/sp1.fst
"$BIN"/fstequal -v=1 "$DAT"/sp2.fst "$TST"/sp2.fst

# to stdout
"$BIN"/fstshortestpath "$DAT"/sp1.fst > "$TST"/sp2.fst
"$BIN"/fstequal -v=1 "$DAT"/sp2.fst "$TST"/sp2.fst

# pipe
"$BIN"/fstshortestpath < "$DAT"/sp1.fst > "$TST"/sp2.fst
"$BIN"/fstequal -v=1 "$DAT"/sp2.fst "$TST"/sp2.fst
