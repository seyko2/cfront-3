#ident	"@(#)Map:makefile	3.1" 
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
CCFLAGS=-O -DBUILDING_LIBRARY +w
BSD=

comp=Map
SRC=Map.c
OBS=Map.o

lib$(comp).a:	$(OBS)
	ar cr lib$(comp).a $(OBS)
	if test "$(SYS)" = BSD; then ranlib lib$(comp).a; fi

Map.o:	Map.c
	$(CC) $(CCFLAGS) -c Map.c

clean:
	rm -f *.o *.a core
	
clobber:	clean

tests:	always
	cd tests; $(MAKE)

always:
