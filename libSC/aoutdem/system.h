/*ident	"@(#)aoutdem:system.h	3.1" */
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

/* -*-fundamental-*- */
int mv(char *, char *);
int cp(char *, char *);
int ap(char *, char *);
int cp_truncate(char *, char *, long);
int copy_access(char *, char *);
int system(char *, char *);
int system(char *, char *, char*);
int system(char *, char *, char*, char *);

