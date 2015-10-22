#!/bin/bash

rm LOG.01 LOG.02 LOG.03 cfront cfront-pre 2> /dev/null
make -C src clean 2> /dev/null

rm scratch/lib/*..c 2> /dev/null
rm scratch/mnch/*..c 2> /dev/null
rm scratch/src/*..c  2> /dev/null
rm scratch/*..o scratch/libC.a  2> /dev/null
rm lib/mk/*.o 2> /dev/null
