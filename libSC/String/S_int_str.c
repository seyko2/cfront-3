/*ident	"@(#)String:S_int_str.c	3.1" */
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
int_to_str(int j) 
{
    if(j==0) return String('0');

    int nodig=0,flag=0;
    int i, flag2=0;

    if (j < 0) {
	flag=1;
	i=-j;
	j=-j;
/*
	Special case to handle the smallest value (-MAXINT-1).  If we
	just negate the value it will be > than MAXINT, it will still be
	a negative number.  Just set the value to MAXINT and deal with
	it later.
*/
	if (i<0) {
	    j=i=MAXINT;
	    flag2=1;
	}
    }
    else {
	i=j;
    }

    //int i = j < 0 ? -j,flag=1,j = -j : j; 
    while(i > 0) {
        nodig++;
        i /= 10;
    }

    String rslt = Stringsize(nodig+1);
    char *rds = rslt.d->str;

    if(flag) { *rds = '-'; rds++; rslt.d->len = 1;}
    rds += nodig - 1;
    rslt.d->len += nodig;

/* early version, ran into MAXINT 
    int last = 0;
*/
    int dig = 0;
    int tmp = 1;
    for(i=1;i<=nodig;i++) {
	tmp = j;
	j /= 10;
	dig = tmp - j * 10;
	if (i==1 && flag2==1)  // Finish dealing with the smallest int case
	    dig++;
/* early version, ran into MAXINT 
        dig = ( (j % (10*tmp)) - last )/tmp;
        last = last + dig*tmp;
        tmp *= 10;
*/
 
        *rds = dig+48;
        --rds;
    }

    return rslt;
}
