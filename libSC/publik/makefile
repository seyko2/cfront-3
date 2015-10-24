#ident	"@(#)publik:makefile	3.1" 
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
BIN=/usr/bin
MAN=/usr/man/man1
LIB=/usr/lib
CFLAGS=$(CCFLAGS)
LDFLAGS=
LIBS=-l++

OFILES=CXXLexer.o CXXToken.o Lexer.o Token.o parse.o publik2.o scope.o

all:	publik2
	chmod +x bin/publik

publik2:	$(OFILES)
	$(CC) $(CCFLAGS) -o publik2 $(OFILES) $(LIBS)
	rm -f lib/publik2; strip publik2; ln publik2 lib

install:
	strip publik2
	/bin/cp bin/publik $(BIN)
	/bin/rm -f $(LIB)/publik2  # remove the old one
	/bin/cp publik2 $(LIB)
	
clean:	force
	rm -f *.o
	rm -f publik2 lib/publik2

clobber:clean

tests:	force
	cd tests; $(MAKE)

force:
