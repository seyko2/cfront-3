#ident	"@(#)Graph:makefile	3.1" 
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

CC = CC
SYS = 
ROOT = ../..
O=-O
CCFLAGS= +w $(O) -I$(ROOT)/incl -DBUILDING_LIBRARY -DNDEBUG

OBS = Graph.o Ticket.o Set_Graph.o

libGraph.a: $(OBS)
	@if [ -d Templates.DB ]; then \
		arit Graph $(OBS); \
	else \
		ar cr libGraph.a $(OBS); \
	fi
	if test "$(SYS)" = BSD; then ranlib libGraph.a; fi

Graph.o : Graph.c
	$(CC) -c $(CCFLAGS) Graph.c

Ticket.o : Ticket.c 
	$(CC) -c $(CCFLAGS) Ticket.c

Set_Graph.o : Set_Graph.c
	$(CC) -c $(CCFLAGS) Set_Graph.c

clean:
	rm -f libGraph.a *.o core 

clobber: clean
