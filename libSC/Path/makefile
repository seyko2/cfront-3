#ident	"@(#)Path:makefile	3.1" 
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
BSD=

# If you're building this component separately, this is
# where it gets installed.
#
LIB=/usr/lib
MAN=/usr/man/man3
INC=/usr/include/CC

#############################################################
# You shouldn't have to change anything below this line.
#############################################################
comp=Path
ROOT=../..
O=
CCFLAGS = +w $(O) -I$(ROOT)/incl -DBUILDING_LIBRARY
CFLAGS=$(O)

KOBS=test.o expand.o 
LOBS=strmatch.o
OBS=$(KOBS) $(LOBS) canon.o complete.o die.o dirname.o explode.o \
	find.o is_wd.o kshtest.o makerel.o ctlist.o no_wd.o path.o \
	prefix.o slash.o search.o strdup.o stream1.o stream2.o \
	tilde.o tmppath.o truncate.o wild.o xgetwd.o PATH.o

all:	lib$(comp).a

lib$(comp).a: $(OBS)
	@if [ -d Templates.DB ]; then \
		arit $(comp) $(OBS); \
	else \
		ar cr lib$(comp).a $(OBS); \
	fi
	if test "$(SYS)" = BSD; then ranlib lib$(comp).a; fi

section3.h:
	sh ./gendefines lstat getwd getcwd >section3.h

test.o:	ksh/test.c section3.h
	cc $(CFLAGS) -c ksh/test.c

expand.o:ksh/expand.c section3.h
	cc $(CFLAGS) -c ksh/expand.c

strmatch.o:	libx/strmatch.c
	cc $(CFLAGS) -c libx/strmatch.c

.c.o:
	$(CC) -c $(CCFLAGS) $<

clean:
	rm -f *.o *.a a.out section3.h

clobber:	clean

install:
	/bin/cp lib$(comp).a $(LIB)
	if test "$(SYS)" = BSD; then ranlib $(LIB)/lib$(comp).a; fi

tests:	always
	cd tests; $(MAKE) -e

always:
