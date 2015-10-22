/*ident	"@(#)cls4:tools/pt/pt.h	1.11" */
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

*******************************************************************************/
/************************* CUSTOMIZATION SECTION *************************/

#ifdef mips				/* path of nm */
#define NMPATH "/usr/bin/nm -B"		/* specify whatever options are */
#endif					/* needed to produce BSD or SysV */
#ifdef hpux				/* output formats */
#define NMPATH "/bin/nm -p"
#endif
#ifdef svr3
#define NMPATH "/bin/nm -p"
#endif
#if defined(svr4) || defined(solaris)
#define NMPATH "/usr/ccs/bin/nm"
#endif
#ifndef NMPATH
#define NMPATH "/bin/nm"
#endif

#ifdef sun				/* if have fast fork() */
#define fork vfork
#endif
#if defined(svr4) || defined(solaris)
#define fork vfork
#endif
#ifdef bsd2
#define fork vfork
#endif
#ifdef bsd3
#define fork vfork
#endif
#if defined(sparc) && !defined(svr4) && !defined(solaris)
#define fork vfork
#include <vfork.h>
#endif

#define LSPATH "/bin/ls"		/* ls path and options */

#define SRC_EXT ".c"			/* extension of C++ source files */
#define INC_EXT ".h"			/* extension of C++ header files */
#define OBJ_EXT ".o"			/* extension of object files */
#define ARC_EXT ".a"			/* extension of archive libraries */
#define DDC_EXT "..c"			/* extension of ..c files */

#ifdef sun				/* if filesystem supports file */
#define LONG_NAMES			/* names more than 14 characters */
#endif
#ifdef bsd2
#define LONG_NAMES
#endif
#ifdef bsd3
#define LONG_NAMES
#endif

/*#define SLOW_SYSTEM*/			/* to use standard versions of */
					/* system() and popen() if */
					/* optimized versions not suitable */

/*#define NO_LOCK*/			/* to disable locking or do your own */

#define LOCK_FCNTL			/* which style of locking */
/*#define LOCK_FLOCK*/
/*#define LOCK_LOCKF*/

#define LOCK_MAX 360			/* maximum iterations to get lock */
#define LOCK_SLEEP 10			/* sleep interval between lock tries */

/*#define NO_SIGNAL*/			/* to not use signals (in ptcomp.c) */
#ifndef NO_SIGNAL			/* to disable interrupts when */
#include <signal.h>			/* the default map file is rewritten */
#else
#define SIG_IGN 0
#endif

/*#define CHECKSUM*/			/* to get back old checksum scheme */

/************************* END CUSTOMIZATION *************************/

/********** NO CHANGES SHOULD BE REQUIRED BELOW THIS POINT **********/

#define MAXLINE 4096			/* general buffer use */
#define MAXPATH 2048			/* max pathname length */
#define MAXCBUF 8192			/* max command buffer */

#define LISTSTART 16			/* starting size of string lists */

#define SYMTSIZE 2003			/* symbol table size (a prime) */
#define TFTABSIZE 997			/* file mapping table (prime) */
#define MAXMAPL 16384			/* max size of a map file line */
#define MAXCPP 16384			/* max length of cpp output line */
#define CS_EXT ".cs"			/* checksum extension */
#define HC_EXT ".hc"			/* used for short filenames */
#define HE_EXT ".he"			/* used for header caches */
#define FTSIZE 97			/* size of file name list (prime) */
#define DISIZE 97			/* size of dir inst table (prime) */

#define FTSIZE2 4001			/* size of filename table (prime) */
#define TFTABSIZE2 4001			/* type -> file map table (prime) */
#define BNSIZE 997			/* basename table (prime) */

#define STRCMP(s, t) ((s)[0] != (t)[0] || strcmp((s), (t)) != 0)
					/* fast strcmp() */

#ifdef SLOW_SYSTEM
#define fast_system system
#define fast_popen popen
#define fast_pclose pclose
#endif

char* gs();				/* function declarations */
char* copy();
char* basename();
unsigned long timestamp();
unsigned long hash();
FILE* fast_popen();
long get_time();

typedef struct Slist Slist;		/* string lists */
struct Slist {
	int n;
	int max;
	char** ptrs;
};
