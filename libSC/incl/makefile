#ident	"@(#)incl:makefile	3.1" 
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

###############################################################################
# You shouldn't have to change anything below this line.
###############################################################################
BIN=/usr/bin
LIB=/usr/lib
MAN=/usr/man/man1
CFLAGS=$(CCFLAGS)
LDFLAGS=
LIBS=-l++

OFILES = incl2.o 

all:    incl2
	chmod +x bin/incl.sh

incl2:	$(OFILES)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OFILES) $(LIBS)
	rm -f lib/incl2; strip incl2; ln incl2 lib

incl2.o:	incl2.c
	$(CC) $(CFLAGS) -c incl2.c

install:
	strip incl2
	/bin/cp bin/incl.sh $(BIN)/incl.sh
	/bin/cp incl2 $(LIB)

tests:	force
	cd tests; $(MAKE)

force:

clean: 	force
	rm -f *.dag *.dvi *.ps incl2.o
	rm -f incl2 lib/incl2

clobber:clean
