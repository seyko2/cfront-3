#ident	"@(#)demangle:osrc/makefile	3.1" 
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

CC = cc
CFLAGS = -O
TARGETS = dem c++filt errfilt libmangle.a

all: $(TARGETS)

OFILES= nplist.o String.o demangle.o args.o strtol.o

libmangle.a: $(OFILES)
	rm -f libmangle.a
	ar cr libmangle.a `lorder $(OFILES) | tsort`
	if test "$(BSD)" ; then ranlib libmangle.a; fi

dem: dem.o libmangle.a
	$(CC) $(CFLAGS) -o dem dem.o libmangle.a

c++filt: c++filt.o libmangle.a
	$(CC) $(CFLAGS) -o c++filt c++filt.o libmangle.a

errfilt: errfilt.c
	$(CC) $(CFLAGS) -o errfilt errfilt.c

clean:
	rm -f *.o libmangle.a

clobber: clean
	rm -f $(TARGETS)

cpio: dem.cpio
	cat files | cpio -ocvu > dem.cpio
