#!/bin/bash

rm LOG.01 LOG.02 LOG.03 cfront cfront.old libC.a munch 2> /dev/null
make -C src clean 2> /dev/null

rm scratch/lib/*..c 2> /dev/null
rm scratch/mnch/*..c 2> /dev/null
rm scratch/src/*..c  2> /dev/null
rm scratch/*..o scratch/libC.a  2> /dev/null
rm lib/mk/*.o 2> /dev/null
rm lib/complex/mk/*.o lib/complex/mk/*.a 2> /dev/null

rm test/hello.i test/hello..c test/hello.o test/hello..o 2> /dev/null
rm test/hello.tmp 2> /dev/null
rm test/hello.cdts..c test/hello.cdts.o test/a.out 2> /dev/null
rm test/hello 2> /dev/null
