#!/bin/bash
# fill a scratch directory with plain old C

[ ! -x cfront ] && {
    echo "Error: run 02-MAKE-cfront.sh first"
    exit 1
}

make -C scratch clean
make fillscratch 2>&1 | tee LOG.05
