/*ident	"@(#)ipc:ipclib.h	3.1" */
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

#include "streamdefs.h"
#include <unistd.h>
#include <fstream.h>
#include <ipcstream.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#if !defined(BSD)
#include <syms.h>
#endif
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>

String ipc_basename_ATTLC(const String& arg);
String ipc_fix_name_ATTLC(const char* p);
String ipc_fix_name_ATTLC(const String& p);
