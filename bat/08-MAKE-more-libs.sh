#!/bin/bash
# make libcomplex.a, ...

[ ! -x cfront ] && {
    echo "Error: run 02-MAKE-cfront.sh first"
    exit 1
}

make -C lib/complex/mk
[ ! -f libcomplex.a ] && {
    echo
    echo "Error: failed to prepare libcomplex.a"
    exit 1
}
echo
echo "OK"
exit 0
