#ident	"@(#)Stopwatch:makefile	3.1" 
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
BSD=
ROOT=../..
CCFLAGS = +w -O -I$(ROOT)/incl -DBUILDING_LIBRARY

SRC = Stopwatch.c

OBS = Stopwatch.o 

libSwatch.a: $(OBS)
	ar cr libSwatch.a $(OBS)
	if test "$(SYS)" = BSD; then ranlib libSwatch.a; fi

Stopwatch.o : Stopwatch.c
	$(CC) $(CCFLAGS) -c Stopwatch.c

clean:
	rm -f *.o *.a

clobber: clean

