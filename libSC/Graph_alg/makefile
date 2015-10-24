#ident	"@(#)Graph_alg:makefile	3.1" 
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
LINKOPT=$(ROOT)/lib/lib++.a $(ROOT)/lib/libSet.a $(ROOT)/lib/libGraph.a
CCFLAGS= +w -O -I$(ROOT)/incl -DBUILDING_LIBRARY -DNDEBUG

OBS = Graph_alg.o dfs.o bfs.o cycle.o component.o 

libGA.a: $(OBS)
	@if [ -d Templates.DB ]; then \
		arit GA $(OBS); \
	else \
		ar cr libGA.a $(OBS); \
	fi
	if test "$(SYS)" = BSD; then ranlib libGA.a; fi

Graph_alg.o : Graph_alg.c
	$(CC) -c $(CCFLAGS) Graph_alg.c

component.o : component.c
	$(CC) -c $(CCFLAGS) component.c

dfs.o: dfs.c
	$(CC) -c $(CCFLAGS) dfs.c

bfs.o: bfs.c
	$(CC) -c $(CCFLAGS) bfs.c

cycle.o: cycle.c
	$(CC) -c $(CCFLAGS) cycle.c

List_Graph.o: List_Graph.c
	$(CC) -c $(CCFLAGS) List_Graph.c

Listp_Edge.o: Listp_Edge.c
	$(CC) -c $(CCFLAGS) Listp_Edge.c

Listp_Vertex.o: Listp_Vertex.c
	$(CC) -c $(CCFLAGS) Listp_Vertex.c

Set_Graph.o: Set_Graph.c 
	$(CC) -c $(CCFLAGS) Set_Graph.c

Setp_SetpV.o: Setp_SetpV.c 
	$(CC) -c $(CCFLAGS) Setp_SetpV.c

clean:
	rm -f *.o libGA.a core

clobber: clean

