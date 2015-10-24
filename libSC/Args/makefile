#ident	"@(#)Args:makefile	3.1" 
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
CCFLAGS = +w $(O) -I$(ROOT)/incl -DBUILDING_LIBRARY
BSD=

SRC = Args.c Argsarg.c Argsflag.c Argsiter.c Argsobj.c Argsopt.c Argsset.c \
	Argssub.c Argsvalue.c

OBS = Args.o Argsarg.o Argsflag.o Argsiter.o Argsobj.o Argsopt.o Argsset.o \
	Argssub.o Argsvalue.o

libArgs.a: $(OBS)
	ar cr libArgs.a $(OBS)
	if test $(SYS) = BSD; then ranlib libArgs.a; fi

.c.o : 
	$(CC) $(CCFLAGS) -c $<

tests:

clean:
	rm -f *.o *.a

clobber: clean

