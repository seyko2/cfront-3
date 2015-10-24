#ident	"@(#)Pool:makefile	3.1" 
###############################################################################
#
# C++ Standard Components, Release 3.0.
#
# Copyright (c) 1991, 1992 AT&T and Unix System Laboratories, Inc.
# Copyright (c) 1988, 1989, 1990 AT&T.  All Rights Reserved.
#
# THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T and Unix System
# Laboratories, Inc.  The copyright notice above does not evidence
# any actual or intended publication of such source code.
#
###############################################################################

CC=CC
SYS=
ROOT=../..
CCFLAGS = +w -O -I$(ROOT)/incl -DBUILDING_LIBRARY
BSD=

SRC = Pool.c Vpool.c

OBS = Pool.o Vpool.o 

libPool.a: $(OBS)
	ar cr libPool.a $(OBS)
	if test $(SYS) = BSD; then ranlib libPool.a; fi

Pool.o : Pool.c
	$(CC) $(CCFLAGS) -c Pool.c

Vpool.o : Vpool.c
	$(CC) $(CCFLAGS) -c Vpool.c

tests:

clean:
	rm -f *.o *.a

clobber: clean

