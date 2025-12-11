#!/bin/bash
# Unit test for fstminimize.

TEST_SRCDIR=.

set -eou pipefail

TST="$TEST_TMPDIR"
FST="$TEST_SRCDIR/openfst"

BIN="$FST/bin"
DAT="$FST/test/testdata/minimize"

source "$BIN"/setup.sh

# acyclic test
"$BIN"/fstminimize "$DAT"/m1.fst "$TST"/acyclic_min.fst
"$BIN"/fstequal -v=1 "$DAT"/acyclic_min.fst "$TST"/acyclic_min.fst

# cyclic test
"$BIN"/fstminimize "$DAT"/m2.fst "$TST"/cyclic_min.fst
"$BIN"/fstequal -v=1 "$DAT"/cyclic_min.fst "$TST"/cyclic_min.fst

# same test, output to stdout
"$BIN"/fstminimize "$DAT"/m2.fst > "$TST"/cyclic_min.fst
"$BIN"/fstequal -v=1 "$DAT"/cyclic_min.fst "$TST"/cyclic_min.fst

# test pipeline-type usage
"$BIN"/fstminimize < "$DAT"/m2.fst > "$TST"/cyclic_min.fst
"$BIN"/fstequal -v=1 "$DAT"/cyclic_min.fst "$TST"/cyclic_min.fst

# weighted acyclic test
"$BIN"/fstminimize "$DAT"/m3.fst "$TST"/weighted_acyclic_min.fst
"$BIN"/fstequal -v=1 "$DAT"/weighted_acyclic_min.fst "$TST"/weighted_acyclic_min.fst

# weighted cyclic test
"$BIN"/fstminimize "$DAT"/m4.fst "$TST"/weighted_cyclic_min.fst
"$BIN"/fstequal -v=1 "$DAT"/weighted_cyclic_min.fst "$TST"/weighted_cyclic_min.fst

# transducer acyclic test (one output)
"$BIN"/fstminimize "$DAT"/m5.fst "$TST"/transducer_acyclic_min.fst
"$BIN"/fstequal -v=1 "$DAT"/transducer_acyclic_min.fst \
  "$TST"/transducer_acyclic_min.fst

# transducer acyclic test (two outputs)
"$BIN"/fstminimize "$DAT"/m5.fst "$TST"/transducer_acyclic_min1.fst \
  "$TST"/transducer_acyclic_min2.fst
"$BIN"/fstequal -v=1 "$DAT"/transducer_acyclic_min1.fst \
  "$TST"/transducer_acyclic_min1.fst
"$BIN"/fstequal -v=1 "$DAT"/transducer_acyclic_min2.fst \
  "$TST"/transducer_acyclic_min2.fst

# same test, but output one of the outputs to standard out
"$BIN"/fstminimize "$DAT"/m5.fst "$TST"/ta_min1.fst "-" > "$TST"/ta_min2.fst
"$BIN"/fstequal -v=1 "$DAT"/transducer_acyclic_min1.fst "$TST"/ta_min1.fst
"$BIN"/fstequal -v=1 "$DAT"/transducer_acyclic_min2.fst "$TST"/ta_min2.fst

# transducer cyclic test
"$BIN"/fstminimize "$DAT"/m6.fst "$TST"/transducer_cyclic_min.fst
"$BIN"/fstequal -v=1 "$DAT"/transducer_cyclic_min.fst \
  "$TST"/transducer_cyclic_min.fst
