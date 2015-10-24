/*ident	"@(#)ipc:streamdefs.h	3.1" */
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

// This file contains #defines for controlling conditional compilation 
// of the stream library


// The follwing functions adjust for machine dependencies

#define BREAKEVEN	10    
			/* The approximate length of a string
			 * for which it is faster to do a strncpy
			 * than a char by char loop. If BREAKEVEN is 0 
			 * then strncpy is always better.  If it is <0 then 
			 * loop is always better, (e.g. if strncopy does
			 * a char by char copy anyway.)
			 */

#define SEEK_ARITH_OK	1
			/* System supports arithmetic on stream positions.
			 * I.e. if file is at a position and we read or
			 * write n bytes we can find the new position
			 * by adding n to old position. (Providing
			 * O_APPEND isn't set in on open.)
			 */

static const int PTRBASE = 16 ;
			/* Base used for output of "void*" */

// There is one important machine dependent feature of this implementation
// It assumes that it can always create a pointer to the byte after
// a char array used as a buffer, and that pointer will be greater than
// any pointer into the array. 
// My reading of the ANSI standard is that this assumption is permissible,
// but I can imagine segmented architectures where it fails.

/* #define VSPRINTF vsprintf */

			/* If defined, the name of a "vsprintf" function.
			 * If not defined, 
			 * various non-portable kludges are used in
			 * oldformat.c
			 */

static const int STREAMBUFSIZE = 1024 ;
			// The default buffer size.

/*******
	#define O_CREAT 01000 
	#define O_TRUNC 02000 
 *******/
			/* Used in filebuf.c.  Define if your system
			 * needs it to have a value different from
			 * that indicated here, but doesn't
			 * define it in standard system headers
			 */
