#ident	"@(#)Regex:makefile	3.1" 
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
O=-O
ROOT=../..
CCFLAGS = $(O) $(G) +w -I$(ROOT)/incl -DBUILDING_LIBRARY -DNDEBUG 
CFLAGS=$(O) $(G) -I./libx -DNDEBUG
LDFLAGS=$(G)

OBS=Regex.o Rematch.o Reiter.o Subex.o recomp.o reexec.o 
#Ident.o Int.o Float.o Double.o Alpha.o Alnum.o Reobj.o

all:	libRegex.a

libRegex.a: $(OBS)
	ar cr libRegex.a $(OBS)
	if test "$(SYS)" = BSD; then ranlib libRegex.a; fi

.c.o:
	$(CC) -c $(CCFLAGS) $<

recomp.o:	libx/recomp.c
	cc $(CFLAGS) -c libx/recomp.c

reexec.o:	libx/reexec.c
	cc $(CFLAGS) -c libx/reexec.c

install:
	/bin/cp libRegex.a $(LIB)
	if test "$(SYS)" = BSD; then ranlib $(LIB)/libRegex.a; fi

tests:	always
	cd tests; $(MAKE) -e

clean:
	rm -f $(OBS) *.a

clobber:	clean

always:


