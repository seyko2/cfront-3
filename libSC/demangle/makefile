#ident	"@(#)demangle:makefile	3.2" 
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

BSD=1
CC=cc
CFLAGS=-O -I.
TARGETS=c++filt dem libmangle.a

all: $(TARGETS)

OFILES= dem.o 

libmangle.a: $(OFILES)
	ar cr libmangle.a $(OFILES)
	if test "$(SYS)" = BSD; then ranlib libmangle.a; fi

dem.o:	dem.c
	cc $(CFLAGS) -c dem.c 

demMain.o: demMain.c
	cc $(CFLAGS) -c demMain.c 

c++filt: dem.c
	cc -DDEM_MAIN $(CFLAGS) -o c++filt dem.c
	rm -f lib/c++filt; strip c++filt; ln c++filt lib

dem:	demMain.c dem.c
	cc $(CFLAGS) -o dem demMain.c dem.c
	rm -f lib/dem; strip dem; ln dem lib

clean:
	rm -f *.o libmangle.a dem c++filt
	rm -f lib/c++filt lib/dem

clobber: clean
	rm -f $(TARGETS)
