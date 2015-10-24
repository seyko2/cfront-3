/*ident	"@(#)G2++:incl/g2comp.h	3.2" */
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

#ifndef G2COMPH
#define G2COMPH

#include <g2io.h>
#include <g2tree.h>
#include <Block.h>
#include <Map.h>
#include <String.h>

//  ASC strings are used to prepopulate the symbol table 
//  so that CHAR, SHORT, LONG are recognized as keywords 
//  to indicate char, short, and long data types. 

const String CHAR_ASC_ATTLC   = int_to_str(CHAR_INT_ATTLC);
const String SHORT_ASC_ATTLC  = int_to_str(SHORT_INT_ATTLC);
const String LONG_ASC_ATTLC   = int_to_str(LONG_INT_ATTLC);
const String STRING_ASC_ATTLC = int_to_str(STRING_INT_ATTLC);

const int G2MOTHER_ATTLC=0xAAAA;

void desc(
    G2NODE*, 
    const String&, 
    FILE*
);
G2NODE* lookup(
    const String&
);
void tdef(
    G2NODE*, 
    FILE*,
    FILE*
);
String upper(
    const String& s
);

struct udt_info_ATTLC{
    Block<String> headers;
    int		header_count;
    String 	put;
    String 	get;
    String	null;
    String	isnull;
    // udt_info():headers(10){}  -- blowing up.  Why?
};

extern Map<String,udt_info_ATTLC> udt_map_ATTLC;

#endif
