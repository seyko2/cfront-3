#!/bin/bash

[ ! -x cfront ] && {
    echo "Error: run 01-MAKE-PRE-CFRONT.sh first"
    exit 1
}

make fillscratch 2>&1 | tee LOG.02
