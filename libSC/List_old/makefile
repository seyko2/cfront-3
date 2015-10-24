#ident	"@(#)List_old:makefile	3.1" 
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

CC = CC
SYS =
ROOT=../..
CCFLAGS =+w -I$(ROOT)/incl -DBUILDING_LIBRARY

SRC = List_old.c List_sort_o.c

OBS = List_old.o List_sort_o.o

libListold.a: $(OBS)
	rm -f libListold.a
	ar cr libListold.a $(OBS)
	if test "$(SYS)" = BSD; then ranlib libListold.a; fi

.c.o:
	$(CC) $(CCFLAGS) -c $<
	
clean:
	rm -f *.o *.a *.out core

tests:	always
	cd tests; $(MAKE) CC=$(CC)

clobber: clean

always:
