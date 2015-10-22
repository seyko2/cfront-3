#!/bin/bash

[ ! -x cfront ] && {
    echo "Error: run 02-MAKE.sh first"
    exit 1
}

rm munch 2> /dev/null
make 2>&1 | tee LOG.03

# compile a test program
cd test
../CC hello.C -o hello +i 
./hello
cd ..
