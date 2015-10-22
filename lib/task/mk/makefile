#ident	"@(#)cls4:lib/task/mk/makefile	1.3"
###############################################################################
#
# C++ source for the C++ Language System, Release 3.0.  This product
# is a new release of the original cfront developed in the computer
# science research center of AT&T Bell Laboratories.
#
# Copyright (c) 1993  UNIX System Laboratories, Inc.
# Copyright (c) 1991, 1992   AT&T and UNIX System Laboratories, Inc.
# Copyright (c) 1984, 1989, 1990 AT&T.  All Rights Reserved.
#
# THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE of AT&T and UNIX System
#	
# Laboratories, Inc.  The copyright notice above does not evidence
# any actual or intended publication of such source code.
#
###############################################################################
O_TASK		=	erand.o event.o fudge.o hw_stack.o obj.o qhead.o qtail.o sched.o sim.o swap.o swap_supp.o task.o timer.o task_compat.o
OFILES	=	$(O_TASK)
CC	=	CC
CCFLAGS	=	-O
SRC=../task

libtask.a:	$(OFILES)
		ar r libtask.a $(OFILES)
		ar r libtask.a $(OFILES)
		if test "$(BSD)" ; then ranlib libtask.a; fi

erand.o:	$(SRC)/erand.c
	$(CC) $(CCFLAGS) -I$(SRC) -c $(SRC)/erand.c

event.o:	$(SRC)/event.c
	$(CC) $(CCFLAGS) -I$(SRC) -c $(SRC)/event.c

fudge.o:	$(SRC)/hw_stack.h $(SRC)/fudge.c
	$(CC) $(CCFLAGS) -I$(SRC) -c $(SRC)/fudge.c

$(SRC)/fudge.c:	$(SRC)/fudge.c.$(MACH)
	cp $(SRC)/fudge.c.$(MACH) $(SRC)/fudge.c

$(SRC)/swap.s:	$(SRC)/swap.s.$(MACH)
	cp $(SRC)/swap.s.$(MACH) $(SRC)/swap.s

hw_stack.o:	$(SRC)/hw_stack.h $(SRC)/hw_stack.c
	$(CC) $(CCFLAGS) -I$(SRC) -c $(SRC)/hw_stack.c

obj.o:		$(SRC)/obj.c
	$(CC) $(CCFLAGS) -I$(SRC) -c $(SRC)/obj.c

qhead.o:	$(SRC)/qhead.c
	$(CC) $(CCFLAGS) -I$(SRC) -c $(SRC)/qhead.c

qtail.o:	$(SRC)/qtail.c
	$(CC) $(CCFLAGS) -I$(SRC) -c $(SRC)/qtail.c

sched.o:	$(SRC)/sched.c
	$(CC) $(CCFLAGS) -I$(SRC) -c $(SRC)/sched.c

sim.o:		$(SRC)/hw_stack.h $(SRC)/sim.c
	$(CC) $(CCFLAGS) -I$(SRC) -c $(SRC)/sim.c

swap.o:		$(SRC)/swap.s
	$(CC) $(CCFLAGS) -I$(SRC) -c $(SRC)/swap.s

swap_supp.o:	$(SRC)/swap_supp.c
	$(CC) $(CCFLAGS) -I$(SRC) -c $(SRC)/swap_supp.c

task.o:		$(SRC)/hw_stack.h $(SRC)/task.c
	$(CC) $(CCFLAGS) -I$(SRC) -c $(SRC)/task.c

timer.o:	$(SRC)/timer.c
	$(CC) $(CCFLAGS) -I$(SRC) -c $(SRC)/timer.c

task_compat.o:	$(SRC)/task_compat.c
	$(CC) $(CCFLAGS) -I$(SRC) -c $(SRC)/task_compat.c
