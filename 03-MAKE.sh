#!/bin/bash

[ ! -x cfront ] && {
    echo "Error: run 02-MAKE.sh first"
    exit 1
}
mv cfront cfront-pre

# scratchCC=tcc
scratchCC=gcc

make scratchCC=${scratchCC} scratch  2>&1 | tee LOG.03

#rm munch
#make 2>&1 | tee LOG.03
