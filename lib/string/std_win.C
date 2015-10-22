/*ident	"@(#) @(#)std_win.c	1.1.1.3" */
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

#include <ctype.h>
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

// STREAMS INPUT/OUTPUT

istream&
operator>>(istream& ii, wstring& ss) {

    /*
     * skip whitespace -- return a null wstring if error encountered
     * while skipping whitespace
     */
    if (!ii.ipfx(0)) {
	ss = wstring();
	return ii;
    }

    register streambuf *nbp = ii.rdbuf();

    int fwidth = ii.width();
    if (fwidth != 0) {
	register int c = nbp->sgetc();
	if (c == EOF) {
	    /* no characters have been read in */
	    ss = wstring();
	    ii.clear(ios::badbit | ii.rdstate());
	}
	else {
	    ss.uniq();
	    ss.reserve(fwidth);
	    wchar_t *start = ss.d->str;
	    register wchar_t *p = start;
	    while (c != EOF && --fwidth > 0) {
		wchar_t cvalue = c << 8;
		nbp->snextc(); // advance the pointer
		int c1 = nbp->sgetc();
		cvalue += c1;
		if (c1 == EOF || WSTRING::is_del(cvalue)) {
			nbp->sputbackc(c);
			break;
		}
		*p++ = cvalue;
		nbp->snextc();
		c = nbp->sgetc();
	    }
	    ss.d->len = p - start;
	}
	return ii;
    }

    const int N = 64;
    int first = 1;

    /*
     * The first time through the loop, don't expand the size
     * of the existing wstring.  On the second and succeeding
     * times, append the new characters to the end.
     */
    while (ii && !ii.eof()) {
	register int c = nbp->sgetc();
	if (c == EOF)
	    break;

	wchar_t cvalue = (c << 8);
	nbp->snextc(); // advance the pointer
	int c1 = nbp->sgetc();
	cvalue += c1;
	nbp->sputbackc(c);
	if (c1 == EOF || WSTRING::is_del(cvalue)) {
	    break;
	}
	
	if (first) {
	    first = 0;
	    ss.uniq();
	    register int w = ss.d->max_size;
	    wchar_t *start = ss.d->str;
	    register wchar_t *p = start;

	    while (c != EOF && --w > 0) {
		wchar_t cvalue = c << 8;
		nbp->snextc(); // advance the pointer
		int c1 = nbp->sgetc();
		cvalue += c1;
		if (c1 == EOF || WSTRING::is_del(cvalue)) {
			nbp->sputbackc(c);
			break;
		}
		*p++ = cvalue;
		nbp->snextc();
		c = nbp->sgetc();
	    }

	    ss.d->len = p - start;
	}
	else {
	    wchar_t buf[N];
	    register int w = N;
	    register wchar_t *p = buf;

	    while (c != EOF && --w > 0) {
		wchar_t cvalue = c << 8;
		nbp->snextc(); // advance the pointer
		int c1 = nbp->sgetc();
		cvalue += c1;
		if (c1 == EOF || WSTRING::is_del(cvalue)) {
			nbp->sputbackc(c);
			break;
		}
		*p++ = cvalue;
		nbp->snextc();
		c = nbp->sgetc();
	    }
	    ss.append(buf, p - buf);
	}
    }

    if (first) {
	/* no characters have been read in */
	ss = wstring();
	ii.clear(ios::badbit | ii.rdstate());
    }
    return ii;
}

istream& getline(istream &ii, wstring &ss, wchar_t delim) {
    register streambuf *nbp = ii.rdbuf();
    const int N = 64;
    int fill_stringrep = 1; // normally set to 1 during the first time
			// through the main loop
    if (ss.d->max_size == 0) fill_stringrep = 0; // unless ss is a null string

    if (ii && (ii.eof() || nbp->sgetc() == EOF)) {
	ii.clear(ios::failbit | ii.rdstate());
	return ii;
    }

    while (ii && !ii.eof()) {
	register int c = nbp->sgetc();
	if (c == EOF)
	    break;

	wchar_t cvalue = c << 8;
	int c1 = nbp->snextc();
	cvalue += c1;
	if (c1 == EOF || cvalue == delim) {
	    if (cvalue == delim)
		nbp->sbumpc();
	    break;
	}
	else
	    c = nbp->snextc();

	register int w = N;
	register wchar_t *p;
	if (fill_stringrep) {
	  // first time through the loop -- overwrite the wstring
	  // representation with the data you are reading in
	    fill_stringrep = 0;
	    w = ss.d->max_size;
	    wchar_t *start = ss.d->str;
	    p = start;
	    *p++ = cvalue;
	    while (c != EOF && --w > 0) {
		cvalue = c << 8;
		int c1 = nbp->snextc();
		cvalue += c1;
		if (c1 == EOF || cvalue == delim) {
		    if (cvalue == delim)
			nbp->sbumpc();
		    break;
		}
		*p++ = cvalue;
		c = nbp->snextc();
	    }
	    ss.d->len = p - start;
	}
	else {
	 // second and subsequent times through the loop -- fill the
	 // array and append to the wstring
	    wchar_t buf[N];
	    w = N;
	    p = buf;
	    *p++ = cvalue;
	    while (c != EOF && --w > 0) {
		cvalue = c << 8;
		int c1 = nbp->snextc();
		cvalue += c1;
		if (c1 == EOF || cvalue == delim) {
		    if (cvalue == delim)
			nbp->sbumpc();
		    break;
		}
		*p++ = cvalue;
		c = nbp->snextc();
	    }
	    if (p - buf > 0) ss.append(buf, p - buf);
	}
	if (cvalue == delim) {
	    break;
	}
    }

    if (fill_stringrep) {
	ss = wstring();
    }
    return ii;
}

#ifdef NAMESPACES_LUCENT
}
#endif
