/*ident	"@(#)Pool:Poollib.h	3.1" */
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

#include <Pool.h>
#include <memory.h>
#include <assert.h>

static const int minblocksz = 1000;
static const int maxblocksz = 10000;

// most stringent alignment, in chars
// must be a power of 2 for this code to work
static const int round = 8;	

// offset in chars of the data part of a block
static const int blockoff = (sizeof(Block_header_ATTLC)+round-1) & (size_t)-round;

// rounds up n to the nearest multiple of round, except
// for 0, which gets rounded up to round
static size_t 
roundup(size_t n)
{
	return ((n + (n == 0) + round - 1) & (size_t)-round);
}
