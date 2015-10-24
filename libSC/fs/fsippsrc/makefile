#ident	"@(#)fs:fsippsrc/makefile	3.1" 
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
CFLAGS=-g
LDFLAGS=-g
LIBS=-l++

OFILES=CXXToken.o Lexer.o CXXLexer.o fsipp.o parse.o scope.o

all:	fsipp

fsipp:	$(OFILES)
	$(CC) $(CFLAGS) -o fsipp $(LDFLAGS) $(OFILES) $(LIBS)

clean:
	/bin/rm -f $(OFILES) 
	/bin/rm -rf ptrepository Templates.DB

clobber:clean
	/bin/rm -f fsipp
