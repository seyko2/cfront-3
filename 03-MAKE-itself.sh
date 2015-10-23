#!/bin/bash
# bootstrap cfront by itself

[ ! -x cfront -o ! libC.a ] && {
    echo "Error: run 02-MAKE.sh first"
    exit 1
}

make -C src clean 2> /dev/null
make CXX=`pwd`/CC -C src	2>&1 | tee LOG.03

make -C src clean 2> /dev/null
make CXX=`pwd`/CC -C src	2>&1 | tee -a LOG.03

make -C src clean 2> /dev/null
make CXX=`pwd`/CC -C src	2>&1 | tee -a LOG.03

diff cfront cfront.old
[ $? != 0 ] && {
    echo "error: failed to bootstrap"
    exit 1
}
echo
echo "Succes !!"
exit 0
