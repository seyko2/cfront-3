#ident	"@(#)Block:makefile	3.1"
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

SRC = Vblock_dtor.c

OBS = Vblock_dtor.o

libBlock.a: $(OBS)
	ar cr libBlock.a $(OBS)
	if test $(SYS) = BSD; then ranlib libBlock.a; fi

Vblock_dtor.o : Vblock_dtor.c
	$(CC) $(CCFLAGS) -c Vblock_dtor.c

clean:
	rm -f *.o *.a

clobber: clean
