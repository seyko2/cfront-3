/*ident	"@(#)G2++:incl/g2values.h	3.0"  */
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
#ifndef G2VALUESH
#define G2VALUESH

//
// This file was generated from the various g2values.h files for
// the systems supported by department 59112
//

#if defined(sparc) || defined(uts) || defined(vax) || defined(i386) || defined(m68k) || defined(hp9000s800) || defined(mips) || defined(u3b) || defined(u3b2) || defined(u3b15) || defined(pyr) || defined(mc68000) || defined(__linux__)
#ifndef HIBITS
#define HIBITS 0x8000
#endif
#ifndef HIBITI
#define HIBITI 0x80000000
#endif
#ifndef HIBITL
#define HIBITL 0x80000000
#endif
#endif

#if defined(__alpha)
#ifndef HIBITS
#define HIBITS 0x8000
#endif
#ifndef HIBITI
#define HIBITI 0x80000000
#endif
#ifndef HIBITL
#define HIBITL 0x8000000000000000
#endif
#endif

#endif
