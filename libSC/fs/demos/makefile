#ident	"@(#)fs:demos/makefile	3.3" 
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
O=-g
CCFLAGS=-fs $(O)
LDFLAGS=$(O)
LIBS="-l++ -lfs"
FSIPP=fsipp

.SUFFIXES: .r .E

all:
	$(SHELL) $(SLEBIN)/regress

.c.r:
	$(CC) $(CCFLAGS) $(LDFLAGS) -o $*.E $< $(LIBS)
	@-./$*.E > $@ 2>&1

.c.E:
	$(CC) $(CCFLAGS) $(LDFLAGS) -o $*.E $< $(LIBS)

.c.o:
	$(CC) $(CCFLAGS) -c $<

clean:
	rm -rf *.ii *.o *.r *.E core ptrepository .variant

clobber: clean
