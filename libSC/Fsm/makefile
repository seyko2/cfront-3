#ident	"@(#)Fsm:makefile	3.1" 
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

#Simple Fsm Library: 
#Version L3

CC = 
SYS =
ROOT=../..
O=-O
CCFLAGS=+w $(O) -I$(ROOT)/incl -DBUILDING_LIBRARY

OBS = Fsm.o Chararray.o Fprint.o

libFsm.a: $(OBS)
	ar cr libFsm.a $(OBS)
	if test "$(SYS)" = BSD ; then ranlib libFsm.a; fi

Fsm.o: Fsm.c
	$(CC) -c $(CCFLAGS)  Fsm.c

Chararray.o: Chararray.c
	$(CC) -c $(CCFLAGS) Chararray.c

Fprint.o: Fprint.c
	$(CC) -c $(CCFLAGS) Fprint.c

clean:
	rm -f *.o libFsm.a core

clobber: clean
