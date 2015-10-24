#ident	"@(#)Symbol:makefile	3.1" 
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

BSD=
CC=CC
ROOT=../..
O=-O
CCFLAGS=+w $(O) -I$(ROOT)/incl -DBUILDING_LIBRARY -DNDEBUG

OBJ = $(SYMBOLOBJ) 

SYMBOLOBJ = string_set.o Symbol.o Symbol_in.o Symbol_out.o

libSymbol.a: $(OBJ)
	@if [ -d Templates.DB ]; then \
		arit Symbol $(OBJ); \
	else \
		ar cr libSymbol.a $(OBJ); \
	fi
	if test "$(SYS)" = BSD; then ranlib libSymbol.a; fi

string_set.o: string_set.c
	$(CC) -c $(CCFLAGS) string_set.c

List_string.o: List_string.c 
	$(CC) -c $(CCFLAGS) List_string.c

Symbol.o: Symbol.c
	$(CC) -c $(CCFLAGS) Symbol.c

Symbol_in.o: Symbol_in.c
	$(CC) -c $(CCFLAGS) Symbol_in.c

Symbol_out.o: Symbol_out.c
	$(CC) -c $(CCFLAGS) Symbol_out.c

clean:
	rm -f *.o core *.a

clobber:clean
