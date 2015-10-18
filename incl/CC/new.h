/*ident	"@(#)cls4:incl-master/const-headers/new.h	1.2" */
/*******************************************************************************
 
C++ source for the C++ Language System, Release 3.0.  This product
is a new release of the original cfront developed in the computer
science research center of AT&T Bell Laboratories.

Copyright (c) 1991, 1992 AT&T and UNIX System Laboratories, Inc.
Copyright (c) 1984, 1989, 1990 AT&T.  All Rights Reserved.

THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE of AT&T and UNIX System
Laboratories, Inc.  The copyright notice above does not evidence
any actual or intended publication of such source code.

*******************************************************************************/
#ifndef __NEW_H
#define __NEW_H

#pragma lib "c++/libC.a"

#ifndef __STDDEF_H
#include <stddef.h>
#endif

extern void (*set_new_handler (void(*)()))();

void *operator new(size_t, void*);

#endif
