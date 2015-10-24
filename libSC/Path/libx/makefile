#ident	"@(#)Path:libx/makefile	3.1" 
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


# These routines are stolen from libx.  You can find
# the entire libx library in the ksh or nmake source.
#
CC=cc
LDFLAGS=-g
CFLAGS=-g
OFILES=strmatch.o

all:	libx.a

t:	t.c
	cc -o t t.c libx.a

libx.a:	$(OFILES)
	/bin/rm -f $@
	ar rc $@ $(OFILES)
	ranlib $@

clean:
	rm -f $(OFILES)
