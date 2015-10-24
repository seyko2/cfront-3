#ident	"@(#)List:makefile	3.2" 
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
CCFLAGS = +w -I$(ROOT)/incl -DBUILDING_LIBRARY

SRC = List.c List_sort.c List_voidP.c

OBS = List.o List_sort.o List_voidP.o

libList.a: $(OBS)
	rm -f libList.a
	ar cr libList.a $(OBS)
	if test "$(SYS)" = BSD; then ranlib libList.a; fi

.c.o:
	$(CC) $(CCFLAGS) -c $<
	
clean:
	rm -f *.o *.a *.out core

tests:	always
	cd tests; $(MAKE) CC=$(CC)

clobber: clean

always:
