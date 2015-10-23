#!/bin/bash

rm LOG.01 LOG.02 LOG.03 cfront cfront.old libC.a munch 2> /dev/null
make -C src clean 2> /dev/null

rm scratch/lib/*..c 2> /dev/null
rm scratch/mnch/*..c 2> /dev/null
rm scratch/src/*..c  2> /dev/null
rm scratch/*..o scratch/libC.a  2> /dev/null
rm lib/mk/*.o 2> /dev/null
rm lib/complex/mk/*.o lib/complex/mk/*.a 2> /dev/null

D=demo/hello
rm $D/hello.i $D/hello..c $D/hello.o $D/hello..o 2> /dev/null
rm $D/hello.tmp 2> /dev/null
rm $D/hello.cdts..c $D/hello.cdts.o $D/a.out 2> /dev/null
rm $D/hello 2> /dev/null
rm $D/c++_c_output..c $D/c++_c_output_C $D/c++_c_output_c 2> /dev/null
