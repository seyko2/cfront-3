/*ident	"@(#) @(#)std_wput.c	1.1.1.2" */
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

#if (defined(_MSC_VER) && _MSC_VER < 1000) || defined(__TCPLUSPLUS__)
#define WSTRING wstring
#else
#define WSTRING wstring::traits_type
#endif

/*
 *  The stream insertion operator has to deal with the field width
 */
ostream&
operator<<(ostream& oo, const wstring& r) {
    int fwidth = oo.width(0);	/* get the current "field width" setting */
    int len = r.d->len;		/* and the length of the string */

    int pad = fwidth - len;	/* this is the amount of padding needed */
    if (pad <= 0) {
	int i;
	wchar_t *p = r.d->str;
	for (i = 0; i < len; i++) {
	    WSTRING::char_out(oo, *p++);
	}
    }
    else {
	/*
	 * check if the padding goes before or after, default is before
	 */
	int leftjust = ( (oo.flags() & ios::left) != 0 );
	char fillchar = oo.fill();
	if (leftjust) {
	    int i;
	    wchar_t *p = r.d->str;
	    for (i = 0; i < len; i++) {
		WSTRING::char_out(oo, *p++);
	    }
	}
	for (; pad; pad--) {
	    oo.put(fillchar);
	    oo.put(fillchar);
	}
	if (!leftjust) {
	    int i;
	    wchar_t *p = r.d->str;
	    for (i = 0; i < len; i++) {
		WSTRING::char_out(oo, *p++);
	    }
	}
    }

    return oo;
}
#ifdef NAMESPACES_LUCENT
}
#endif
