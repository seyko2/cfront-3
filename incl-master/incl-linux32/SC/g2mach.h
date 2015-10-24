/*ident	"@(#)G2++:incl/g2mach.h	3.0"  */
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

//
// G2 machine-dependent parameters
//
// This file was generated from the various g2mach.h files for
// the systems supported by department 59112
//
#ifndef G2MACHH
#define G2MACHH

#if defined(sparc) || defined(uts) || defined (vax) || defined(i386) || defined(hp9000s800) || defined(mips)
const int CHAR_SIZE_ATTLC=1;
const int SHORT_SIZE_ATTLC=2;
const int LONG_SIZE_ATTLC=4;
const int STRING_SIZE_ATTLC=4;
const int VBLOCK_SIZE_ATTLC=12;

const int CHAR_ALIGN_ATTLC=1;
const int SHORT_ALIGN_ATTLC=2;
const int LONG_ALIGN_ATTLC=4;
const int STRING_ALIGN_ATTLC=4;
const int VBLOCK_ALIGN_ATTLC=4;
const int STRUCT_ALIGN_ATTLC=1;
#endif

#if defined(m68k) || defined(mc68000)
const int CHAR_SIZE_ATTLC=1;
const int SHORT_SIZE_ATTLC=2;
const int LONG_SIZE_ATTLC=4;
const int STRING_SIZE_ATTLC=4;
const int VBLOCK_SIZE_ATTLC=12;

const int CHAR_ALIGN_ATTLC=1;
const int SHORT_ALIGN_ATTLC=2;
const int LONG_ALIGN_ATTLC=2;
const int STRING_ALIGN_ATTLC=2;
const int VBLOCK_ALIGN_ATTLC=2;
const int STRUCT_ALIGN_ATTLC=2;
#endif

#if defined(u3b) || defined(u3b2) || defined(u3b15) || defined(pyr)
const int CHAR_SIZE_ATTLC=1;
const int SHORT_SIZE_ATTLC=2;
const int LONG_SIZE_ATTLC=4;
const int STRING_SIZE_ATTLC=4;
const int VBLOCK_SIZE_ATTLC=12;

const int CHAR_ALIGN_ATTLC=1;
const int SHORT_ALIGN_ATTLC=2;
const int LONG_ALIGN_ATTLC=4;
const int STRING_ALIGN_ATTLC=4;
const int VBLOCK_ALIGN_ATTLC=4;
const int STRUCT_ALIGN_ATTLC=4;
#endif

#endif
