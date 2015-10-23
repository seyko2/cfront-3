#!/bin/sh

D=../..

if [ ! -x ${D}/cfront ]; then
    echo "error: cfront compiler not found"
    exit
fi

${D}/CC c++_c_output.C -o c++_c_output_C +i
cc c++_c_output.c -o c++_c_output_c

echo
echo "Output of the c++ program (c++_c_output_C)"
./c++_c_output_C

echo
echo "Output of the C program (c++_c_output_c)"
./c++_c_output_c

echo
echo "Must be the same"
