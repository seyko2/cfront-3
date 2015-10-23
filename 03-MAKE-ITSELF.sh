#!/bin/bash
# bootstrap cfront by itself

[ ! -x cfront -o ! libC.a ] && {
    echo "Error: run 02-MAKE.sh first"
    exit 1
}

make -C src clean 2> /dev/null
make CXX=`pwd`/CC -C src

make -C src clean 2> /dev/null
make CXX=`pwd`/CC -C src

make -C src clean 2> /dev/null
make CXX=`pwd`/CC -C src

diff cfront cfront.old
[ $? != 0 ] && {
    echo "error: failed to bootstrap"
    exit 1
}
echo
echo "Succes !!"
exit 0
