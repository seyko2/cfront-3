#ident	"@(#)fs:libsrc/makefile	3.1" 
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
CFLAGS=-I. -g
LDFLAGS=

# Don't include the instrumented _vec.c, because _vec_new only
# gets called for the cases of new X[...] where X has a constructor.
# In any other case of new ...[...], ::new gets called.
# And _vec_new calls ::new anyway.

OFILES=_new.o _delete.o reg.o map.o placenew.o

all:	libfs.a

libfs.a:$(OFILES)
	rm -f libfs.a
	ar r libfs.a $(OFILES)
	if test -x /usr/bin/ranlib ; then ranlib libfs.a; fi

clean:
	rm -f $(OFILES) libfs.a

