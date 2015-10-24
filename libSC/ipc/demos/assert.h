/*ident	"@(#)cls4:incl-master/proto-headers/assert.h	1.1" */
/* This header file intentionally has no wrapper, since the user
*  may want to re-include it to turn off/on assertions for only
*  a portion of the source file.
*/


#ifdef assert
#undef assert
#endif
#ifdef _assert
#undef _assert
#endif

#ifdef NDEBUG

#define assert(e) ((void)0)

#else

#ifndef __STDIO_H
#include <stdio.h>
#endif

#ifndef __STDLIB_H
#include <stdlib.h>
#endif

#if defined(__STDC__)
#define assert(EX) (void)((EX) || (fprintf(stderr,"Assertion failed: " # EX ", file \"%s\", line %d\n", __FILE__, __LINE__), abort(), 0))
#else
#define assert(EX) (void)((EX) || (fprintf(stderr,"Assertion failed: EX file \"%s\", line %d\n", __FILE__, __LINE__), abort(), 0))
#endif
#endif

#define _assert(e) assert(e)

