#ident	"@(#)cls4:demo/complex/makefile	1.1"
###############################################################################
#
# C++ source for the C++ Language System, Release 3.0.  This product
# is a new release of the original cfront developed in the computer
# science research center of AT&T Bell Laboratories.
#
# Copyright (c) 1991 AT&T and UNIX System Laboratories, Inc.
# Copyright (c) 1984, 1989, 1990 AT&T.  All Rights Reserved.
#
# THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE of AT&T and UNIX System
#	
# Laboratories, Inc.  The copyright notice above does not evidence
# any actual or intended publication of such source code.
#
###############################################################################
CCC	?=	../../CC
CCFLAGS=-O
L=../..
LINKOPT=-lcomplex -lm
LOPT="-L$(L)"


ALL = \
	abs.test.E \
	arg.test.E \
	conj.test.E \
	err.test.E \
	exp.test.E \
	imag.test.E \
	log.test.E \
	norm.test.E \
	polar.test.E \
	pow.test.E \
	real.test.E \
	sqrt.test.E 

OBJECTS = \
	abs.test.o \
	arg.test.o \
	conj.test.o \
	err.test.o \
	exp.test.o \
	imag.test.o \
	log.test.o \
	norm.test.o \
	polar.test.o \
	pow.test.o \
	real.test.o \
	sqrt.test.o 

NAMES = \
	abs.test \
	arg.test \
	conj.test \
	err.test \
	exp.test \
	imag.test \
	log.test \
	norm.test \
	polar.test \
	pow.test \
	real.test \
	sqrt.test 

run:	$(ALL)
	chmod +x runtests
	./runtests $(NAMES)

compile: $(ALL)

$(ALL): $(L)/libcomplex.a

democomplex: run

$(OBJECTS):

.SUFFIXES:	.C .E

.C.o:
	$(CCC) $(CCFLAGS) -c $<

.o.E:
	$(CCC) $(CCFLAGS) -o $@ $*.o $(LOPT) $(LINKOPT)

clean:
	rm -f $(ALL)
	rm -f $(OBJECTS)
	rm -f *.out
