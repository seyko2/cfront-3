/*ident	"@(#)cls4:src/size.h	1.6" */
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

size.h:
	sizes and alignments used to calculate sizeofs

	table and butffer sizes

***************************************************************************/
#ifndef GRAM
extern int BI_IN_WORD;
extern int BI_IN_BYTE;
				/*	byte sizes */
extern int SZ_CHAR;
extern int AL_CHAR;

extern int SZ_SHORT;
extern int AL_SHORT;

extern int SZ_INT;
extern int AL_INT;

extern int SZ_LONG;
extern int AL_LONG;

extern int SZ_LLONG;
extern int AL_LLONG;

extern int SZ_FLOAT;
extern int AL_FLOAT;

extern int SZ_DOUBLE;
extern int AL_DOUBLE;

extern int SZ_LDOUBLE;
extern int AL_LDOUBLE;

extern int SZ_STRUCT;	/* minimum struct size */
extern int AL_STRUCT;

//extern SZ_FRAME;
//extern AL_FRAME;

extern int SZ_WORD;

extern int SZ_WPTR;
extern int AL_WPTR;

extern int SZ_BPTR;
extern int AL_BPTR;	

//extern SZ_TOP;
//extern SZ_BOTTOM;

extern char* LARGEST_INT, * LARGEST_LONG, * LARGEST_LLONG;
extern int F_SENSITIVE;	// is field alignment sensitive to the type of the field?
extern int F_OPTIMIZED;	// can the compiler fit a small int field into a char?
#endif

#include "./szal.result"

#define KTBLSIZE	123	/*	initial keyword table size */
#define GTBLSIZE	257	/*	initial global name table size */
#define CTBLSIZE	12	/*	initial class table size */
#define TBLSIZE		20	/*	initial block table size */
#define BLMAX		128	/*	max block nesting */
#define MAXERR		13	/* maximum number of errors before terminating */

#ifndef GRAM
const int CHUNK = 8*1024;
void* chunk(int);
#endif
