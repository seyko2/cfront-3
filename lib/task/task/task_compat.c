/*ident "@(#)cls4:lib/task/task/task_compat.c	1.3" */
/*******************************************************************************
 
C++ source for the C++ Language System, Release 3.0.  This product
is a new release of the original cfront developed in the computer
science research center of AT&T Bell Laboratories.

Copyright (c) 1993  UNIX System Laboratories, Inc.
Copyright (c) 1991, 1992 AT&T and UNIX System Laboratories, Inc.
Copyright (c) 1984, 1989, 1990 AT&T.  All Rights Reserved.

THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE of AT&T and UNIX System
Laboratories, Inc.  The copyright notice above does not evidence
any actual or intended publication of such source code.


task_compat.c:
        The functions in this file allow object files that were created
        with C++ 2.0 or C++ 2.1 to be linked with a task library
        compiled with C++ 3.0.  The types "statetype" and "modetype"
	are enums defined within the classes sched and task respectively
        and their names in argument lists is different in C++ 3.0 because
	of the new "nested classes" feature.

*****************************************************************************/

#include <task.h>

extern "C" {	
  void __ct__4taskFPc8modetypei(task *, char *, task::modetype, int);
  char *mode_string__F8modetype(task::modetype);
  char *state_string__F9statetype(sched::statetype);
}

extern "C" {
  void __ct__4taskFPcQ2_4task8modetypei(task *, char *, task::modetype, int);
}

extern char *mode_string(task::modetype);
extern char *state_string(sched::statetype);

void
__ct__4taskFPc8modetypei(task *t, char *name, task::modetype mode, int stacksize) {
	__ct__4taskFPcQ2_4task8modetypei(t, name, mode, stacksize);
}

char *
mode_string__F8modetype(task::modetype m) {
	return (mode_string(m));
}

char *
state_string__F9statetype(sched::statetype s) {
	return (state_string(s));
}
