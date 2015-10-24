#!/bin/bash
# build libCC.a, munch and cfront from a scratch directory

[ ! -x cfront ] && {
    echo "Error: run 02-MAKE-cfront.sh first"
    exit 1
}

make -C scratch 2>&1 | tee LOG.06
[ $? != 0 ] && {
    echo "Error: failed to build cfront from a scratch directory"
    exit 1
}
echo
echo "OK"
echo "You can try to repeat steps 4, 5, 6"
