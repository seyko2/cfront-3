/*ident	"@(#)G2++:incl/g2desc.h	3.1" */
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

#ifndef G2DESCH
#define G2DESCH

#include <stream.h>

typedef ostream& PUT(ostream&,const void*);
typedef istream& GET(istream&,void*);
typedef void NULLIFY(void*); 
typedef int IS_NULL(void*); 

struct G2DESC{
    char* name;    // field name 
    short type;    // 'L': leaf; 'S': struct; 'A': array
    short offset;  // relative to enclosing struct 
    short size;    // sizeof(...)
    short nel;     // struct: number of children
		      // array, string: 
		      //      0 (flexible)
		      //     >0 (fixed)
    G2DESC* child; // Pointer to first child. 
		   // Siblings are in contiguous, ascending
		   // memory.  Use 'nel' for sibling count.
    PUT* pfn;      // put function (for user-defined types)
    GET* gfn;      // get function (for user-defined types)
    NULLIFY* nfn;  // nullify function (for user-defined types)
    IS_NULL* isn;  // nullify function (for user-defined types)
};

#endif
