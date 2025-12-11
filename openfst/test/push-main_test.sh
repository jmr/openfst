#!/bin/bash
# Unit test for fstpush.

TEST_SRCDIR=.

set -eou pipefail

TST="$TEST_TMPDIR"
FST="$TEST_SRCDIR/openfst"

BIN="$FST/bin"
DAT="$FST/test/testdata/push"

source "$BIN/setup.sh"

for i in 1 2; do
  if [[ $i -eq 2 ]]; then
    R=r
    RTW=--remove_total_weight
    RCA=--remove_common_affix
  else
    R=
    RTW=
    RCA=
  fi

  "$BIN"/fstpush -push_weights -reweight_type=to_initial $RTW "$DAT"/p1.fst "$TST"/p2$R.fst
  "$BIN"/fstequal -v=1 "$DAT"/p2$R.fst "$TST"/p2$R.fst

  "$BIN"/fstpush -push_weights -reweight_type=to_final $RTW "$DAT"/p1.fst "$TST"/p3$R.fst
  "$BIN"/fstequal -v=1 "$DAT"/p3$R.fst "$TST"/p3$R.fst

  "$BIN"/fstpush -push_labels -reweight_type=to_initial $RCA "$DAT"/p1.fst "$TST"/p4$R.fst
  "$BIN"/fstequal -v=1 "$DAT"/p4$R.fst "$TST"/p4$R.fst

  "$BIN"/fstpush -push_labels -reweight_type=to_final $RCA "$DAT"/p1.fst "$TST"/p5$R.fst
  "$BIN"/fstequal -v=1 "$DAT"/p5$R.fst "$TST"/p5$R.fst

done

"$BIN"/fstpush -push_weights -delta=100 \
  "$DAT"/delta_test.fst "$TST"/delta_test_100.fst
"$BIN"/fstequal "$DAT"/delta_test_100.fst "$TST"/delta_test_100.fst

"$BIN"/fstpush -push_weights -delta=1 \
  "$DAT"/delta_test.fst "$TST"/delta_test_1.fst
"$BIN"/fstequal "$DAT"/delta_test_1.fst "$TST"/delta_test_1.fst

# from stdin
"$BIN"/fstpush -push_weights -delta=1 \
  - "$TST"/delta_test_1.fst < "$DAT"/delta_test.fst
"$BIN"/fstequal "$DAT"/delta_test_1.fst "$TST"/delta_test_1.fst

# to stdout
"$BIN"/fstpush -push_weights -delta=1 \
  "$DAT"/delta_test.fst > "$TST"/delta_test_1.fst
"$BIN"/fstequal "$DAT"/delta_test_1.fst "$TST"/delta_test_1.fst

# stdin to stdout
"$BIN"/fstpush -push_weights -delta=1 \
  < "$DAT"/delta_test.fst > "$TST"/delta_test_1.fst
"$BIN"/fstequal "$DAT"/delta_test_1.fst "$TST"/delta_test_1.fst
