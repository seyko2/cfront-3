#ident	"@(#)Time:makefile	3.1" 
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
ROOT=../..
CCFLAGS=+w -O -I$(ROOT)/incl -DBUILDING_LIBRARY

OBS = 	Duration.o \
	Place.o \
	Time.o \
	Tprint.o \
	Time_prev.o \
	mystrdup.o\
	time_comm.o \
	tmdata.o \
	tmdate.o \
	tmfix.o\
	tmform.o \
	tmgoff.o \
	tmleap.o \
	tmlex.o \
	tmmake.o \
	tmpoff.o\
	tmtime.o \
	tmtype.o \
	tmword.o \
	tmzone.o

libTime.a: $(OBS)
	ar cr libTime.a $(OBS)
	if test "$(SYS)" = BSD ; then ranlib libTime.a; fi

mystrdup.o: tm/mystrdup.c
	$(CC) -c $(CCFLAGS) tm/mystrdup.c

tmdata.o: tm/tmdata.c
	$(CC) -c -I. $(CCFLAGS) tm/tmdata.c

tmdate.o: tm/tmdate.c
	$(CC) -c -I. $(CCFLAGS) tm/tmdate.c

tmfix.o: tm/tmfix.c
	$(CC) -c -I. $(CCFLAGS) tm/tmfix.c

tmform.o: tm/tmform.c
	$(CC) -c -I. $(CCFLAGS) tm/tmform.c

tmgoff.o: tm/tmgoff.c
	$(CC) -c -I. $(CCFLAGS) tm/tmgoff.c

tmleap.o: tm/tmleap.c
	$(CC) -c -I. $(CCFLAGS) tm/tmleap.c

tmlex.o: tm/tmlex.c
	$(CC) -c -I. $(CCFLAGS) tm/tmlex.c

tmmake.o: tm/tmmake.c
	$(CC) -c -I. $(CCFLAGS) tm/tmmake.c

tmpoff.o: tm/tmpoff.c
	$(CC) -c -I. $(CCFLAGS) tm/tmpoff.c

tmtime.o: tm/tmtime.c
	$(CC) -c -I. $(CCFLAGS) tm/tmtime.c

tmtype.o: tm/tmtype.c
	$(CC) -c -I. $(CCFLAGS) tm/tmtype.c

tmword.o: tm/tmword.c
	$(CC) -c -I. $(CCFLAGS) tm/tmword.c

tmzone.o: tm/tmzone.c
	$(CC) -c -I. $(CCFLAGS) tm/tmzone.c

time_comm.o: ctime/time_comm.c
	$(CC) -c -I. $(CCFLAGS) ctime/time_comm.c

Duration.o: Duration.c
	$(CC) -c $(CCFLAGS) Duration.c

Time.o: Time.c
	$(CC) -c $(CCFLAGS) Time.c

Place.o: Place.c
	$(CC) -c $(CCFLAGS) Place.c

Tprint.o: Tprint.c
	$(CC) -c $(CCFLAGS) Tprint.c

Time_prev.o: Time_prev.c
	$(CC) -c $(CCFLAGS) Time_prev.c

clean:
	rm -f *.o core *.a

clobber:clean
