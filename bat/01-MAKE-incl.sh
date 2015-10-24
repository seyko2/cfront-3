#!/bin/bash
# generate (configure) incl directory

# currently this step is dummy: include headers
# are a mix from the cfront-1 and plan9 port
# headers

[ -L incl ] && rm incl
ln -s incl-master/incl-linux32 incl
echo OK
