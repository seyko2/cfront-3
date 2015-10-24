#ident	"@(#)hier:makefile	3.1" 
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
TOOL=hier

OFILES = hier2.o parse.o CXXLexer.o CXXToken.o Lexer.o Token.o

all:    hier2
	chmod +x bin/hier

hier2:	$(OFILES)
	$(CC) $(CCFLAGS) -o $@ $(OFILES) $(LIBS)
	rm -f lib/hier2; strip hier2; ln hier2 lib

install:
	strip hier2
	/bin/cp bin/$(TOOL) $(BIN)
	/bin/rm -f $(LIB)/hier2  # remove the old one
	/bin/cp hier2 $(LIB)

clean: 
	rm -f *.dag *.dvi *.ps *.o
	rm -f hier2 lib/hier2

clobber:clean

tests:	force
	cd tests; $(MAKE)

force:

