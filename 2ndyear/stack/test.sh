#!/bin/bash
# tells a path to the default interpreter - bash

if [ "$#" == "0" ]
then
  echo "Did you forget to mention the binary to test?" >&2
  exit 1
fi

# $1: binary name to test

set -e  # abort on errors
set -x  # print each command before executing

# $1 means 'run argv[1] command'

# "|" means redirect output from "$1" to
# the following command (diff)

# diff A B - compares files "A" and "B"
# "-" means stdin / input stream

"$1" 1 2 3 4 5 | diff - 12345.out
"$1" 5 4 6 7 8 | diff - 54678.out
"$1" 53 42 67 79 81 | diff - 53_42_67_79_81.out

set +x  # stop printing commands before executing
echo "PASS"
