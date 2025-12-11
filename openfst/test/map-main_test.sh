#!/bin/bash
# Unit test for fstmap.

TEST_SRCDIR=.

set -eou pipefail

TST="$TEST_TMPDIR"
FST="$TEST_SRCDIR/openfst"

BIN="$FST/bin"
ADAT="$FST/test/testdata/arc-map"
SDAT="$FST/test/testdata/state-map"

source "$BIN"/setup.sh

# Maps with arc_sum mapper.
"$BIN"/fstmap -map_type=arc_sum "$SDAT"/a1.fst "$TST"/a.fst
"$BIN"/fstequal -v=1 "$SDAT"/a2.fst "$TST"/a.fst

# Maps with float power mapper.

# Maps with input_epsilon mapper.
"$BIN"/fstmap -map_type=input_epsilon "$ADAT"/m1.fst "$TST"/m.fst
"$BIN"/fstequal -v=1 "$ADAT"/m13.fst "$TST"/m.fst

# Maps with invert mapper.
"$BIN"/fstmap -map_type=invert "$ADAT"/m1.fst "$TST"/m.fst
"$BIN"/fstequal -v=1 "$ADAT"/m7.fst "$TST"/m.fst
"$BIN"/fstmap -map_type=invert "$ADAT"/m7.fst "$TST"/m.fst
"$BIN"/fstequal -v=1 "$ADAT"/m1.fst "$TST"/m.fst

# Maps with output_epsilon mapper.
"$BIN"/fstmap -map_type=output_epsilon "$ADAT"/m1.fst "$TST"/m.fst
"$BIN"/fstequal -v=1 "$ADAT"/m14.fst "$TST"/m.fst

# Maps with power mapper.
"$BIN"/fstmap -map_type=power -power=.5 "$ADAT"/m1.fst "$TST"/m.fst
"$BIN"/fstequal -v=1 "$ADAT"/m15.fst "$TST"/m.fst
"$BIN"/fstmap -map_type=power -power=2 "$ADAT"/m1.fst "$TST"/m.fst
"$BIN"/fstequal -v=1 "$ADAT"/m16.fst "$TST"/m.fst

# Maps with plus mapper.
"$BIN"/fstmap -map_type=plus -weight=2 "$ADAT"/m1.fst "$TST"/m.fst
"$BIN"/fstequal -v=1 "$ADAT"/m9.fst "$TST"/m.fst

# Maps with quantize mapper.
"$BIN"/fstmap -map_type=quantize -delta=2 "$ADAT"/m1.fst "$TST"/m.fst
"$BIN"/fstequal -v=1 "$ADAT"/m8.fst "$TST"/m.fst
"$BIN"/fstmap -map_type=quantize -delta=2 "$ADAT"/m8.fst "$TST"/m.fst
"$BIN"/fstequal -v=1 "$ADAT"/m8.fst "$TST"/m.fst

# Maps with rmweight mapper.
"$BIN"/fstmap -map_type=rmweight "$ADAT"/m1.fst "$TST"/m.fst
"$BIN"/fstequal -v=1 "$ADAT"/m6.fst "$TST"/m.fst

# Maps with superfinal mapper.
"$BIN"/fstmap -map_type=superfinal "$ADAT"/m1.fst "$TST"/m.fst
"$BIN"/fstequal -v=1 "$ADAT"/m4.fst  "$TST"/m.fst

# Maps with times mapper.
"$BIN"/fstmap -map_type=times -weight=2 "$ADAT"/m1.fst "$TST"/m.fst
"$BIN"/fstequal -v=1 "$ADAT"/m10.fst "$TST"/m.fst

# Maps with to_log mapper.
"$BIN"/fstmap -map_type=to_log "$ADAT"/m1.fst "$TST"/m.fst # from std
"$BIN"/fstequal -v=1 "$ADAT"/m11.fst "$TST"/m.fst
"$BIN"/fstmap -map_type=to_log "$ADAT"/m12.fst "$TST"/m.fst # from log64
"$BIN"/fstequal -v=1 "$ADAT"/m11.fst "$TST"/m.fst

# Maps with to_log64 mapper.
"$BIN"/fstmap -map_type=to_log64 "$ADAT"/m1.fst "$TST"/m.fst # from std
"$BIN"/fstequal -v=1 "$ADAT"/m12.fst "$TST"/m.fst
"$BIN"/fstmap -map_type=to_log64 "$ADAT"/m11.fst "$TST"/m.fst # from log
"$BIN"/fstequal -v=1 "$ADAT"/m12.fst "$TST"/m.fst

# map with to_standard mapper.
"$BIN"/fstmap -map_type=to_standard "$ADAT"/m11.fst "$TST"/m.fst # from log
"$BIN"/fstequal -v=1 "$ADAT"/m1.fst "$TST"/m.fst
"$BIN"/fstmap -map_type=to_standard "$ADAT"/m12.fst "$TST"/m.fst # from log64
"$BIN"/fstequal -v=1 "$ADAT"/m1.fst "$TST"/m.fst

# Maps with identity mapper, to stdout.
"$BIN"/fstmap -map_type=identity "$ADAT"/m1.fst > "$TST"/m.fst
"$BIN"/fstequal -v=1 "$ADAT"/m1.fst "$TST"/m.fst

# Maps with identity mapper, stdin to stdout.
"$BIN"/fstmap -map_type=identity < "$ADAT"/m1.fst > "$TST"/m.fst
"$BIN"/fstequal -v=1 "$ADAT"/m1.fst "$TST"/m.fst
