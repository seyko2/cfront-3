# /*ident	"@(#)cls4:tools/pt/makefile	1.3" */

###############################################################################
#
# C++ source for the C++ Language System, Release 3.0.  This product
# is a new release of the original cfront developed in the computer
# science research center of AT&T Bell Laboratories.
#
# Copyright (c) 1993  UNIX System Laboratories, Inc.
# Copyright (c) 1991, 1992  AT&T and UNIX System Laboratories, Inc.
# Copyright (c) 1984, 1989, 1990 AT&T.  All Rights Reserved.
#
# THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE of AT&T and UNIX System
#	
# Laboratories, Inc.  The copyright notice above does not evidence
# any actual or intended publication of such source code.
#
###############################################################################

#
#  C++ PT Instantation Source Code
#
#BSD=1
CC = cc
CFLAGS = -O
TARGETS = ptcomp ptlink

all: $(TARGETS)

ptcomp: ptcomp.o ptutil.o
	$(CC) $(CFLAGS) -o ptcomp ptcomp.o ptutil.o

ptlink: ptlink.o ptutil.o ../demangler/dem.o
	$(CC) $(CFLAGS) -o ptlink ptlink.o ptutil.o dem.o

ptutil.o:	ptutil.c pt.h
	$(CC) $(CFLAGS) -c ptutil.c

ptcomp.o:	ptcomp.c pt.h
	$(CC) $(CFLAGS) -c ptcomp.c

ptlink.o:	ptlink.c pt.h ../demangler/dem.h
	$(CC) $(CFLAGS) -I../demangler -c ptlink.c

clean:
	rm -f *.o 

clobber: clean
	rm -f $(TARGETS)
