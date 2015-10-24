/*ident	"@(#)Time:mytime.h	3.1" */
/******************************************************************************
*
* C++ Standard Components, Release 3.0.
*
* Copyright (c) 1991, 1992 AT&T and Unix System Laboratories, Inc.
* Copyright (c) 1988, 1989, 1990 AT&T.  All Rights Reserved.
*
* THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T and Unix System
* Laboratories, Inc.  The copyright notice above does not evidence
* any actual or intended publication of such source code.
*
******************************************************************************/

#ifndef MYTIME_H
#define MYTIME_H

//  First, take everything from the standard time.h:

#include <time.h>

extern time_t timezone_ATTLC;

extern time_t altzone_ATTLC;
extern int daylight_ATTLC;
extern char *tzname_ATTLC[];

//  The following variables and functions are declared as
//  static in time_comm.c.  We need them to be extern:

extern time_t start_dst_ATTLC; /* Start date of alternate time zone */
extern time_t end_dst_ATTLC;   /* End date of alternate time zone */
extern tm *localtime_ATTLC(time_t*);
extern void getusa_ATTLC(long*,long*,tm*);
extern tm *gmtime_ATTLC(time_t*);
extern void tzset_ATTLC();

#endif
