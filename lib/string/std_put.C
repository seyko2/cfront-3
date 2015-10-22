/*ident	"@(#) @(#)std_put.c	1.1.1.2" */
/******************************************************************************
*
* C++ Standard Library
*
* Copyright (c) 1996  Lucent Technologies.  All Rights Reserved.
*
* THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF Lucent Technologies.
* The copyright notice above does not evidence any actual or
* intended publication of such source code.
*
******************************************************************************/

#include <iostream.h>
#include "std_string.h"
#ifdef NAMESPACES_LUCENT
namespace std {
#endif


/*
 *  The stream insertion operator has to deal with the field width
 *  stored in the stream object.
 */
ostream&
operator<<(ostream& oo, const string& r) {
#ifndef IOSTREAMH
    char *p = r.d->str;
    char *end = r.d->str + r.d->len;

    char c = *end;
    *end = '\0';
    while (p < end) {
	oo << p;
	while (*p) p++;
	if (p < end) {
	    oo.put((char)0);  /* write out embedded null */
	    p++;
	}
    }
    *end = c;
#else
    int fwidth = oo.width(0);	/* get the current "field width" setting */
    int len = r.d->len;		/* and the length of the string */

    int pad = fwidth - len;	/* this is the amount of padding needed */
    if (pad <= 0) {
	oo.write(r.d->str,r.d->len);
    }
    else {
	/*
	 * check if the padding goes before or after, default is before
	 */
	int leftjust = ( (oo.flags() & ios::left) != 0 );
	char fillchar = oo.fill();
	if (leftjust) {
	    oo.write(r.d->str,r.d->len);
	}
	for (; pad; pad--) {
	    oo.put(fillchar);
	}
	if (!leftjust) {
	    oo.write(r.d->str,r.d->len);
	}
    }
#endif

    return oo;
}
#ifdef NAMESPACES_LUCENT
}
#endif
