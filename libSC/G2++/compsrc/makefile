#ident	"@(#)G2++:compsrc/makefile	3.1" 
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
SYS=
CCFLAGS=-I../incl -I../../../incl -DBUILDING_LIBRARY 
LIBS=-l++

########################################################
# You shouldn't have to change anything below this line.
########################################################
OBJ = comp.o desc.o typedef.o 

g2++comp: $(OBJ)
	$(CC) $(CCFLAGS) -o g2++comp $(OBJ) $(LIBS)
	strip g2++comp

comp.o: comp.c
	$(CC) -c $(CCFLAGS) comp.c

desc.o: desc.c
	$(CC) -c $(CCFLAGS) desc.c

typedef.o: typedef.c
	$(CC) -c $(CCFLAGS) typedef.c

clean:
	rm -f g2++comp core *.o 
	rm -rf ptrepository Templates.DB

clobber:clean
