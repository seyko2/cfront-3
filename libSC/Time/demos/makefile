#ident	"@(#)Time:demos/makefile	3.1" 
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

CCFLAGS=-g
LIBS=-l++

.c.r:
	$(CC) $(CCFLAGS) $(LDFLAGS) -o $*.E $< $(LIBS)
	cd .; TZ=EST5EDT; export TZ; echo "12:42 noon foo" | ./$*.E > $@

all:
	$(SHELL) $(SLEBIN)/regress 

cal.r:	cal.E
	cd .; TZ=EST5EDT; export TZ; ./cal.E < appts >cal.r 2>&1

clean:
	rm -rf *.ii *.o *.r *.E core ptrepository .variant
