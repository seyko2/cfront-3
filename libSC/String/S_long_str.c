/*ident	"@(#)String:S_long_str.c	3.1" */
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

#define IN_STRING_LIB
#include "String.h"
#include "values.h"

String 
long_to_str(long j) 
{
    if(j==0) return String('0');

    int nodig=0,flag=0;
    long i;
    int flag2=0;

    if (j < 0) {
        flag=1;
        i=-j;
        j=-j;
/*
        Special case to handle the smallest value (-MAXLONG-1).  If we
        just negate the value it will be > than MAXLONG, it will still be
        a negative number.  Just set the value to MAXLONG and deal with
        it later.
*/
        if (i<0) {
            j=i=MAXLONG;
            flag2=1;
        }
    }
    else {
        i=j;
    }

    //long i = j < 0 ? -j,flag=1,j = -j : j;
    while(i > 0) {
        nodig++;
        i /= 10;
    }

    String rslt = Stringsize(nodig+1);
    char *rds = rslt.d->str;

    if(flag) { *rds = '-'; rds++; rslt.d->len = 1;}
    rds += nodig - 1;
    rslt.d->len += nodig;

/* early version: will run into MAXINT
    long last = 0;
*/
    long dig = 0;
    long tmp = 1;
    for(i=1;i<=nodig;i++) {
	tmp = j;
	j /= 10;
	dig = tmp - j * 10;
        if (i==1 && flag2==1)  // Finish dealing with the smallest int case
            dig++;
/* early version: will run into MAXINT
        dig = ( (j % (10*tmp)) - last )/tmp;
        last = last + dig*tmp;
        tmp *= 10;
*/
 
        *rds = (char) dig+48;
        --rds;
    }

    return rslt;
}
