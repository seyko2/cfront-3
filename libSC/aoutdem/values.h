/*ident	"@(#)aoutdem:values.h	3.1" */
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

#define BITSPERBYTE     8
#define BITS(type)      (BITSPERBYTE * (int)sizeof(type))

/* short, regular and long ints with only the high-order bit turned on */
#define HIBITS  ((short)(1 << BITS(short) - 1))
#define HIBITI  (1 << BITS(int) - 1)
#define HIBITL  (1L << BITS(long) - 1)

/* largest short, regular and long int */
#define MAXSHORT        ((short)~HIBITS)
#define MAXINT  (~HIBITI)
#define MAXLONG (~HIBITL)

