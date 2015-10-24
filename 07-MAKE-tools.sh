#!/bin/bash

make -C tools/demangler 2>&1 | tee LOG.07
[ $? != 0 ] && {
    echo "Error: failed to build c++filt"
    exit 1
}

make -C tools/pt 2>&1 | tee -a LOG.07
[ $? != 0 ] && {
    echo "Error: failed to build ptcomp/ptlink"
    exit 1
}
echo
echo "OK"
