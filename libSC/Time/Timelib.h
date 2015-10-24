/*ident	"@(#)Time:Timelib.h	3.1" */
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

#include "Time.h"
#include "mytime.h"
#include "tm.h"
#include <stddef.h>
#include <stdlib.h>
#ifndef __hpux
#include <sysent.h>
#else
#include <sys/unistd.h>
#endif
#include <string.h>
