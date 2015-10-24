#ident	"@(#)String:makefile	3.1" 
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

CC= CC
SYS=
ROOT=../..
CCFLAGS= -D$(SYS) -O +w -I$(ROOT)/incl -DBUILDING_LIBRARY -DNDEBUG

SRC = S_substr.c S_queue.c S_string3.c S_misc.c S_add.c S_int_str.c S_long_str.c\
      S_read.c S_fputs.c S_puts.c S_fgets.c S_gets.c S_sgets.c\
      S_index.c S_input.c S_app.c S_ass.c S_setup.c S_square.c S_put.c\
      S_read1.c S_reserve.c

OBS = S_substr.o S_queue.o S_string3.o S_misc.o S_add.o S_int_str.o S_long_str.o\
      S_read.o S_fputs.o S_puts.o S_fgets.o S_gets.o S_sgets.o\
      S_index.o S_input.o S_app.o S_ass.o S_setup.o S_square.o S_put.o\
      S_read1.o S_reserve.o

libString.a: $(OBS)
	ar cr libString.a $(OBS)
	if test "$(SYS)" = BSD; then ranlib libString.a; fi

S_substr.o : S_substr.c 
	$(CC) -c $(CCFLAGS) S_substr.c 

S_int_str.o : S_int_str.c
	$(CC) -c $(CCFLAGS) S_int_str.c 

S_long_str.o : S_long_str.c
	$(CC) -c $(CCFLAGS) S_long_str.c

S_square.o : S_square.c
	$(CC) -c $(CCFLAGS) S_square.c 

S_queue.o : S_queue.c 
	$(CC) -c $(CCFLAGS) S_queue.c 

S_string3.o : S_string3.c 
	$(CC) -c $(CCFLAGS) S_string3.c 

S_misc.o : S_misc.c  
	$(CC) -c $(CCFLAGS) S_misc.c 

S_add.o : S_add.c 
	$(CC) -c $(CCFLAGS) S_add.c 

S_read.o : S_read.c 
	$(CC) -c $(CCFLAGS) S_read.c 

S_fputs.o : S_fputs.c 
	$(CC) -c $(CCFLAGS) S_fputs.c 

S_puts.o : S_puts.c 
	$(CC) -c $(CCFLAGS) S_puts.c 

S_fgets.o : S_fgets.c 
	$(CC) -c $(CCFLAGS) S_fgets.c 

S_gets.o : S_gets.c 
	$(CC) -c $(CCFLAGS) S_gets.c 

S_sgets.o : S_sgets.c 
	$(CC) -c $(CCFLAGS) S_sgets.c 

S_index.o : S_index.c 
	$(CC) -c $(CCFLAGS) S_index.c 

S_input.o : S_input.c  
	$(CC) -c $(CCFLAGS) S_input.c 

S_app.o : S_app.c 
	$(CC) -c $(CCFLAGS) S_app.c 

S_ass.o : S_ass.c 
	$(CC) -c $(CCFLAGS) S_ass.c 

S_setup.o : S_setup.c  
	$(CC) -c $(CCFLAGS) S_setup.c 

S_put.o : S_put.c
	$(CC) -c $(CCFLAGS) S_put.c

S_read1.o : S_read1.c
	$(CC) -c $(CCFLAGS) S_read1.c

S_reserve.o : S_reserve.c
	$(CC) -c $(CCFLAGS) S_reserve.c

clean:
	rm -f *.o *.a *.out core

clobber: clean
