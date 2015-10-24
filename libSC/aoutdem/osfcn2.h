/*ident	"@(#)aoutdem:osfcn2.h	3.1" */
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

	
/*
*  Uncomment whichever ones are missing from your osfcn.h or libc.h
*/

extern "C" {
//	int stat(const char*, struct stat*);
//	int system(const char*);
//	int chmod(const char*, int);
//	char *mktemp(char *);
//	int getopt(int, const char **, const char *);
//	int moncontrol(int);
#ifndef SYSV
//	void perror(const char*);
#ifndef V9
//	int setbuffer(FILE *, char *, int);
//	int truncate(const char *, off_t);
//	int ftruncate(int, off_t);
#endif
#endif
}
