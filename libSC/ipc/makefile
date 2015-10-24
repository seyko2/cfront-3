#ident	"@(#)ipc:makefile	3.1" 
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

CC = 
SYS =
ROOT=../..
CCFLAGS = +w -I$(ROOT)/incl -DBUILDING_LIBRARY

SRC = ipcstream.c ipcbuf.c testipc.c ipcmisc.c

OBS = ipcstream.o ipcbuf.o ipcmisc.o

libipc.a: $(OBS)
	ar cr libipc.a $(OBS)
	if test "$(SYS)" = BSD; then ranlib libipc.a;fi

ipcstream.o : ipcstream.c 
	$(CC) -c $(CCFLAGS) ipcstream.c

ipcbuf.o : ipcbuf.c
	$(CC) -c $(CCFLAGS) ipcbuf.c

ipcmisc.o : ipcmisc.c
	$(CC) -c $(CCFLAGS) ipcmisc.c

testipc.o : testipc.c
	$(CC) -c $(CCFLAGS) testipc.c

clean:
	rm -f *.o core libipc.a 

clobber: clean
