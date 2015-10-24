#ident	"@(#)Map:demos/makefile	3.1" 
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

all:
	$(SHELL) $(SLEBIN)/regress 

example.r:	example.E
	cat example.c | ./example.E >example.r

topsort.r:	topsort.E
	cat clothes | ./topsort.E >topsort.r

clean:
	rm -rf *.ii *.o *.r *.E core ptrepository .variant
