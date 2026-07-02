#!/bin/bash

# Crude testcase harness.  We just check that we have no errors, and
# that the expected number of warnings are actually diagnosed.  An
# expected warning is "WARN, " at each exepected warning spot in the
# source.

CC=$1
PLUGIN=$2
SRC=$3

function finish {
    rm -f $TMP
}
TMP=`mktemp`
trap finish EXIT

set -x
if ! $CC -fplugin=$PLUGIN -c $SRC -O2 2>&1 | tee $TMP; then
    exit $?
fi
set +x

nwarns=`grep "warning: " $TMP | wc -l`
expected_warns=`grep "WARN, " $SRC | wc -l`
if [ x$nwarns != x$expected_warns ]; then
    echo Number of warnings do not match for $PLUGIN.
    echo Exepected $expected_warns but got $nwarns.
    exit 1
fi
