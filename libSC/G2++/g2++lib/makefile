#ident	"@(#)G2++:g2++lib/makefile	3.1" 
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
SYS=BSD
CCFLAGS=-I. -I../incl -DBUILDING_LIBRARY -DNDEBUG
LIBS=-l++

OBJ = Text.o \
	err.o \
	g2ctab.o\
	g2ctype.o \
	g2debug.o \
	g2ntostr.o \
	g2tree.o \
	g2uio.o \
	g2util.o \
	getbuf.o \
	getdot.o \
	getrec.o \
	init.o \
	manip.o \
	putbuf.o \
	putrec.o

all: g2values.h g2ctab.c g2mach.h libg2++.a

g2values.h: geng2values
	./geng2values
	rm -f ../incl/g2values.h
	ln g2values.h ../incl

g2ctab.c: geng2ctab
	./geng2ctab

g2mach.h: geng2mach
	./geng2mach
	rm -f ../incl/g2mach.h
	ln g2mach.h ../incl

libg2++.a: $(OBJ) 
	@if [ -d Templates.DB ]; then \
		arit g2++ $(OBJ); \
	else \
		ar cr libg2++.a $(OBJ); \
	fi
	if test "$(SYS)" = BSD; then ranlib libg2++.a; fi

geng2ctab: geng2ctab.c
	$(CC) $(CCFLAGS) -o geng2ctab geng2ctab.c $(LIBS)

geng2mach:geng2mach.c
	$(CC) $(CCFLAGS) -o geng2mach geng2mach.c $(LIBS)

geng2values: geng2values.c
	$(CC) $(CCFLAGS) -o geng2values geng2values.c $(LIBS)

Text.o: Text.c
	$(CC) -c $(CCFLAGS) Text.c

err.o: err.c
	$(CC) -c $(CCFLAGS) err.c

g2clear.o: g2clear.c
	$(CC) -c $(CCFLAGS) g2clear.c

g2ctab.o: g2ctab.c
	$(CC) -c $(CCFLAGS) g2ctab.c

g2ctype.o: g2ctype.c
	$(CC) -c $(CCFLAGS) g2ctype.c

g2debug.o: g2debug.c
	$(CC) -c $(CCFLAGS) g2debug.c

g2ntostr.o: g2ntostr.c
	$(CC) -c $(CCFLAGS) g2ntostr.c

g2tree.o: g2tree.c
	$(CC) -c $(CCFLAGS) g2tree.c

g2uio.o: g2uio.c
	$(CC) -c $(CCFLAGS) g2uio.c

g2util.o: g2util.c
	$(CC) -c $(CCFLAGS) g2util.c

getbuf.o: getbuf.c
	$(CC) -c $(CCFLAGS) getbuf.c

getdot.o: getdot.c
	$(CC) -c $(CCFLAGS) getdot.c

getrec.o: getrec.c
	$(CC) -c $(CCFLAGS) getrec.c

init.o: init.c
	$(CC) -c $(CCFLAGS) init.c

manip.o: manip.c
	$(CC) -c $(CCFLAGS) manip.c

putbuf.o: putbuf.c
	$(CC) -c $(CCFLAGS) putbuf.c

putrec.o: putrec.c
	$(CC) -c $(CCFLAGS) putrec.c

clean:
	rm -f libg2++.a *.o geng2mach geng2ctab geng2values mon.out
	rm -f g2ctab.c g2mach.h g2values.h ../incl/g2mach.h ../incl/g2values.h
	rm -rf ptrepository Templates.DB
