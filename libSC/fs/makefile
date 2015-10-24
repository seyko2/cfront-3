#ident	"@(#)fs:makefile	3.1" 
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

#############################################################
# You shouldn't have to change anything below this line.
#############################################################
O=-O
CCFLAGS=$(O)
LDFLAGS=$(O)
LIBS=-l++

all:	libfs.a fsipp

libfs.a:force
	cd libsrc; $(MAKE) CC=$(CC) CFLAGS="$(CCFLAGS)" LDFLAGS="$(LDFLAGS)"
	rm -f libfs.a; ln libsrc/libfs.a .

fsipp:	force
	cd fsippsrc; $(MAKE) CC=$(CC) CFLAGS="$(CCFLAGS)" LDFLAGS="$(LDFLAGS)" LIBS="$(LIBS)"
	rm -f lib/fsipp; strip fsippsrc/fsipp; ln fsippsrc/fsipp lib

tests:	force
	cd tests; $(MAKE)

clean:	force
	cd libsrc; $(MAKE) clean
	cd fsippsrc; $(MAKE) clean
	rm -f *.a fsippsrc/fsipp lib/fsipp

clobber:force
	cd libsrc; $(MAKE) clobber
	cd fsippsrc; $(MAKE) clobber
	rm -f *.a lib/fsipp

force:

