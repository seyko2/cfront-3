/*ident	"@(#)String:demos/replace.c	3.1" */
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

#include <String.h>
#include <stream.h>

String
replace(String in_String, String pattern, String rep)
{
    int i = in_String.index(pattern);
    if( i < 0) return in_String;

    int plen = length(pattern);
    int rlen = length(rep);
    int j;

    do {
        in_String(i, plen) = rep;
        i += rlen;
        String rest = in_String(i, length(in_String)-i);
        i += (j = rest.index(pattern));
    }
    while(j >= 0);

    return in_String;
}


main(int, char *argv[])
{
    String in_String;
    //String rslt;

    while( gets(in_String) ) {
        String rslt = replace(in_String,argv[1],argv[2]);
        cout << rslt << "\n";
    }
    return 0;
}
