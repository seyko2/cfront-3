#ident	"@(#)cls4:lib/complex/mk/makefile	1.3"
###############################################################################
#
# C++ source for the C++ Language System, Release 3.0.  This product
# is a new release of the original cfront developed in the computer
# science research center of AT&T Bell Laboratories.
#
# Copyright (c) 1993  UNIX System Laboratories, Inc.
# Copyright (c) 1991, 1992   AT&T and UNIX System Laboratories, Inc.
# Copyright (c) 1984, 1989, 1990 AT&T.  All Rights Reserved.
#
# THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE of AT&T and UNIX System
#	
# Laboratories, Inc.  The copyright notice above does not evidence
# any actual or intended publication of such source code.
#
###############################################################################
O_COMPLEX	=	abs.o arg.o cos.o error.o exp.o io.o log.o oper.o \
			polar.o pow.o sin.o sqrt.o
CC	=	../../../CC
CCFLAGS	= -DPRAGMATIC -I../../../incl

libcomplex.a:	$(O_COMPLEX)
	ar r libcomplex.a $(O_COMPLEX)
	if test "$(BSD)" ; then ranlib libcomplex.a; fi
	if test -f ../../../$@; then mv ../../../$@ ../../../$@.old; fi
	cp $@ ../../../

libcomplex_b.a:	$(O_COMPLEX)
	ar r libcomplex_b.a $(O_COMPLEX)
	if test "$(BSD)" ; then ranlib libcomplex_b.a; fi

libcomplex_eh.a:	$(O_COMPLEX)
	ar r libcomplex_eh.a $(O_COMPLEX)
	if test "$(BSD)" ; then ranlib libcomplex_eh.a; fi

libcomplex_p.a:	$(O_COMPLEX)
	ar r libcomplex_p.a $(O_COMPLEX)
	if test "$(BSD)" ; then ranlib libcomplex_p.a; fi

abs.o:	../complex/abs.C
	$(CC) $(CCFLAGS) -I../complex -c ../complex/abs.C

arg.o:	../complex/arg.C
	$(CC) $(CCFLAGS) -I../complex -c ../complex/arg.C

cos.o:	../complex/cos.C
	$(CC) $(CCFLAGS) -I../complex -c ../complex/cos.C

error.o:	../complex/error.C
	$(CC) $(CCFLAGS) -I../complex -c ../complex/error.C

exp.o:	../complex/exp.C
	$(CC) $(CCFLAGS) -I../complex -c ../complex/exp.C

io.o:	../complex/io.C
	$(CC) $(CCFLAGS) -I../complex -c ../complex/io.C

log.o:	../complex/log.C
	$(CC) $(CCFLAGS) -I../complex -c ../complex/log.C

oper.o:	../complex/oper.C
	$(CC) $(CCFLAGS) -I../complex -c ../complex/oper.C

polar.o:	../complex/polar.C
	$(CC) $(CCFLAGS) -I../complex -c ../complex/polar.C

pow.o:	../complex/pow.C
	$(CC) $(CCFLAGS) -I../complex -c ../complex/pow.C

sin.o:	../complex/sin.C
	$(CC) $(CCFLAGS) -I../complex -c ../complex/sin.C

sqrt.o:	../complex/sqrt.C
	$(CC) $(CCFLAGS) -I../complex -c ../complex/sqrt.C

clean:
	-@rm -f $(O_COMPLEX)
	-@rm libcomplex*.a