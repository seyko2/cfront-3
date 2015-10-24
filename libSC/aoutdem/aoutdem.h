/*ident	"@(#)aoutdem:aoutdem.h	3.1" */
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

#include <osfcn.h>
#include "osfcn2.h"
#include <stdio.h>    
#include <libc.h>
#include <iostream.h>
#include <ctype.h>
#include <string.h>
#include <String.h>
#ifdef BSD
#include "values.h"
#define MARK(M)
#else
#include <values.h>
#include <prof.h>
#endif
#include "system.h"

#ifdef SYSV

#include <filehdr.h>
#include <aouthdr.h>
#include <scnhdr.h>
#include <syms.h>
#include <storclass.h>

#else

#include <a.out.h>
#include <stab.h>
#include <sys/file.h>

#endif

extern int optind;

#include <dem.h>
extern "C" {
	int dem(char*, DEM*, char*);
	int dem_print( DEM*, char*);
}

extern int verboseDbx;
FILE *xfopen(char *, char *);
int xfread(void *, int, int, FILE *);
int xfwrite(const void *, int, int, FILE *);
String demangleAndSimplify(const String &, int, int);
int hasextra_(int);
#ifndef SYSV
String parseDbxString(char *, int);
#endif


/* Most System V/non-System V differences hidden here.
*/
#ifdef SYSV

#define HDR filehdr
#define HDRSZ FILHSZ
#define SYM syment
#define SYMSZ SYMESZ

#define MAGIC(x)	(x).f_magic

#ifndef ISCOFF

#ifdef AMDWRMAGIC
#define ISAMD(x)	((x) == AMDWRMAGIC || (x) == AMDROMAGIC)
#else
#define ISAMD(x)	(0)
#endif

	// If your machine's magic number is not listed, add it.
	// (Look in /usr/include/filehdr.h to see the possibilities.)
	// Just because a magic number is listed doesn't mean
	// I've tested the demangler on that machine.  I don't
	// have access to all these machines.

#define ISCOFF(x) \
	ISAMD(x) || \
	((x) == XLMAGIC || (x) == N3BMAGIC || (x) == NTVMAGIC || \
	(x) == VAXROMAGIC || (x) == VAXWRMAGIC || \
	(x) == MC68MAGIC || (x) == MC68TVMAGIC || \
	(x) == M68MAGIC || (x) == M68TVMAGIC)

#endif

#define N_BADMAG(x) !(ISCOFF(MAGIC(x)))

#define N_SYMS(x) (x).f_nsyms
#define N_STROFF(x) ((x).f_symptr + ((x).f_nsyms * SYMESZ))
#define N_SYMOFF(x) (x).f_symptr
#define N_AUX(n) (n).n_numaux
#define N_STRINDEX(n) (n).n_offset

#else

#define HDR exec
#define HDRSZ sizeof(exec)
#define SYM nlist
#define SYMSZ sizeof(nlist)

#define MAGIC(x)	(x).a_magic

#define N_SYMS(x) ((x).a_syms/SYMSZ)
#define N_AUX(n) 0
#define N_STRINDEX(n) (n).n_un.n_strx

#endif
