#ident	"@(#)Set:makefile	3.1" 
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
ROOT=../..
O=-O
CCFLAGS= +w $(O) -I$(ROOT)/incl -DBUILDING_LIBRARY -DNDEBUG

OBJ = $(BAGOBJ) $(SETOBJ) $(SET_OF_POBJ)

SETOBJ = Set.o Set1.o Setio.o
SET_OF_POBJ = Set_of_p.o Set_of_p1.o Set_of_p2.o
BAGOBJ = Bag.o Bag1.o Bagio.o

libSet.a: $(OBJ)
	ar cr libSet.a $(OBJ)
	if test "$(SYS)" = BSD; then ranlib libSet.a; fi

Bag.o: Bag.c
	$(CC) -c $(CCFLAGS) Bag.c

Bagio.o: Bagio.c
	$(CC) -c $(CCFLAGS) Bagio.c

Bag1.o: Bag1.c
	$(CC) -c $(CCFLAGS) Bag1.c

Set.o: Set.c
	$(CC) -c $(CCFLAGS) Set.c

Setio.o: Setio.c
	$(CC) -c $(CCFLAGS) Setio.c

Set1.o: Set1.c
	$(CC) -c $(CCFLAGS) Set1.c

Set_of_p.o: Set_of_p.c
	$(CC) -c $(CCFLAGS) Set_of_p.c

Set_of_p1.o: Set_of_p1.c
	$(CC) -c $(CCFLAGS) Set_of_p1.c

Set_of_p2.o: Set_of_p2.c
	$(CC) -c $(CCFLAGS) Set_of_p2.c

clean:	force
	rm -f *.o core *.a

clobber:	clean

force:
