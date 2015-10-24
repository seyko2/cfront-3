#ident	"@(#)G2++:compsrc/demos/makefile	3.2" 
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

CC=
CCFLAGS=
LDFLAGS=
LIBS=-l++
g2++comp=g2++comp

all: 	test1.o force
	$(SHELL) $(SLEBIN)/regress

test1.c: test1.g
	rm -f test1.h test1.c
	$(g2++comp) test1.g

test1.o:	test1.c
	$(CC) $(CCFLAGS) -c test1.c

.c.r:
	$(CC) $(CCFLAGS) $(LDFLAGS) -o $*.E $< test1.o $(LIBS)
	./$*.E > $*.r

force:

clean: force
	rm -rf *.ii *.o *.r *.E ptrepository core test1.c test1.h .variant

clobber: clean

