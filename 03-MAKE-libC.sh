#!/bin/bash
# make libC and munch

[ ! -x cfront ] && {
    echo "Error: run 02-MAKE-cfront.sh first"
    exit 1
}

rm munch 2> /dev/null
make 2>&1 | tee LOG.03

# compile a test program

D=demo/hello
U=../..

cd $D
$U/CC hello.C -o hello +i 
./hello
cd $U
