#ident	"@(#)G2++:g2++lib/demos/makefile	3.1" 
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

include $(SLELIB)/demorules

CC=CC
CCFLAGS=
LDFLAGS=
LIBS=../libg2++.a -l++
g2++comp=g2++comp

all: 	usr.o force
	$(SHELL) $(SLEBIN)/regress

usr.c: usr.g
	rm -f usr.h usr.c
	$(g2++comp) usr.g

usr.o:	usr.c
	$(CC) $(CCFLAGS) -c usr.c

.c.r:
	$(CC) $(CCFLAGS) $(LDFLAGS) -o $*.E $< usr.o $(LIBS)
	./$*.E <usr.data > $*.r

force:

clean: force
	rm -rf *.ii *.o *.r *.E core ptrepository usr.c usr.h .variant

clobber: clean
