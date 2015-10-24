# /*ident	"@(#)cls4:tools/demangler/makefile	1.3" */

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
#  C++ Demangler Source Code
#
#BSD=1
CC = cc
CFLAGS = -O
TARGETS = c++filt

all: $(TARGETS)

OFILES= dem.o 

dem.o:	dem.c dem.h 
	$(CC) $(CFLAGS) -I. -c dem.c 

c++filt: dem.c dem.h
	$(CC) -DDEM_MAIN $(CFLAGS) -I. -o c++filt dem.c
	cp $@ ../../

clean:
	rm -f *.o 

clobber: clean
	rm -f $(TARGETS)
