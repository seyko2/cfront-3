/*ident	"@(#) @(#)std_wstr.c	1.1.1.2" */
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

#include "std_string.h"
#include <Pool_std.h>

#if (defined(_MSC_VER) && _MSC_VER < 1000) || defined(__TCPLUSPLUS__)
#define WSTRING wstring
#else
#define WSTRING wstring::traits_type
#endif

// Addition:

#ifdef NAMESPACES_LUCENT
namespace std {
#endif
wstring
operator+(const wstring& s, const wstring& r)
{
    register int slen = s.d->len;
    register int rlen = r.d->len;

    if(rlen == 0) return wstring(s);
    if(slen == 0) return wstring(r);

    register int newln = rlen + slen;
    register wstring::Srep_LUCENT* rd =
	wstring::Srep_LUCENT::new_srep(newln);

    WSTRING::copy(rd->str, s.d->str, slen);
    WSTRING::copy(rd->str+slen, r.d->str, rlen);

    return wstring(rd);
}

wstring
operator+(const wstring& s, const wchar_t *st)
{
    register int slen = s.d->len;
    register int ln = st ? WSTRING::length(st) : 0;

    if(ln==0) return wstring(s);

    register int newln = ln + slen;
    register wstring::Srep_LUCENT* rd =
	wstring::Srep_LUCENT::new_srep(newln);

    WSTRING::copy(rd->str, s.d->str, slen);
    WSTRING::copy(rd->str+slen, st, ln);

    return wstring(rd);
}

wstring
operator+(const wchar_t *st, const wstring& s)
{
    register int ln = st ? WSTRING::length(st) : 0;
    register int slen = s.d->len;

    if(ln==0) return wstring(s);

    register int newln = ln + slen;
    register wstring::Srep_LUCENT* rd = wstring::Srep_LUCENT::new_srep(newln);

    WSTRING::copy(rd->str, st, ln);
    WSTRING::copy(rd->str+ln, s.d->str, slen);

    return wstring(rd);
}

wstring
operator+(const wstring& s, wchar_t c)
{
    register int slen = s.d->len;

    register int newln = 1 + slen;
    register wstring::Srep_LUCENT* rd = wstring::Srep_LUCENT::new_srep(newln);

    WSTRING::copy(rd->str, s.d->str, slen);
    rd->str[slen] = c;

    return wstring(rd);
}

wstring
operator+(wchar_t c, const wstring& s)
{
    register int slen = s.d->len;

    register int newln = 1 + slen;
    register wstring::Srep_LUCENT* rd = wstring::Srep_LUCENT::new_srep(newln);

    WSTRING::copy(rd->str+1, s.d->str, slen);
    rd->str[0] = c;

    return wstring(rd);
}

// Appending:

wstring&
wstring::newcopy(wchar_t c)
{
    register int oldlen = d->len;
    register wstring::Srep_LUCENT *x = wstring::Srep_LUCENT::new_srep((oldlen+1));
    if (oldlen > 0) wstring::traits_type::copy(x->str, d->str, oldlen);
    x->str[oldlen] = c;
    d->rcdec();
    d = x;
    return *this;
}

wstring&
wstring::append(const wstring &s, size_t pos, size_t n) {
#if defined(EXCEPTION_LUCENT)
    if (pos >= s.d->len) throw out_of_range(string("in append"));
#endif
    if (n == 0)
	return (*this);
    if (n == npos)
	n = s.d->len;

    register int oldlen = d->len;
    register int newln = n + oldlen;

    if (d->refc > 1 || newln >= d->max_size) {
	register wstring::Srep_LUCENT *x = wstring::Srep_LUCENT::new_srep(newln);
	if (oldlen > 0) wstring::traits_type::copy(x->str, d->str, oldlen);
	wstring::traits_type::copy(x->str+oldlen, s.d->str+pos, n);
	d->rcdec();
	d = x;
	return (*this);
    }
    wstring::traits_type::copy(d->str+oldlen, s.d->str+pos, n);
    d->len += n;
    return (*this);
}

wstring&
wstring::append(const wchar_t *s, size_t ln)
{
    if (ln == 0) 
	return (*this);
    if (ln == 1) {  // optimization
	*this += *s;
	return (*this);
    }
    if (d->max_size == 0) {
	assign(s, ln);
	return (*this);
    }

    register int oldlen = d->len;
    register int newln = ln + oldlen;

    if (d->refc > 1 || newln >= d->max_size) {
	register wstring::Srep_LUCENT *x = wstring::Srep_LUCENT::new_srep(newln);
	if (oldlen > 0) wstring::traits_type::copy(x->str, d->str, oldlen);
	if (ln > 0) wstring::traits_type::copy(x->str+oldlen, s, ln);
	d->rcdec();
	d = x;
	return (*this);
    }
    if (ln > 0) wstring::traits_type::copy(d->str+oldlen, s, ln);
    d->len += ln;
    return (*this);
}
wstring&
wstring::append(const wchar_t *s) {
    return (append(s, wstring::traits_type::length(s)));
}
wstring&
wstring::append(size_t n, wchar_t c) {
    if (n == 0) 
	return (*this);

    register int oldlen = d->len;
    register int newln = n + oldlen;

    if (d->refc > 1 || newln >= d->max_size) {
	register wstring::Srep_LUCENT *x = wstring::Srep_LUCENT::new_srep(newln);
	if (oldlen > 0) wstring::traits_type::copy(x->str, d->str, oldlen);
	memset(x->str+oldlen, c, n);
	d->rcdec();
	d = x;
	return (*this);
    }
    memset(d->str+oldlen, c, n);
    d->len += n;
    return (*this);
}

wstring&
wstring::operator+=(const wchar_t * s)
{
    register int ln = s ? wstring::traits_type::length(s) : 0;
    append(s, ln);
    return *this;
}

wstring&
wstring::operator+=(const wstring& s)
{
    append(s.d->str, s.d->len);
    return *this;
}

// STRING OPERATORS:

// Assignment:

wstring&
wstring::assign(const wstring &s, size_t pos, size_t n) {
#if defined(EXCEPTION_LUCENT)
    if (pos >= s.d->len) throw out_of_range(string("in assign"));
#endif
    if (n == npos)
	n = s.d->len - pos;
    else {
	int maxlen = s.d->len - pos;
	if (n > maxlen) n = maxlen;
    }
    if (d->refc > 1 || n >= d->max_size) {
	d->rcdec();
	d = wstring::Srep_LUCENT::new_srep(n);
    }
    d->len = n;
    if (n != 0) wstring::traits_type::copy(d->str, s.d->str + pos, n);
    return (*this);
}

wstring&
wstring::assign(const wchar_t *s, size_t ln)
{
    if (ln == 1) {  // optimization
	*this = *s;
	return (*this);
    }
    if (d->refc > 1 || ln >= d->max_size) {
	d->rcdec();
	d = wstring::Srep_LUCENT::new_srep(ln);
    }
    d->len = ln;
    if (ln > 0) wstring::traits_type::copy(d->str,s,ln);
    return (*this);
}

wstring&
wstring::assign(const wchar_t *s) {
    return (assign(s, wstring::traits_type::length(s)));
}

wstring&
wstring::assign(size_t n, wchar_t c) {
    if (d->refc > 1 || n >= d->max_size) {
	d->rcdec();
	d = wstring::Srep_LUCENT::new_srep(n);
    }
    d->len = n;
    if (n != 0) memset(d->str,c,n);
    return (*this);
}

wstring&
wstring::operator=(const wchar_t *s)
{
    register int ln = s ? wstring::traits_type::length(s) : 0;
    assign(s, ln);
    return *this;
}

/* position of first occurrence of wchar_t */
size_t
wstring::find(wchar_t c, size_t pos) const
{
    register wchar_t *p = d->str - 1 + pos;
    register wchar_t *q = d->str + d->len;
    while (++p < q)
        if (wstring::traits_type::eq(*p, c))
            return p - d->str;
    return npos;
}

size_t
wstring::find(const wstring& pattern, size_t pos) const
{
    if (pos == npos) pos = d->len;
    register int plen = pattern.d->len;
    register int slen = d->len;
    register const wchar_t *pp = pattern.d->str;
    register const wchar_t *sbp = d->str + pos;
    register const wchar_t *sep = d->str + slen;

    int i;
    for (i=pos; sbp<sep-plen+1; i++) {
        if (wstring::traits_type::compare(sbp++, pp, plen) == 0) return i;
    }
    return npos;
}
size_t
wstring::find(const wchar_t *s, size_t pos, size_t n) const
{
    register int plen = n;
    register int slen = d->len;
    register const wchar_t *pp = s;
    register const wchar_t *sbp = d->str + pos;
    register const wchar_t *sep = d->str + slen;

    int i;
    for (i=pos; sbp<sep-plen+1; i++) {
        if (wstring::traits_type::compare(sbp++, pp, plen) == 0) return i;
    }
    return npos;
}
size_t
wstring::find(const wchar_t *s, size_t pos) const
{
    register int plen = wstring::traits_type::length(s);
    register int slen = d->len;
    register const wchar_t *pp = s;
    register const wchar_t *sbp = d->str + pos;
    register const wchar_t *sep = d->str + slen;

    int i;
    for (i=pos; sbp<sep-plen+1; i++) {
        if (wstring::traits_type::compare(sbp++, pp, plen) == 0) return i;
    }
    return npos;
}

/* ... last ... (npos if wchar_t not there) */
size_t
wstring::rfind(wchar_t c, size_t pos) const
{
    register wchar_t *p;
    if (pos == npos)
	p = d->str + d->len;
    else
	p = d->str + pos;
    register wchar_t *q = d->str;
    while (--p >= q)
        if (wstring::traits_type::eq(*p, c))
            return p - d->str;
    return npos;
}
size_t
wstring::rfind(const wstring& pattern, size_t pos) const
{
    if (pos == npos) pos = d->len;
    register int plen = pattern.d->len;
    register int slen = d->len;
    register const wchar_t *pp = pattern.d->str;
    register const wchar_t *sep = d->str + pos;
    register const wchar_t *sbp = d->str + slen - plen;

    int i;
    for(i=slen-plen; sbp>=sep; i--) {
        if (wstring::traits_type::compare(sbp--, pp, plen) == 0) return i;
    }
    return npos;
}
size_t
wstring::rfind(const wchar_t *s, size_t pos, size_t n) const
{
    register int plen = n;
    register int slen = d->len;
    register const wchar_t *pp = s;
    register const wchar_t *sep = d->str + pos;
    register const wchar_t *sbp = d->str + slen - plen;

    int i;
    for (i=slen-plen; sbp>=sep; i--) {
        if(wstring::traits_type::compare(sbp--, pp, plen) == 0) return i;
    }
    return npos;
}
size_t
wstring::rfind(const wchar_t *s, size_t pos) const
{
    register int plen = wstring::traits_type::length(s);
    register int slen = d->len;
    register const wchar_t *pp = s;
    register const wchar_t *sep = d->str + pos;
    register const wchar_t *sbp = d->str + slen - plen;

    int i;
    for (i=slen-plen; sbp>=sep; i--) {
        if (wstring::traits_type::compare(sbp--, pp, plen) == 0) return i;
    }
    return npos;
}

size_t
wstring::find_first_of(const wstring &s, size_t pos) const
{
    register const wchar_t *p, *q;
    const wstring tmpstring(s);
    for (p = d->str-1+pos, q=d->str+d->len; ++p < q; )
	if (tmpstring.find(*p) != npos)
	    return p - d->str;
    return npos;
}
size_t
wstring::find_first_of(const wchar_t *s, size_t pos, size_t n) const
{
    register const wchar_t *p, *q;
    const wstring tmpstring(s, n);
    for (p = d->str-1+pos, q=d->str+d->len; ++p < q; )
        if (tmpstring.find(*p) != npos)
            return p - d->str;
    return npos;
}
size_t
wstring::find_first_of(const wchar_t *s, size_t pos) const
{
    register const wchar_t *p, *q;
    const wstring tmpstring(s);
    for (p = d->str-1+pos, q=d->str+d->len; ++p < q; )
        if (tmpstring.find(*p) != npos)
            return p - d->str;
    return npos;
}
size_t
wstring::find_first_of(wchar_t c, size_t pos) const {
    return (find(c, pos));
}
size_t
wstring::find_last_of(const wstring &s, size_t pos) const
{
    register const wchar_t *p, *q;
    const wstring tmpstring(s);
    for (p = d->str+d->len, q=d->str+pos; --p >= q; )
	if (tmpstring.find(*p) != npos)
	    return p - d->str;
    return npos;
}
size_t
wstring::find_last_of(const wchar_t *s, size_t pos, size_t n) const
{
    register const wchar_t *p, *q;
    const wstring tmpstring(s, n);
    for (p = d->str+d->len, q=d->str+pos; --p >= q; )
	if (tmpstring.find(*p) != npos)
	    return p - d->str;
    return npos;
}
size_t
wstring::find_last_of(const wchar_t *s, size_t pos) const
{
    register const wchar_t *p, *q;
    const wstring tmpstring(s);
    for (p = d->str+d->len, q=d->str+pos; --p >= q; )
	if (tmpstring.find(*p) != npos)
	    return p - d->str;
    return npos;
}
size_t
wstring::find_last_of(wchar_t c, size_t pos) const {
    return (rfind(c, pos));
}

size_t
wstring::find_first_not_of(const wstring &s, size_t pos) const
{
    register const wchar_t *p, *q;
    const wstring tmpstring(s);
    for (p = d->str-1+pos, q=d->str+d->len; ++p < q; )
	if (!tmpstring.find(*p) != npos)
	    return p - d->str;
    return npos;
}
size_t
wstring::find_first_not_of(const wchar_t *s, size_t pos, size_t n) const
{
    register const wchar_t *p, *q;
    const wstring tmpstring(s, n);
    for (p = d->str-1+pos, q=d->str+d->len; ++p < q; )
	if (!tmpstring.find(*p) != npos)
	    return p - d->str;
    return npos;
}
size_t
wstring::find_first_not_of(const wchar_t *s, size_t pos) const
{
    register const wchar_t *p, *q;
    const wstring tmpstring(s);
    for (p = d->str-1+pos, q=d->str+d->len; ++p < q; )
	if (!tmpstring.find(*p) != npos)
	    return p - d->str;
    return npos;
}
size_t
wstring::find_first_not_of(wchar_t c, size_t pos) const
{
    register const wchar_t    *p, *q;
    for (p = d->str-1+pos, q=d->str+d->len; ++p < q; )
        if (!wstring::traits_type::eq(*p, c))
            return p - d->str;
    return npos;
}
size_t
wstring::find_last_not_of(const wstring &s, size_t pos) const
{
    register const wchar_t *p, *q;
    const wstring tmpstring(s);
    for (p = d->str+d->len, q=d->str+pos; --p >= q; )
	if (!tmpstring.find(*p) != npos)
	    return p - d->str;
    return npos;
}
size_t
wstring::find_last_not_of(const wchar_t *s, size_t pos, size_t n) const
{
    register const wchar_t *p, *q;
    const wstring tmpstring(s, n);
    for (p = d->str+d->len, q=d->str+pos; --p >= q; )
	if (!tmpstring.find(*p) != npos)
	    return p - d->str;
    return npos;
}
size_t
wstring::find_last_not_of(const wchar_t *s, size_t pos) const
{
    register const wchar_t *p, *q;
    const wstring tmpstring(s);
    for (p = d->str+d->len, q=d->str+pos; --p >= q; )
	if (!tmpstring.find(*p) != npos)
	    return p - d->str;
    return npos;
}
size_t
wstring::find_last_not_of(wchar_t c, size_t pos) const
{
    register wchar_t    *p, *q;
    for (p = d->str+d->len, q=d->str+pos; --p >= q; )
        if (!wstring::traits_type::eq(*p, c))
            return p - d->str;
    return npos;
}

// Inserting:

wstring&
wstring::insert(size_t pos1, const wstring &str, size_t pos2, size_t n) {
    if (pos1 > d->len)
#if defined(EXCEPTION_LUCENT)
	throw out_of_range(string("in insert"));
#else
	pos1 = d->len;
#endif
    if (pos2 > str.d->len)
#if defined(EXCEPTION_LUCENT)
	throw out_of_range(string("in insert"));
#else
	pos2 = str.d->len;
#endif
    if (n == npos)
	n = str.d->len - pos2;
    else {
	int maxlen = str.d->len - pos2;
	if (n > maxlen) n = maxlen;
    }
    if (n == 0)
	return (*this);

    register int oldlen = d->len;
    register int newln = n + oldlen;

    if (d->refc > 1 || newln >= d->max_size) {
	register wstring::Srep_LUCENT *x = wstring::Srep_LUCENT::new_srep(newln);
	if (pos1 > 0) wstring::traits_type::copy(x->str, d->str, pos1);
	wstring::traits_type::copy(x->str+pos1, str.d->str+pos2, n);
	wstring::traits_type::copy(x->str+pos1+n, d->str+pos1, oldlen-pos1);
	d->rcdec();
	d = x;
	return (*this);
    }
    int diff = oldlen - pos1;
    wchar_t *tmp;
    if (diff > 0) {
	tmp = new wchar_t[diff];
	wstring::traits_type::copy(tmp, d->str+pos1, diff);
    }
    wstring::traits_type::copy(d->str+pos1, str.d->str+pos2, n);
    if (diff > 0) {
	wstring::traits_type::copy(d->str+pos1+n, tmp, diff);
	delete tmp;
    }
    d->len += n;
    return (*this);
}
wstring&
wstring::insert(size_t pos, const wchar_t *s, size_t n)
{
    if (pos > d->len)
#if defined(EXCEPTION_LUCENT)
	throw out_of_range(string("in insert"));
#else
	pos = d->len;
#endif
    if (n == 0) 
	return (*this);

    register int oldlen = d->len;
    register int newln = n + oldlen;

    if (d->refc > 1 || newln >= d->max_size) {
	register wstring::Srep_LUCENT *x = wstring::Srep_LUCENT::new_srep(newln);
	if (pos > 0) wstring::traits_type::copy(x->str, d->str, pos);
	wstring::traits_type::copy(x->str+pos, s, n);
	wstring::traits_type::copy(x->str+pos+n, d->str+pos, oldlen-pos);
	d->rcdec();
	d = x;
	return (*this);
    }
    int diff = oldlen - pos;
    wchar_t *tmp;
    if (diff > 0) {
	tmp = new wchar_t[diff];
	wstring::traits_type::copy(tmp, d->str+pos, diff);
    }
    wstring::traits_type::copy(d->str+pos, s, n);
    if (diff > 0) {
	wstring::traits_type::copy(d->str+pos+n, tmp, diff);
	delete tmp;
    }
    d->len += n;
    return (*this);
}
wstring&
wstring::insert(size_t pos, const wchar_t *s) {
    return (insert(pos, s, wstring::traits_type::length(s)));
}
wstring&
wstring::insert(size_t pos, size_t n, wchar_t c) {
    if (pos > d->len)
#if defined(EXCEPTION_LUCENT)
	throw out_of_range(string("in insert"));
#else
	pos = d->len;
#endif
    if (n == 0) 
	return (*this);

    register int oldlen = d->len;
    register int newln = n + oldlen;

    if (d->refc > 1 || newln >= d->max_size) {
	register wstring::Srep_LUCENT *x = wstring::Srep_LUCENT::new_srep(newln);
	if (pos > 0) wstring::traits_type::copy(x->str, d->str, pos);
	memset(x->str+pos, c, n);
	wstring::traits_type::copy(x->str+pos+n, d->str+pos, oldlen-pos);
	d->rcdec();
	d = x;
	return (*this);
    }
    int diff = oldlen - pos;
    wchar_t *tmp;
    if (diff > 0) {
	tmp = new wchar_t[diff];
	wstring::traits_type::copy(tmp, d->str+pos, diff);
    }
    memset(d->str+pos, c, n);
    if (diff > 0) {
	wstring::traits_type::copy(d->str+pos+n, tmp, diff);
	delete tmp;
    }
    d->len += n;
    return (*this);
}
wstring::iterator
wstring::insert(iterator p, wchar_t c) {
    return (insert(p, 1, c));
}
wstring::iterator
wstring::insert(iterator p, size_t n, wchar_t c) {
    if (p < d->str || p > d->str+d->len)
#if defined(EXCEPTION_LUCENT)
	throw out_of_range(string("in insert"));
#else
	p = d->str+d->len;
#endif
    iterator retval = p;
    if (n == 0) 
	return (retval);

    register int oldlen = d->len;
    register int newln = n + oldlen;
    int pos = p - d->str;

    if (d->refc > 1 || newln >= d->max_size) {
	register wstring::Srep_LUCENT *x = wstring::Srep_LUCENT::new_srep(newln);
	if (pos > 0) wstring::traits_type::copy(x->str, d->str, pos);
	memset(x->str+pos, c, n);
	wstring::traits_type::copy(x->str+pos+n, d->str+pos, oldlen-pos);
	d->rcdec();
	d = x;
	retval = x->str + pos;
	return (retval);
    }
    int diff = oldlen - pos;
    wchar_t *tmp;
    if (diff > 0) {
	tmp = new wchar_t[diff];
	wstring::traits_type::copy(tmp, d->str+pos, diff);
    }
    memset(d->str+pos, c, n);
    if (diff > 0) {
	wstring::traits_type::copy(d->str+pos+n, tmp, diff);
	delete tmp;
    }
    d->len += n;
    return (retval);
}

const size_t wstring::npos = (size_t) -1;

size_t
wstring::copy(wchar_t *s, size_t n, size_t pos) {
    if (pos > d->len)
#if defined(EXCEPTION_LUCENT)
	throw out_of_range(string("in copy"));
#else
	pos = d->len;
#endif
    if (n == npos)
	n = d->len - pos;
    else {
	int max_len = d->len - pos;
	if (n > max_len) n = max_len;
    }
    wstring::traits_type::copy(s, d->str+pos, n); 
    return (n);
}

void
wstring::swap(wstring &s) {
    wstring::Srep_LUCENT *p = s.d;
    s.d = this->d;
    this->d = p;
}

int
wstring::compare(const wstring& s, size_t pos, size_t n) const {
	int retval;
	if (pos == npos)
		pos = d->len;
	register int dlen = d->len - pos;
	register int slen = s.d->len;
	if (n == npos || n > dlen || n > slen)
		n = (dlen < slen) ? dlen : slen;
	retval = wstring::traits_type::compare(d->str + pos, s.d->str, n);
	if (retval == 0)
		retval = dlen - slen;
	return retval;
}

int
wstring::compare(const wchar_t *s, size_t pos, size_t n) const {
	int retval;
	if (pos == npos)
		pos = d->len;
	register int dlen = d->len - pos;
	register int slen = n;
	if (n > dlen)
		n = dlen;
	retval = wstring::traits_type::compare(d->str + pos, s, n);
	if (retval == 0)
		retval = dlen - slen;
	return retval;
}

int
wstring::compare(const wchar_t *s, size_t pos) const {
	register int dlen = d->len - pos;
	register int n = s ? wstring::traits_type::length(s) : 0;
	register int slen = n;
	if (n > dlen)
		n = dlen;
	int retval = wstring::traits_type::compare(d->str + pos, s, n);
	if (retval == 0)
		retval = dlen - slen;
	return retval;
}
void
wstring::resize(size_t n, wchar_t c) {
    if (n == npos) {
#if defined(EXCEPTION_LUCENT)
	throw length_error(string("in resize"));
#else
	n = d->len;
#endif
    }
    if (d->len == n) return;

    int copycount = d->len;
    if (copycount > n) copycount = n;

    if (d->refc > 1 || n >= d->max_size) {
	register wstring::Srep_LUCENT *x = wstring::Srep_LUCENT::new_srep(n);
	wstring::traits_type::copy(x->str, d->str, copycount);
	d->rcdec();
	d = x;
    }
    if (n > copycount) memset(d->str + copycount, c, n - copycount);
    d->len = n;
}

// Removing:

wstring&
wstring::remove(size_t pos, size_t n) {
    if (pos > d->len)
#if defined(EXCEPTION_LUCENT)
	throw out_of_range(string("in remove"));
#else
	pos = d->len;
#endif
    if (n == npos)
	n = d->len - pos;
    else {
	int max_diff = d->len - pos;
	if (n > max_diff) n = max_diff;
    }
    if (n == 0)  // do nothing
	return (*this);
    if (pos == 0 && n == d->len)  // clear the entire wstring
	return (assign(d->str, 0));
	
    register int newln = d->len - n;

    if (d->refc > 1) {
	register wstring::Srep_LUCENT *x = wstring::Srep_LUCENT::new_srep(newln);
	if (pos > 0) wstring::traits_type::copy(x->str, d->str, pos);
	wstring::traits_type::copy(x->str+pos, d->str+pos+n, newln-pos);
	d->rcdec();
	d = x;
	return (*this);
    }
    wstring::traits_type::copy(d->str+pos, d->str+pos+n, newln-pos);
    d->len -= n;
    return (*this);
}
wstring&
wstring::remove(iterator p) {
    return (remove(p - d->str, 1));
}
wstring&
wstring::remove(iterator first, iterator last) {
    return (remove(first - d->str, last - first));
}

// Removing:

wstring&
wstring::replace(size_t pos1, size_t n1, const wstring &str, size_t pos2, size_t n2) {
    if (pos1 > d->len)
#if defined(EXCEPTION_LUCENT)
	throw out_of_range(string("in replace"));
#else
	pos1 = d->len;
#endif
    if (pos2 > str.d->len)
#if defined(EXCEPTION_LUCENT)
	throw out_of_range(string("in replace"));
#else
	pos2 = str.d->len;
#endif
    if (n1 == npos)
	n1 = d->len - pos1;
    else {
	int max_len = d->len - pos1;
	if (n1 > max_len) n1 = max_len;
    }
    if (n2 == npos)
	n2 = str.d->len - pos2;
    else {
	int max_len = str.d->len - pos2;
	if (n2 > max_len) n2 = max_len;
    }
	
    register int oldlen = d->len;
    register int newln = oldlen - n1 + n2;
    int taillen = oldlen - pos1 - n1;

    if (d->refc > 1 || newln >= d->max_size) {
	register wstring::Srep_LUCENT *x = wstring::Srep_LUCENT::new_srep(newln);
	if (pos1 > 0) wstring::traits_type::copy(x->str, d->str, pos1);
	if (n2 > 0) wstring::traits_type::copy(x->str+pos1, str.d->str+pos2, n2);
	if (taillen != 0) wstring::traits_type::copy(x->str+pos1+n2, d->str+pos1+n1, taillen);
	d->rcdec();
	d = x;
	return (*this);
    }
    wchar_t *tmp;
    if (taillen > 0) {
	tmp = new wchar_t[taillen];
	wstring::traits_type::copy(tmp, d->str+pos1+n1, taillen);
    }
    if (n2 > 0) wstring::traits_type::copy(d->str+pos1, str.d->str+pos2, n2);
    if (taillen > 0) {
	wstring::traits_type::copy(d->str+pos1+n2, tmp, taillen);
	delete tmp;
    }
    d->len = newln;
    return (*this);
}
wstring&
wstring::replace(size_t pos, size_t n1, const wchar_t *s, size_t n2) {
    if (pos > d->len)
#if defined(EXCEPTION_LUCENT)
	throw out_of_range(string("in replace"));
#else
	pos = d->len;
#endif
    if (n1 == npos)
	n1 = d->len - pos;
    else {
	int max_len = d->len - pos;
	if (n1 > max_len) n1 = max_len;
    }
	
    register int oldlen = d->len;
    register int newln = oldlen - n1 + n2;
    int taillen = oldlen - pos - n1;

    if (d->refc > 1 || newln >= d->max_size) {
	register wstring::Srep_LUCENT *x = wstring::Srep_LUCENT::new_srep(newln);
	if (pos > 0) wstring::traits_type::copy(x->str, d->str, pos);
	if (n2 > 0) wstring::traits_type::copy(x->str+pos, s, n2);
	if (taillen != 0) wstring::traits_type::copy(x->str+pos+n2, d->str+pos+n1, taillen);
	d->rcdec();
	d = x;
	return (*this);
    }
    wchar_t *tmp;
    if (taillen > 0) {
	tmp = new wchar_t[taillen];
	wstring::traits_type::copy(tmp, d->str+pos+n1, taillen);
    }
    if (n2 > 0) wstring::traits_type::copy(d->str+pos, s, n2);
    if (taillen > 0) {
	wstring::traits_type::copy(d->str+pos+n2, tmp, taillen);
	delete tmp;
    }
    d->len = newln;
    return (*this);
}
wstring&
wstring::replace(size_t pos, size_t n1, const wchar_t *s) {
    return (replace(pos, n1, s, wstring::traits_type::length(s)));
}
wstring&
wstring::replace(size_t pos, size_t n, wchar_t c) {
    if (pos > d->len)
#if defined(EXCEPTION_LUCENT)
	throw out_of_range(string("in replace"));
#else
	pos = d->len;
#endif
    if (n == npos)
	n = d->len - pos;
    else {
	int max_len = d->len - pos;
	if (n > max_len) n = max_len;
    }
    if (n == 0)  // do nothing
	return (*this);
	
    register int oldlen = d->len;
    int taillen = oldlen - pos - n;

    if (d->refc > 1) {
	register wstring::Srep_LUCENT *x = wstring::Srep_LUCENT::new_srep(oldlen);
	if (pos > 0) wstring::traits_type::copy(x->str, d->str, pos);
	memset(x->str+pos, c, n);
	if (taillen != 0) wstring::traits_type::copy(x->str+pos+n, d->str+pos+n, taillen);
	d->rcdec();
	d = x;
	return (*this);
    }
    memset(d->str+pos, c, n);
    return (*this);
}
wstring&
wstring::replace(iterator first, iterator last, const wstring &str) {
    int n2 = str.d->len;
	
    register int oldlen = d->len;
    register int newln = oldlen - (last - first) + n2;
    int taillen = oldlen - (last - d->str);
    int pos = first - d->str;

    if (d->refc > 1 || newln >= d->max_size) {
	register wstring::Srep_LUCENT *x =
	    wstring::Srep_LUCENT::new_srep(newln);
	if (pos > 0) wstring::traits_type::copy(x->str, d->str, pos);
	if (n2 > 0) wstring::traits_type::copy(x->str+pos, str.d->str, n2);
	if (taillen != 0) wstring::traits_type::copy(x->str+pos+n2, last, taillen);
	d->rcdec();
	d = x;
	return (*this);
    }
    wchar_t *tmp;
    if (taillen > 0) {
	tmp = new wchar_t[taillen];
	wstring::traits_type::copy(tmp, last, taillen);
    }
    if (n2 > 0) wstring::traits_type::copy(d->str+pos, str.d->str, n2);
    if (taillen > 0) {
	wstring::traits_type::copy(d->str+pos+n2, tmp, taillen);
	delete tmp;
    }
    d->len = newln;
    return (*this);
}
wstring&
wstring::replace(iterator first, iterator last, const wchar_t *s, size_t n) {
    register int oldlen = d->len;
    register int newln = oldlen - (last - first) + n;
    int taillen = oldlen - (last - d->str);
    int pos = first - d->str;

    if (d->refc > 1 || newln >= d->max_size) {
	register wstring::Srep_LUCENT *x =
	    wstring::Srep_LUCENT::new_srep(newln);
	if (pos > 0) wstring::traits_type::copy(x->str, d->str, pos);
	if (n > 0) wstring::traits_type::copy(x->str+pos, s, n);
	if (taillen != 0) wstring::traits_type::copy(x->str+pos+n, last, taillen);
	d->rcdec();
	d = x;
	return (*this);
    }
    wchar_t *tmp;
    if (taillen > 0) {
	tmp = new wchar_t[taillen];
	wstring::traits_type::copy(tmp, last, taillen);
    }
    if (n > 0) wstring::traits_type::copy(d->str+pos, s, n);
    if (taillen > 0) {
	wstring::traits_type::copy(d->str+pos+n, tmp, taillen);
	delete tmp;
    }
    d->len = newln;
    return (*this);
}
wstring&
wstring::replace(iterator first, iterator last, const wchar_t *s) {
    return(replace(first, last, s, wstring::traits_type::length(s)));
}
wstring&
wstring::replace(iterator first, iterator last, wchar_t c) {
    int n = last - first;
    if (n == 0)  // do nothing
	return (*this);
	
    register int oldlen = d->len;
    int taillen = oldlen - (last - d->str);
    int pos = first - d->str;

    if (d->refc > 1) {
	register wstring::Srep_LUCENT *x = wstring::Srep_LUCENT::new_srep(oldlen);
	if (pos > 0) wstring::traits_type::copy(x->str, d->str, pos);
	memset(x->str+pos, c, n);
	if (taillen != 0) wstring::traits_type::copy(x->str+pos+n, last, taillen);
	d->rcdec();
	d = x;
	return (*this);
    }
    memset(d->str+pos, c, n);
    return (*this);
}

void
wstring::reserve_grow(int target)
{
    wstring::Srep_LUCENT *x = wstring::Srep_LUCENT::new_srep(target);
    x->len = d->len;
    if (d->len != 0) {
	wstring::traits_type::copy(x->str, d->str, d->len);
    }
    d->rcdec();
    d = x;
}

#if 1
wstring::wstring(const wstring& s, size_t pos, size_t n) {
#else
wstring::basic_string(const wstring& s, size_t pos, size_t n) {
#endif
    if (pos > s.d->len) {
#if defined(EXCEPTION_LUCENT)
	throw out_of_range(string("in constructor"));
#else
	pos = s.d->len;
#endif
    }
    if (pos == 0 && (n == npos || s.d->len)) {
	d = s.d;
	d->rcinc();
    }
    else {
	if (n == npos) n = s.d->len - pos;
	d = wstring::Srep_LUCENT::new_srep(n);
	if (n != 0) wstring::traits_type::copy(d->str, s.d->str + pos, n);
    }
}

#if 1
wstring::wstring(const wchar_t *st, unsigned length)
#else
wstring::basic_string(const wchar_t *st, unsigned length)
#endif
{
    if (!st) length=0;
    d = wstring::Srep_LUCENT::new_srep(length);
    if (length) wstring::traits_type::copy(d->str, st, length);
}

#if 1
wstring::wstring(const wchar_t *st)
#else
wstring::basic_string(const wchar_t *st)
#endif
{
    register int ln = st ? wstring::traits_type::length(st) : 0;
    d = wstring::Srep_LUCENT::new_srep(ln);
    if (ln) wstring::traits_type::copy(d->str, st, ln);
}

#if 1
wstring::wstring(size_t n, wchar_t c) {
#else
wstring::basic_string(size_t n, wchar_t c) {
#endif
#if defined(EXCEPTION_LUCENT)
    if (n == npos) throw length_error(string("in constructor"));
#endif
    d = wstring::Srep_LUCENT::new_srep(n);
    if (n != 0) memset(d->str, c, n);
}

/* SUBCHAR STUFF */
wstring::reference
wstring::operator[](size_t i)
{
    if (d->refc!=1) {
	register wstring::Srep_LUCENT* r = wstring::Srep_LUCENT::new_srep(length());
	if (length() > 0)
	    wstring::traits_type::copy(r->str,d->str,length());
	if (d->refc > 0)
	    --(d->refc);
	d = r;
    }
    return d->str[i];
}
wstring::reference
wstring::at(size_t i)
{
#if defined(EXCEPTION_LUCENT)
    if (i >= d->len) throw out_of_range(string("in at()"));
#endif
    if (d->refc != 1) {
	register wstring::Srep_LUCENT* r = wstring::Srep_LUCENT::new_srep(length());
	if (length() > 0)
	    wstring::traits_type::copy(r->str,d->str,length());
	if (d->refc > 0)
	    --(d->refc);
	d = r;
    }
    return d->str[i];
}

wstring
wstring::substr(size_t pos, size_t n) const {
    if (n == npos) {
	n = d->len - pos;
    }
    if (n == d->len && pos == 0) {
	return (*this);	// the copy constructor will avoid
			// making a new string representation
    }
    register wchar_t *sp = d->str + pos;
    return wstring(sp, n);
}
Pool_std* wstring::Srep_LUCENT::Reelp = 0;
Pool_std* wstring::Srep_LUCENT::SPoolp = 0;
wstring::Srep_LUCENT* wstring::Srep_LUCENT::nullrep_ = 0;

void
wstring::Srep_LUCENT::doinitialize() 
{
    Reelp = new Pool_std(sizeof(wstring::Srep_LUCENT));
    SPoolp = new Pool_std(sizeof(wstring::Srep_LUCENT) + sizeof(wchar_t)*(MAXL - MAXS));
    nullrep_ = new wstring::Srep_LUCENT();
}

wstring::Srep_LUCENT *
wstring::Srep_LUCENT::new_srep(size_t length)
{ 
    initialize();
    register wstring::Srep_LUCENT* x;
    if (length == 0) return nullrep_;
    if (length >= MAXL) {
	 x = get_long(length);
    }
    else {
	if (length < MAXS) 
		x = (wstring::Srep_LUCENT*) Reelp->alloc();
	else
		x = (wstring::Srep_LUCENT*) SPoolp->alloc();
	x->len = length;
	x->refc = 1;
	x->max_size = length < MAXS ? MAXS : MAXL;
    }
    return x;
}

wstring::Srep_LUCENT *
wstring::Srep_LUCENT::get_long(size_t length) 
{ 
    register int m = 128;
    while (m <= length) m <<= 1;

    register wstring::Srep_LUCENT *x =
	(wstring::Srep_LUCENT *) new char[sizeof(wchar_t)*m + sizeof(wstring::Srep_LUCENT) - sizeof(wchar_t)*MAXS]; 
    x->len = length;
    x->max_size = m;
    x->refc = 1;
    return x;
}

void 
wstring::Srep_LUCENT::delete_srep() 
{ 
    if (max_size==0) 
	; 
    else if (max_size <= MAXS) Reelp->free((char*) this); 
    else if (max_size <= MAXL) SPoolp->free((char*) this); 
    else delete (char *) this; 
}

#ifdef NAMESPACES_LUCENT
}
#endif
