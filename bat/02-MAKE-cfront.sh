#!/bin/bash

[ ! -d incl ] && {
    echo "Error: run 01-MAKE-incl.sh first"
    exit 1
}

make -C src 2>&1 | tee LOG.02
[ $? != 0 ] && {
    echo "Error!"
    exit 1
}
echo OK
