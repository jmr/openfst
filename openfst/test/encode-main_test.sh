#!/bin/bash

TEST_SRCDIR=.

set -eou pipefail

TST="$TEST_TMPDIR"
FST="$TEST_SRCDIR/openfst"

BIN="$FST/bin"
DAT="$FST/test/testdata/encode"

source "$FST"/bin/setup.sh

# Encode labels.
"$BIN"/fstencode -encode_labels $DAT/e1.fst "$TST"/codex  "$TST"/e1_c.fst
"$BIN"/fstencode -decode -encode_labels "$TST"/e1_c.fst "$TST"/codex "$TST"/e1_cd.fst
rm "$TST"/codex
"$BIN"/fstequal -v=1 $DAT/e1_cd.fst "$TST"/e1_cd.fst

# Encode weights.
"$BIN"/fstencode -encode_weights $DAT/e1.fst "$TST"/codex  "$TST"/e1_c.fst
"$BIN"/fstencode -decode -encode_weights "$TST"/e1_c.fst "$TST"/codex "$TST"/e1_cd.fst
rm "$TST"/codex
"$BIN"/fstequal -v=1 $DAT/e1_cd.fst "$TST"/e1_cd.fst

# Encode labels and weights.
"$BIN"/fstencode -encode_labels -encode_weights $DAT/e1.fst "$TST"/codex \
    "$TST"/e1_c.fst
"$BIN"/fstencode -decode -encode_labels -encode_weights "$TST"/e1_c.fst "$TST"/codex \
    "$TST"/e1_cd.fst
"$BIN"/fstequal -v=1 $DAT/e1_cd.fst "$TST"/e1_cd.fst

# We keep codex for next test.

# Encode labels and weights using pre-existing codex.
"$BIN"/fstencode -encode_labels -encode_weights -encode_reuse $DAT/e2.fst \
    "$TST"/codex "$TST"/e2_c.fst
"$BIN"/fstequal -v=1 $DAT/e2_c.fst "$TST"/e2_c.fst

# Encode labels using out.
"$BIN"/fstencode -encode_labels $DAT/e1.fst "$TST"/codex > "$TST"/e1_c.fst
"$BIN"/fstencode -decode -encode_labels "$TST"/e1_c.fst "$TST"/codex "$TST"/e1_cd.fst
"$BIN"/fstequal -v=1 $DAT/e1_cd.fst "$TST"/e1_cd.fst

echo "PASS"
