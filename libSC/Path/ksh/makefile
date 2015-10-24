#ident	"@(#)Path:ksh/makefile	3.1" 
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


# I purposely left the ksh stuff in good-old C in order
# to keep it as close as possible to the actual ksh 
# source.  This way, any future updates to ksh will
# be easy to apply.
#
CC=cc
LDFLAGS=-g
LSTAT=-DLSTAT=1
CFLAGS=-g $(LSTAT)
OFILES=test.o expand.o 

all:	ksh.a t

t:	t.o ksh.a
	yCC -o t t.o ../strmatch.o ksh.a -l++

t.o:	t.c
	yCC $(CFLAGS) -c t.c

group:	group.o
	cc -o group group.o
	chmod 2777 group

ksh.a:	$(OFILES)
	/bin/rm -f $@
	ar rc $@ $(OFILES)
	ranlib $@

clean:
	rm -f $(OFILES)


