#!/bin/bash

rm LOG.0? 2> /dev/null
rm cfront cfront.old munch munch.old libC.a libC.a.old 2> /dev/null

[ -L incl ] && rm incl
make -C src clean 2> /dev/null
make -C scratch clean 2> /dev/null

rm lib/mk/*.o lib/mk/*..c 2> /dev/null
rm lib/complex/mk/*.o lib/complex/mk/*.a lib/complex/mk/*..c 2> /dev/null

D=demo/hello
rm $D/hello.i $D/hello..c $D/hello.o $D/hello..o 2> /dev/null
rm $D/hello.tmp 2> /dev/null
rm $D/hello.cdts..c $D/hello.cdts.o $D/a.out 2> /dev/null
rm $D/hello 2> /dev/null
rm $D/c++_c_output..c $D/c++_c_output_C $D/c++_c_output_c 2> /dev/null

make -C tools/demangler clobber  2> /dev/null
make -C tools/pt clobber  2> /dev/null
rm ptcomp ptlink c++filt  2> /dev/null

make -C lib/complex/mk clean  2> /dev/null
rm libcomplex*.a  2> /dev/null

for i in *.sh; do
    [ -L $i ] && rm $i
done

[ -L CC ] && {
    rm CC
    ln -s CC3 CC
}
