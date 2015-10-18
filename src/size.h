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

extern int SZ_VLONG;
extern int AL_VLONG;

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

extern char* LARGEST_INT;
extern int F_SENSITIVE;	// is field alignment sensitive to the type of the field?
extern int F_OPTIMIZED;	// can the compiler fit a small int field into a char?
#endif
				// default sizes:
	// Note: #if doesn't work on all systems
#ifdef u3b
#define Abbb
#endif
#ifdef u3b2
#define Abbb
#endif
#ifdef u3b5
#define Abbb
#endif
#ifdef u3b15
#define Abbb
#endif
#ifdef pyr
#define Abbb
#endif


#ifdef alliant
#define Am68
#endif
#ifdef apollo
#define Am68
#endif
#if defined(sun2) || defined(mc68010)
#define Am68
#endif
#if defined(sun3) || defined(mc68020)
#define Am68
#endif
#if defined(mc68k) || defined(hp9000s200) || defined(hp9000s300)
#define Am68
#endif

#ifdef iAPX286
#ifdef LARGE
#define Ai286l
#endif
#endif

#ifdef Abbb
				/* AT&T 3Bs */
#define DBI_IN_WORD 32
#define DBI_IN_BYTE 8
#define DSZ_CHAR 1
#define DAL_CHAR 1
#define DSZ_SHORT 2
#define DAL_SHORT 2
#define DSZ_INT 4
#define DAL_INT 4
#define DSZ_LONG 4
#define DAL_LONG 4
#define DSZ_FLOAT 4
#define DAL_FLOAT 4
#define DSZ_DOUBLE 8
#define DAL_DOUBLE 4
#define DSZ_LDOUBLE 8
#define DAL_LDOUBLE 4
#define DSZ_STRUCT 4
#define DAL_STRUCT 4
//#define DSZ_FRAME 4
//#define DAL_FRAME 4
#define DSZ_WORD 4
#define DSZ_WPTR 4
#define DAL_WPTR 4
#define DSZ_BPTR 4
#define DAL_BPTR 4
//#define DSZ_TOP 0
//#define DSZ_BOTTOM 0
#define DLARGEST_INT "2147483647"	/* 2**31 - 1 */
#define DF_SENSITIVE 0
#define DF_OPTIMIZED 1
#else
#ifdef Am68
				/* most M68K boxes */
#if defined(hp9000s200) || defined(hp9000s300)
#define DBI_IN_WORD 16 
#else
#define DBI_IN_WORD 16
#endif
#define DBI_IN_BYTE 8
#define DSZ_CHAR 1
#define DAL_CHAR 1
#define DSZ_SHORT 2
#define DAL_SHORT 2
#define DSZ_INT 4
#define DAL_INT 2
#define DSZ_LONG 4
#define DAL_LONG 2
#define DSZ_FLOAT 4
#define DAL_FLOAT 2
#define DSZ_DOUBLE 8
#define DAL_DOUBLE 2
#define DSZ_LDOUBLE 8
#define DAL_LDOUBLE 2
#define DSZ_STRUCT 2
#define DAL_STRUCT 2
//#define DSZ_FRAME 4
//#define DAL_FRAME 4
#define DSZ_WORD 2
#define DSZ_WPTR 4
#define DAL_WPTR 2
#define DSZ_BPTR 4
#define DAL_BPTR 2
//#define DSZ_TOP 0
//#define DSZ_BOTTOM 0
#define DLARGEST_INT "2147483647"	/* 2**31 - 1 */
#define DF_SENSITIVE 0
#define DF_OPTIMIZED 1
#else
#ifdef Ai286l
				/* Intel 80286 large model */
#define DBI_IN_WORD 16
#define DBI_IN_BYTE 8
#define DSZ_CHAR 1
#define DAL_CHAR 1
#define DSZ_SHORT 2
#define DAL_SHORT 2
#define DSZ_INT 2
#define DAL_INT 2
#define DSZ_LONG 4
#define DAL_LONG 2
#define DSZ_FLOAT 4
#define DAL_FLOAT 2
#define DSZ_DOUBLE 8
#define DAL_DOUBLE 2
#define DSZ_LDOUBLE 8
#define DAL_LDOUBLE 2
#define DSZ_STRUCT 2
#define DAL_STRUCT 2
//#define DSZ_FRAME 4
//#define DAL_FRAME 4
#define DSZ_WORD 2
#define DSZ_WPTR 4
#define DAL_WPTR 2
#define DSZ_BPTR 4
#define DAL_BPTR 2
//#define DSZ_TOP 0
//#define DSZ_BOTTOM 0
#define DLARGEST_INT "32767"	/* 2**15 - 1 */
#define DF_SENSITIVE 0
#define DF_OPTIMIZED 1
#else
#if defined(uts) || defined(sun4) || defined(sparc)
				/* Amdahl running UTS */
#define DBI_IN_WORD 32
#define DBI_IN_BYTE 8
#define DSZ_CHAR 1
#define DAL_CHAR 1
#define DSZ_SHORT 2
#define DAL_SHORT 2
#define DSZ_INT 4
#define DAL_INT 4
#define DSZ_LONG 4
#define DAL_LONG 4
#define DSZ_FLOAT 4
#define DAL_FLOAT 4
#define DSZ_DOUBLE 8
#define DAL_DOUBLE 8
#define DSZ_LDOUBLE 8
#define DAL_LDOUBLE 8
#define DSZ_STRUCT 1
#define DAL_STRUCT 1
#define DSZ_WORD 4
#define DSZ_WPTR 4
#define DAL_WPTR 4
#define DSZ_BPTR 4
#define DAL_BPTR 4
#define DLARGEST_INT "2147483647"	/* 2**31 - 1 */
#if defined(uts)
#define DF_SENSITIVE 1
#else
#define DF_SENSITIVE 0
#endif
#define DF_OPTIMIZED 1
#else
#if defined(hpux) || defined(hp9000s800) 
                                /* hp */
#define DBI_IN_WORD 32
#define DBI_IN_BYTE 8
#define DSZ_CHAR 1
#define DAL_CHAR 1
#define DSZ_SHORT 2
#define DAL_SHORT 2
#define DSZ_INT 4
#define DAL_INT 4
#define DSZ_LONG 4
#define DAL_LONG 4
#define DSZ_FLOAT 4
#define DAL_FLOAT 4
#define DSZ_DOUBLE 8
#define DAL_DOUBLE 8 
#define DSZ_LDOUBLE 8
#define DAL_LDOUBLE 8
#define DSZ_STRUCT 1 
#define DAL_STRUCT 1 
#define DSZ_WORD 4
#define DSZ_WPTR 4
#define DAL_WPTR 4
#define DSZ_BPTR 4
#define DAL_BPTR 4
#define DLARGEST_INT "2147483647"       /* 2**31 - 1 */
#define DF_SENSITIVE 1 
#define DF_OPTIMIZED 1
#else
#if defined(vax) || defined(ibm032) || defined(_IBMR2) || defined(i386)
				/* VAX, IBM 32, IBM RS/6000, Intel 386 */
#define DBI_IN_WORD 32
#define DBI_IN_BYTE 8
#define DSZ_CHAR 1
#define DAL_CHAR 1
#define DSZ_SHORT 2
#define DAL_SHORT 2
#define DSZ_INT 4
#define DAL_INT 4
#define DSZ_LONG 4
#define DAL_LONG 4
#define DSZ_FLOAT 4
#define DAL_FLOAT 4
#define DSZ_DOUBLE 8
#define DAL_DOUBLE 4
#define DSZ_LDOUBLE 8
#define DAL_LDOUBLE 4
#define DSZ_STRUCT 1
#define DAL_STRUCT 1
//#define DSZ_FRAME 4
//#define DAL_FRAME 4
#define DSZ_WORD 4
#define DSZ_WPTR 4
#define DAL_WPTR 4
#define DSZ_BPTR 4
#define DAL_BPTR 4
//#define DSZ_TOP 0
//#define DSZ_BOTTOM 0
#define DLARGEST_INT "2147483647"	/* 2**31 - 1 */
#define DF_SENSITIVE 0
#define DF_OPTIMIZED 1
#else
#ifdef mc300
#define DBI_IN_WORD 32
#define DBI_IN_BYTE 8
#define DSZ_CHAR 1
#define DAL_CHAR 1
#define DSZ_SHORT 2
#define DAL_SHORT 2
#define DSZ_INT 4
#define DAL_INT 4
#define DSZ_LONG 4
#define DAL_LONG 4
#define DSZ_FLOAT 4
#define DAL_FLOAT 4
#define DSZ_DOUBLE 8
#define DAL_DOUBLE 4
#define DSZ_LDOUBLE 8
#define DAL_LDOUBLE 4
#define DSZ_STRUCT 2
#define DAL_STRUCT 2
//#define DSZ_FRAME 4
//#define DAL_FRAME 4
#define DSZ_WORD 4
#define DSZ_WPTR 4
#define DAL_WPTR 4
#define DSZ_BPTR 4
#define DAL_BPTR 4
//#define DSZ_TOP 0
//#define DSZ_BOTTOM 0
#define DLARGEST_INT "2147483647"       /* 2**31 - 1 */
#define DF_SENSITIVE 0
#define DF_OPTIMIZED 1
#else
#ifdef mips
#define DBI_IN_BYTE 8
#define DBI_IN_WORD 32
#define DSZ_WORD 4
#define DSZ_CHAR 1
#define DAL_CHAR 1
#define DSZ_SHORT 2
#define DAL_SHORT 2
#define DSZ_INT 4
#define DAL_INT 4
#define DLARGEST_INT "2147483647"
#define DSZ_LONG 4
#define DAL_LONG 4
#define DSZ_FLOAT 4
#define DAL_FLOAT 4
#define DSZ_DOUBLE 8
#define DAL_DOUBLE 8
#define DSZ_LDOUBLE 8 
#define DAL_LDOUBLE 8 
#define DSZ_BPTR 4
#define DAL_BPTR 4
#define DSZ_WPTR 4
#define DAL_WPTR 4
#define DSZ_STRUCT 1
#define DAL_STRUCT 1
#define DF_SENSITIVE 0
#define DF_OPTIMIZED 1
#else
				/* defaults: 0 => error */
#define DBI_IN_WORD 0
#define DBI_IN_BYTE 0
#define DSZ_CHAR 1
#define DAL_CHAR 1
#define DSZ_SHORT 0
#define DAL_SHORT 0
#define DSZ_INT 0
#define DAL_INT 0
#define DSZ_LONG 0
#define DAL_LONG 0
#define DSZ_VLONG 0
#define DAL_VLONG 0
#define DSZ_FLOAT 0
#define DAL_FLOAT 0
#define DSZ_DOUBLE 0
#define DAL_DOUBLE 0
#define DSZ_LDOUBLE 0
#define DAL_LDOUBLE 0
#define DSZ_STRUCT 0
#define DAL_STRUCT 0
#define DSZ_WORD 0
#define DSZ_WPTR 0
#define DAL_WPTR 0
#define DSZ_BPTR 0
#define DAL_BPTR 0
#define DLARGEST_INT "0"
#define DF_SENSITIVE 0
#define DF_OPTIMIZED 0
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif

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
