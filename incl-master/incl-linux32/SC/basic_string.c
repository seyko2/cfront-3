/*ident	"@(#) @(#)basic_string.c	1.1.1.2" */
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

// Addition:

#ifdef NAMESPACES_LUCENT
namespace std {
#endif
template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>
operator+(const basic_string<charT,traits,Alloc>& s, const basic_string<charT,traits,Alloc>& r)
{
    register int slen = s.d->len;
    register int rlen = r.d->len;

    if(rlen == 0) return basic_string<charT,traits,Alloc>(s);
    if(slen == 0) return basic_string<charT,traits,Alloc>(r);

    register int newln = rlen + slen;
    register basic_string<charT,traits,Alloc>::Srep_LUCENT* rd =
	basic_string<charT,traits,Alloc>::Srep_LUCENT::new_srep(newln);

    traits::copy(rd->str, s.d->str, slen);
    traits::copy(rd->str+slen, r.d->str, rlen);

    return basic_string<charT,traits,Alloc>(rd);
}

template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>
operator+(const basic_string<charT,traits,Alloc>& s, const charT *st)
{
    register int slen = s.d->len;
    register int ln = st ? traits::length(st) : 0;

    if(ln==0) return basic_string<charT,traits,Alloc>(s);

    register int newln = ln + slen;
    register basic_string<charT,traits,Alloc>::Srep_LUCENT* rd =
	basic_string<charT,traits,Alloc>::Srep_LUCENT::new_srep(newln);

    traits::copy(rd->str, s.d->str, slen);
    traits::copy(rd->str+slen, st, ln);

    return basic_string<charT,traits,Alloc>(rd);
}

template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>
operator+(const charT *st, const basic_string<charT,traits,Alloc>& s)
{
    register int ln = st ? traits::length(st) : 0;
    register int slen = s.d->len;

    if(ln==0) return basic_string<charT,traits,Alloc>(s);

    register int newln = ln + slen;
    register basic_string<charT,traits,Alloc>::Srep_LUCENT* rd = basic_string<charT,traits,Alloc>::Srep_LUCENT::new_srep(newln);

    traits::copy(rd->str, st, ln);
    traits::copy(rd->str+ln, s.d->str, slen);

    return basic_string<charT,traits,Alloc>(rd);
}

template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>
operator+(const basic_string<charT,traits,Alloc>& s, charT c)
{
    register int slen = s.d->len;

    register int newln = 1 + slen;
    register basic_string<charT,traits,Alloc>::Srep_LUCENT* rd = basic_string<charT,traits,Alloc>::Srep_LUCENT::new_srep(newln);

    traits::copy(rd->str, s.d->str, slen);
    rd->str[slen] = c;

    return basic_string<charT,traits,Alloc>(rd);
}

template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>
operator+(charT c, const basic_string<charT,traits,Alloc>& s)
{
    register int slen = s.d->len;

    register int newln = 1 + slen;
    register basic_string<charT,traits,Alloc>::Srep_LUCENT* rd = basic_string<charT,traits,Alloc>::Srep_LUCENT::new_srep(newln);

    traits::copy(rd->str+1, s.d->str, slen);
    rd->str[0] = c;

    return basic_string<charT,traits,Alloc>(rd);
}

// Appending:

template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>&
basic_string<charT,traits,Alloc>::newcopy(charT c)
{
    register int oldlen = d->len;
    register basic_string<charT,traits,Alloc>::Srep_LUCENT *x = basic_string<charT,traits,Alloc>::Srep_LUCENT::new_srep((oldlen+1));
    if (oldlen > 0) traits::copy(x->str, d->str, oldlen);
    x->str[oldlen] = c;
    d->rcdec();
    d = x;
    return *this;
}

template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>&
basic_string<charT,traits,Alloc>::append(const basic_string<charT,traits,Alloc> &s, size_t pos, size_t n) {
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
	register basic_string<charT,traits,Alloc>::Srep_LUCENT *x = basic_string<charT,traits,Alloc>::Srep_LUCENT::new_srep(newln);
	if (oldlen > 0) traits::copy(x->str, d->str, oldlen);
	traits::copy(x->str+oldlen, s.d->str+pos, n);
	d->rcdec();
	d = x;
	return (*this);
    }
    traits::copy(d->str+oldlen, s.d->str+pos, n);
    d->len += n;
    return (*this);
}

template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>&
basic_string<charT,traits,Alloc>::append(const charT *s, size_t ln)
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
	register basic_string<charT,traits,Alloc>::Srep_LUCENT *x = basic_string<charT,traits,Alloc>::Srep_LUCENT::new_srep(newln);
	if (oldlen > 0) traits::copy(x->str, d->str, oldlen);
	if (ln > 0) traits::copy(x->str+oldlen, s, ln);
	d->rcdec();
	d = x;
	return (*this);
    }
    if (ln > 0) traits::copy(d->str+oldlen, s, ln);
    d->len += ln;
    return (*this);
}
template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>&
basic_string<charT,traits,Alloc>::append(const charT *s) {
    return (append(s, traits::length(s)));
}
template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>&
basic_string<charT,traits,Alloc>::append(size_t n, charT c) {
    if (n == 0) 
	return (*this);

    register int oldlen = d->len;
    register int newln = n + oldlen;

    if (d->refc > 1 || newln >= d->max_size) {
	register basic_string<charT,traits,Alloc>::Srep_LUCENT *x = basic_string<charT,traits,Alloc>::Srep_LUCENT::new_srep(newln);
	if (oldlen > 0) traits::copy(x->str, d->str, oldlen);
	memset(x->str+oldlen, c, n);
	d->rcdec();
	d = x;
	return (*this);
    }
    memset(d->str+oldlen, c, n);
    d->len += n;
    return (*this);
}

template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>&
basic_string<charT,traits,Alloc>::operator+=(const charT * s)
{
    register int ln = s ? traits::length(s) : 0;
    append(s, ln);
    return *this;
}

template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>&
basic_string<charT,traits,Alloc>::operator+=(const basic_string<charT,traits,Alloc>& s)
{
    append(s.d->str, s.d->len);
    return *this;
}

// STRING OPERATORS:

// Assignment:

template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>&
basic_string<charT,traits,Alloc>::assign(const basic_string<charT,traits,Alloc> &s, size_t pos, size_t n) {
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
	d = basic_string<charT,traits,Alloc>::Srep_LUCENT::new_srep(n);
    }
    d->len = n;
    if (n != 0) traits::copy(d->str, s.d->str + pos, n);
    return (*this);
}

template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>&
basic_string<charT,traits,Alloc>::assign(const charT *s, size_t ln)
{
    if (ln == 1) {  // optimization
	*this = *s;
	return (*this);
    }
    if (d->refc > 1 || ln >= d->max_size) {
	d->rcdec();
	d = basic_string<charT,traits,Alloc>::Srep_LUCENT::new_srep(ln);
    }
    d->len = ln;
    if (ln > 0) traits::copy(d->str,s,ln);
    return (*this);
}

template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>&
basic_string<charT,traits,Alloc>::assign(const charT *s) {
    return (assign(s, traits::length(s)));
}

template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>&
basic_string<charT,traits,Alloc>::assign(size_t n, charT c) {
    if (d->refc > 1 || n >= d->max_size) {
	d->rcdec();
	d = basic_string<charT,traits,Alloc>::Srep_LUCENT::new_srep(n);
    }
    d->len = n;
    if (n != 0) memset(d->str,c,n);
    return (*this);
}

template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>&
basic_string<charT,traits,Alloc>::operator=(const charT *s)
{
    register int ln = s ? traits::length(s) : 0;
    assign(s, ln);
    return *this;
}

/* position of first occurrence of charT */
template<class charT,class traits,class Alloc>
size_t
basic_string<charT,traits,Alloc>::find(charT c, size_t pos) const
{
    register charT *p = d->str - 1 + pos;
    register charT *q = d->str + d->len;
    while (++p < q)
        if (traits::eq(*p, c))
            return p - d->str;
    return npos;
}

template<class charT,class traits,class Alloc>
size_t
basic_string<charT,traits,Alloc>::find(const basic_string<charT,traits,Alloc>& pattern, size_t pos) const
{
    if (pos == npos) pos = d->len;
    register int plen = pattern.d->len;
    register int slen = d->len;
    register const charT *pp = pattern.d->str;
    register const charT *sbp = d->str + pos;
    register const charT *sep = d->str + slen;

    int i;
    for (i=pos; sbp<sep-plen+1; i++) {
        if (traits::compare(sbp++, pp, plen) == 0) return i;
    }
    return npos;
}
template<class charT,class traits,class Alloc>
size_t
basic_string<charT,traits,Alloc>::find(const charT *s, size_t pos, size_t n) const
{
    register int plen = n;
    register int slen = d->len;
    register const charT *pp = s;
    register const charT *sbp = d->str + pos;
    register const charT *sep = d->str + slen;

    int i;
    for (i=pos; sbp<sep-plen+1; i++) {
        if (traits::compare(sbp++, pp, plen) == 0) return i;
    }
    return npos;
}
template<class charT,class traits,class Alloc>
size_t
basic_string<charT,traits,Alloc>::find(const charT *s, size_t pos) const
{
    register int plen = traits::length(s);
    register int slen = d->len;
    register const charT *pp = s;
    register const charT *sbp = d->str + pos;
    register const charT *sep = d->str + slen;

    int i;
    for (i=pos; sbp<sep-plen+1; i++) {
        if (traits::compare(sbp++, pp, plen) == 0) return i;
    }
    return npos;
}

/* ... last ... (npos if charT not there) */
template<class charT,class traits,class Alloc>
size_t
basic_string<charT,traits,Alloc>::rfind(charT c, size_t pos) const
{
    register charT *p;
    if (pos == npos)
	p = d->str + d->len;
    else
	p = d->str + pos;
    register charT *q = d->str;
    while (--p >= q)
        if (traits::eq(*p, c))
            return p - d->str;
    return npos;
}
template<class charT,class traits,class Alloc>
size_t
basic_string<charT,traits,Alloc>::rfind(const basic_string<charT,traits,Alloc>& pattern, size_t pos) const
{
    if (pos == npos) pos = d->len;
    register int plen = pattern.d->len;
    register int slen = d->len;
    register const charT *pp = pattern.d->str;
    register const charT *sep = d->str + pos;
    register const charT *sbp = d->str + slen - plen;

    int i;
    for(i=slen-plen; sbp>=sep; i--) {
        if (traits::compare(sbp--, pp, plen) == 0) return i;
    }
    return npos;
}
template<class charT,class traits,class Alloc>
size_t
basic_string<charT,traits,Alloc>::rfind(const charT *s, size_t pos, size_t n) const
{
    register int plen = n;
    register int slen = d->len;
    register const charT *pp = s;
    register const charT *sep = d->str + pos;
    register const charT *sbp = d->str + slen - plen;

    int i;
    for (i=slen-plen; sbp>=sep; i--) {
        if(traits::compare(sbp--, pp, plen) == 0) return i;
    }
    return npos;
}
template<class charT,class traits,class Alloc>
size_t
basic_string<charT,traits,Alloc>::rfind(const charT *s, size_t pos) const
{
    register int plen = traits::length(s);
    register int slen = d->len;
    register const charT *pp = s;
    register const charT *sep = d->str + pos;
    register const charT *sbp = d->str + slen - plen;

    int i;
    for (i=slen-plen; sbp>=sep; i--) {
        if (traits::compare(sbp--, pp, plen) == 0) return i;
    }
    return npos;
}

template<class charT,class traits,class Alloc>
size_t
basic_string<charT,traits,Alloc>::find_first_of(const basic_string<charT,traits,Alloc> &s, size_t pos) const
{
    register const charT *p, *q;
    const basic_string<charT,traits,Alloc> tmpstring(s);
    for (p = d->str-1+pos, q=d->str+d->len; ++p < q; )
	if (tmpstring.find(*p) != npos)
	    return p - d->str;
    return npos;
}
template<class charT,class traits,class Alloc>
size_t
basic_string<charT,traits,Alloc>::find_first_of(const charT *s, size_t pos, size_t n) const
{
    register const charT *p, *q;
    const basic_string<charT,traits,Alloc> tmpstring(s, n);
    for (p = d->str-1+pos, q=d->str+d->len; ++p < q; )
        if (tmpstring.find(*p) != npos)
            return p - d->str;
    return npos;
}
template<class charT,class traits,class Alloc>
size_t
basic_string<charT,traits,Alloc>::find_first_of(const charT *s, size_t pos) const
{
    register const charT *p, *q;
    const basic_string<charT,traits,Alloc> tmpstring(s);
    for (p = d->str-1+pos, q=d->str+d->len; ++p < q; )
        if (tmpstring.find(*p) != npos)
            return p - d->str;
    return npos;
}
template<class charT,class traits,class Alloc>
size_t
basic_string<charT,traits,Alloc>::find_first_of(charT c, size_t pos) const {
    return (find(c, pos));
}
template<class charT,class traits,class Alloc>
size_t
basic_string<charT,traits,Alloc>::find_last_of(const basic_string<charT,traits,Alloc> &s, size_t pos) const
{
    register const charT *p, *q;
    const basic_string<charT,traits,Alloc> tmpstring(s);
    for (p = d->str+d->len+1, q=d->str+pos; --p >= q; )
	if (tmpstring.find(*p) != npos)
	    return p - d->str;
    return npos;
}
template<class charT,class traits,class Alloc>
size_t
basic_string<charT,traits,Alloc>::find_last_of(const charT *s, size_t pos, size_t n) const
{
    register const charT *p, *q;
    const basic_string<charT,traits,Alloc> tmpstring(s, n);
    for (p = d->str+d->len+1, q=d->str+pos; --p >= q; )
	if (tmpstring.find(*p) != npos)
	    return p - d->str;
    return npos;
}
template<class charT,class traits,class Alloc>
size_t
basic_string<charT,traits,Alloc>::find_last_of(const charT *s, size_t pos) const
{
    register const charT *p, *q;
    const basic_string<charT,traits,Alloc> tmpstring(s);
    for (p = d->str+d->len+1, q=d->str+pos; --p >= q; )
	if (tmpstring.find(*p) != npos)
	    return p - d->str;
    return npos;
}
template<class charT,class traits,class Alloc>
size_t
basic_string<charT,traits,Alloc>::find_last_of(charT c, size_t pos) const {
    return (rfind(c, pos));
}

template<class charT,class traits,class Alloc>
size_t
basic_string<charT,traits,Alloc>::find_first_not_of(const basic_string<charT,traits,Alloc> &s, size_t pos) const
{
    register const charT *p, *q;
    const basic_string<charT,traits,Alloc> tmpstring(s);
    for (p = d->str-1+pos, q=d->str+d->len; ++p < q; )
	if (!tmpstring.find(*p) != npos)
	    return p - d->str;
    return npos;
}
template<class charT,class traits,class Alloc>
size_t
basic_string<charT,traits,Alloc>::find_first_not_of(const charT *s, size_t pos, size_t n) const
{
    register const charT *p, *q;
    const basic_string<charT,traits,Alloc> tmpstring(s, n);
    for (p = d->str-1+pos, q=d->str+d->len; ++p < q; )
	if (!tmpstring.find(*p) != npos)
	    return p - d->str;
    return npos;
}
template<class charT,class traits,class Alloc>
size_t
basic_string<charT,traits,Alloc>::find_first_not_of(const charT *s, size_t pos) const
{
    register const charT *p, *q;
    const basic_string<charT,traits,Alloc> tmpstring(s);
    for (p = d->str-1+pos, q=d->str+d->len; ++p < q; )
	if (!tmpstring.find(*p) != npos)
	    return p - d->str;
    return npos;
}
template<class charT,class traits,class Alloc>
size_t
basic_string<charT,traits,Alloc>::find_first_not_of(charT c, size_t pos) const
{
    register const charT    *p, *q;
    for (p = d->str-1+pos, q=d->str+d->len; ++p < q; )
        if (!traits::eq(*p, c))
            return p - d->str;
    return npos;
}
template<class charT,class traits,class Alloc>
size_t
basic_string<charT,traits,Alloc>::find_last_not_of(const basic_string<charT,traits,Alloc> &s, size_t pos) const
{
    register const charT *p, *q;
    const basic_string<charT,traits,Alloc> tmpstring(s);
    for (p = d->str+d->len, q=d->str+pos; --p >= q; )
	if (!tmpstring.find(*p) != npos)
	    return p - d->str;
    return npos;
}
template<class charT,class traits,class Alloc>
size_t
basic_string<charT,traits,Alloc>::find_last_not_of(const charT *s, size_t pos, size_t n) const
{
    register const charT *p, *q;
    const basic_string<charT,traits,Alloc> tmpstring(s, n);
    for (p = d->str+d->len, q=d->str+pos; --p >= q; )
	if (!tmpstring.find(*p) != npos)
	    return p - d->str;
    return npos;
}
template<class charT,class traits,class Alloc>
size_t
basic_string<charT,traits,Alloc>::find_last_not_of(const charT *s, size_t pos) const
{
    register const charT *p, *q;
    const basic_string<charT,traits,Alloc> tmpstring(s);
    for (p = d->str+d->len, q=d->str+pos; --p >= q; )
	if (!tmpstring.find(*p) != npos)
	    return p - d->str;
    return npos;
}
template<class charT,class traits,class Alloc>
size_t
basic_string<charT,traits,Alloc>::find_last_not_of(charT c, size_t pos) const
{
    register charT    *p, *q;
    for (p = d->str+d->len, q=d->str+pos; --p >= q; )
        if (!traits::eq(*p, c))
            return p - d->str;
    return npos;
}

// Inserting:

template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>&
basic_string<charT,traits,Alloc>::insert(size_t pos1, const basic_string<charT,traits,Alloc> &str, size_t pos2, size_t n) {
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
	register basic_string<charT,traits,Alloc>::Srep_LUCENT *x = basic_string<charT,traits,Alloc>::Srep_LUCENT::new_srep(newln);
	if (pos1 > 0) traits::copy(x->str, d->str, pos1);
	traits::copy(x->str+pos1, str.d->str+pos2, n);
	traits::copy(x->str+pos1+n, d->str+pos1, oldlen-pos1);
	d->rcdec();
	d = x;
	return (*this);
    }
    int diff = oldlen - pos1;
    charT *tmp;
    if (diff > 0) {
	tmp = new charT[diff];
	traits::copy(tmp, d->str+pos1, diff);
    }
    traits::copy(d->str+pos1, str.d->str+pos2, n);
    if (diff > 0) {
	traits::copy(d->str+pos1+n, tmp, diff);
	delete tmp;
    }
    d->len += n;
    return (*this);
}
template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>&
basic_string<charT,traits,Alloc>::insert(size_t pos, const charT *s, size_t n)
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
	register basic_string<charT,traits,Alloc>::Srep_LUCENT *x = basic_string<charT,traits,Alloc>::Srep_LUCENT::new_srep(newln);
	if (pos > 0) traits::copy(x->str, d->str, pos);
	traits::copy(x->str+pos, s, n);
	traits::copy(x->str+pos+n, d->str+pos, oldlen-pos);
	d->rcdec();
	d = x;
	return (*this);
    }
    int diff = oldlen - pos;
    charT *tmp;
    if (diff > 0) {
	tmp = new charT[diff];
	traits::copy(tmp, d->str+pos, diff);
    }
    traits::copy(d->str+pos, s, n);
    if (diff > 0) {
	traits::copy(d->str+pos+n, tmp, diff);
	delete tmp;
    }
    d->len += n;
    return (*this);
}
template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>&
basic_string<charT,traits,Alloc>::insert(size_t pos, const charT *s) {
    return (insert(pos, s, traits::length(s)));
}
template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>&
basic_string<charT,traits,Alloc>::insert(size_t pos, size_t n, charT c) {
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
	register basic_string<charT,traits,Alloc>::Srep_LUCENT *x = basic_string<charT,traits,Alloc>::Srep_LUCENT::new_srep(newln);
	if (pos > 0) traits::copy(x->str, d->str, pos);
	memset(x->str+pos, c, n);
	traits::copy(x->str+pos+n, d->str+pos, oldlen-pos);
	d->rcdec();
	d = x;
	return (*this);
    }
    int diff = oldlen - pos;
    charT *tmp;
    if (diff > 0) {
	tmp = new charT[diff];
	traits::copy(tmp, d->str+pos, diff);
    }
    memset(d->str+pos, c, n);
    if (diff > 0) {
	traits::copy(d->str+pos+n, tmp, diff);
	delete tmp;
    }
    d->len += n;
    return (*this);
}
template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>::iterator
basic_string<charT,traits,Alloc>::insert(iterator p, charT c) {
    return (insert(p, 1, c));
}
template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>::iterator
basic_string<charT,traits,Alloc>::insert(iterator p, size_t n, charT c) {
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
	register basic_string<charT,traits,Alloc>::Srep_LUCENT *x = basic_string<charT,traits,Alloc>::Srep_LUCENT::new_srep(newln);
	if (pos > 0) traits::copy(x->str, d->str, pos);
	memset(x->str+pos, c, n);
	traits::copy(x->str+pos+n, d->str+pos, oldlen-pos);
	d->rcdec();
	d = x;
	retval = x->str + pos;
	return (retval);
    }
    int diff = oldlen - pos;
    charT *tmp;
    if (diff > 0) {
	tmp = new charT[diff];
	traits::copy(tmp, d->str+pos, diff);
    }
    memset(d->str+pos, c, n);
    if (diff > 0) {
	traits::copy(d->str+pos+n, tmp, diff);
	delete tmp;
    }
    d->len += n;
    return (retval);
}

template<class charT,class traits,class Alloc>
const size_t basic_string<charT,traits,Alloc>::npos = (size_t) -1;

template<class charT,class traits,class Alloc>
size_t
basic_string<charT,traits,Alloc>::copy(charT *s, size_t n, size_t pos) {
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
    traits::copy(s, d->str+pos, n); 
    return (n);
}

template<class charT,class traits,class Alloc>
void
basic_string<charT,traits,Alloc>::swap(basic_string<charT,traits,Alloc> &s) {
    basic_string<charT,traits,Alloc>::Srep_LUCENT *p = s.d;
    s.d = this->d;
    this->d = p;
}

template<class charT,class traits,class Alloc>
int
basic_string<charT,traits,Alloc>::compare(const basic_string<charT,traits,Alloc>& s, size_t pos, size_t n) const {
	int retval;
	if (pos == npos)
		pos = d->len;
	register int dlen = d->len - pos;
	register int slen = s.d->len;
	if (n == npos || n > dlen || n > slen)
		n = (dlen < slen) ? dlen : slen;
	retval = traits::compare(d->str + pos, s.d->str, n);
	if (retval == 0)
		retval = dlen - slen;
	return retval;
}

template<class charT,class traits,class Alloc>
int
basic_string<charT,traits,Alloc>::compare(const charT *s, size_t pos, size_t n) const {
	int retval;
	if (pos == npos)
		pos = d->len;
	register int dlen = d->len - pos;
	register int slen = n;
	if (n > dlen)
		n = dlen;
	retval = traits::compare(d->str + pos, s, n);
	if (retval == 0)
		retval = dlen - slen;
	return retval;
}

template<class charT,class traits,class Alloc>
int
basic_string<charT,traits,Alloc>::compare(const charT *s, size_t pos) const {
	register int dlen = d->len - pos;
	register int n = s ? traits::length(s) : 0;
	register int slen = n;
	if (n > dlen)
		n = dlen;
	int retval = traits::compare(d->str + pos, s, n);
	if (retval == 0)
		retval = dlen - slen;
	return retval;
}
template<class charT,class traits,class Alloc>
void
basic_string<charT,traits,Alloc>::resize(size_t n, charT c) {
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
	register basic_string<charT,traits,Alloc>::Srep_LUCENT *x = basic_string<charT,traits,Alloc>::Srep_LUCENT::new_srep(n);
	traits::copy(x->str, d->str, copycount);
	d->rcdec();
	d = x;
    }
    if (n > copycount) memset(d->str + copycount, c, n - copycount);
    d->len = n;
}

// Removing:

template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>&
basic_string<charT,traits,Alloc>::remove(size_t pos, size_t n) {
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
    if (pos == 0 && n == d->len)  // clear the entire basic_string<charT,traits,Alloc>
	return (assign(d->str, 0));
	
    register int newln = d->len - n;

    if (d->refc > 1) {
	register basic_string<charT,traits,Alloc>::Srep_LUCENT *x = basic_string<charT,traits,Alloc>::Srep_LUCENT::new_srep(newln);
	if (pos > 0) traits::copy(x->str, d->str, pos);
	traits::copy(x->str+pos, d->str+pos+n, newln-pos);
	d->rcdec();
	d = x;
	return (*this);
    }
    traits::copy(d->str+pos, d->str+pos+n, newln-pos);
    d->len -= n;
    return (*this);
}
template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>&
basic_string<charT,traits,Alloc>::remove(iterator p) {
    return (remove(p - d->str, 1));
}
template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>&
basic_string<charT,traits,Alloc>::remove(iterator first, iterator last) {
    return (remove(first - d->str, last - first));
}

// Removing:

template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>&
basic_string<charT,traits,Alloc>::replace(size_t pos1, size_t n1, const basic_string<charT,traits,Alloc> &str, size_t pos2, size_t n2) {
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
	register basic_string<charT,traits,Alloc>::Srep_LUCENT *x = basic_string<charT,traits,Alloc>::Srep_LUCENT::new_srep(newln);
	if (pos1 > 0) traits::copy(x->str, d->str, pos1);
	if (n2 > 0) traits::copy(x->str+pos1, str.d->str+pos2, n2);
	if (taillen != 0) traits::copy(x->str+pos1+n2, d->str+pos1+n1, taillen);
	d->rcdec();
	d = x;
	return (*this);
    }
    charT *tmp;
    if (taillen > 0) {
	tmp = new charT[taillen];
	traits::copy(tmp, d->str+pos1+n1, taillen);
    }
    if (n2 > 0) traits::copy(d->str+pos1, str.d->str+pos2, n2);
    if (taillen > 0) {
	traits::copy(d->str+pos1+n2, tmp, taillen);
	delete tmp;
    }
    d->len = newln;
    return (*this);
}
template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>&
basic_string<charT,traits,Alloc>::replace(size_t pos, size_t n1, const charT *s, size_t n2) {
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
	register basic_string<charT,traits,Alloc>::Srep_LUCENT *x = basic_string<charT,traits,Alloc>::Srep_LUCENT::new_srep(newln);
	if (pos > 0) traits::copy(x->str, d->str, pos);
	if (n2 > 0) traits::copy(x->str+pos, s, n2);
	if (taillen != 0) traits::copy(x->str+pos+n2, d->str+pos+n1, taillen);
	d->rcdec();
	d = x;
	return (*this);
    }
    charT *tmp;
    if (taillen > 0) {
	tmp = new charT[taillen];
	traits::copy(tmp, d->str+pos+n1, taillen);
    }
    if (n2 > 0) traits::copy(d->str+pos, s, n2);
    if (taillen > 0) {
	traits::copy(d->str+pos+n2, tmp, taillen);
	delete tmp;
    }
    d->len = newln;
    return (*this);
}
template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>&
basic_string<charT,traits,Alloc>::replace(size_t pos, size_t n1, const charT *s) {
    return (replace(pos, n1, s, traits::length(s)));
}
template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>&
basic_string<charT,traits,Alloc>::replace(size_t pos, size_t n, charT c) {
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
	register basic_string<charT,traits,Alloc>::Srep_LUCENT *x = basic_string<charT,traits,Alloc>::Srep_LUCENT::new_srep(oldlen);
	if (pos > 0) traits::copy(x->str, d->str, pos);
	memset(x->str+pos, c, n);
	if (taillen != 0) traits::copy(x->str+pos+n, d->str+pos+n, taillen);
	d->rcdec();
	d = x;
	return (*this);
    }
    memset(d->str+pos, c, n);
    return (*this);
}
template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>&
basic_string<charT,traits,Alloc>::replace(iterator first, iterator last, const basic_string<charT,traits,Alloc> &str) {
    int n2 = str.d->len;
	
    register int oldlen = d->len;
    register int newln = oldlen - (last - first) + n2;
    int taillen = oldlen - (last - d->str);
    int pos = first - d->str;

    if (d->refc > 1 || newln >= d->max_size) {
	register basic_string<charT,traits,Alloc>::Srep_LUCENT *x =
	    basic_string<charT,traits,Alloc>::Srep_LUCENT::new_srep(newln);
	if (pos > 0) traits::copy(x->str, d->str, pos);
	if (n2 > 0) traits::copy(x->str+pos, str.d->str, n2);
	if (taillen != 0) traits::copy(x->str+pos+n2, last, taillen);
	d->rcdec();
	d = x;
	return (*this);
    }
    charT *tmp;
    if (taillen > 0) {
	tmp = new charT[taillen];
	traits::copy(tmp, last, taillen);
    }
    if (n2 > 0) traits::copy(d->str+pos, str.d->str, n2);
    if (taillen > 0) {
	traits::copy(d->str+pos+n2, tmp, taillen);
	delete tmp;
    }
    d->len = newln;
    return (*this);
}
template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>&
basic_string<charT,traits,Alloc>::replace(iterator first, iterator last, const charT *s, size_t n) {
    register int oldlen = d->len;
    register int newln = oldlen - (last - first) + n;
    int taillen = oldlen - (last - d->str);
    int pos = first - d->str;

    if (d->refc > 1 || newln >= d->max_size) {
	register basic_string<charT,traits,Alloc>::Srep_LUCENT *x =
	    basic_string<charT,traits,Alloc>::Srep_LUCENT::new_srep(newln);
	if (pos > 0) traits::copy(x->str, d->str, pos);
	if (n > 0) traits::copy(x->str+pos, s, n);
	if (taillen != 0) traits::copy(x->str+pos+n, last, taillen);
	d->rcdec();
	d = x;
	return (*this);
    }
    charT *tmp;
    if (taillen > 0) {
	tmp = new charT[taillen];
	traits::copy(tmp, last, taillen);
    }
    if (n > 0) traits::copy(d->str+pos, s, n);
    if (taillen > 0) {
	traits::copy(d->str+pos+n, tmp, taillen);
	delete tmp;
    }
    d->len = newln;
    return (*this);
}
template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>&
basic_string<charT,traits,Alloc>::replace(iterator first, iterator last, const charT *s) {
    return(replace(first, last, s, traits::length(s)));
}
template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>&
basic_string<charT,traits,Alloc>::replace(iterator first, iterator last, charT c) {
    int n = last - first;
    if (n == 0)  // do nothing
	return (*this);
	
    register int oldlen = d->len;
    int taillen = oldlen - (last - d->str);
    int pos = first - d->str;

    if (d->refc > 1) {
	register basic_string<charT,traits,Alloc>::Srep_LUCENT *x = basic_string<charT,traits,Alloc>::Srep_LUCENT::new_srep(oldlen);
	if (pos > 0) traits::copy(x->str, d->str, pos);
	memset(x->str+pos, c, n);
	if (taillen != 0) traits::copy(x->str+pos+n, last, taillen);
	d->rcdec();
	d = x;
	return (*this);
    }
    memset(d->str+pos, c, n);
    return (*this);
}

template<class charT,class traits,class Alloc>
void
basic_string<charT,traits,Alloc>::reserve_grow(int target)
{
    basic_string<charT,traits,Alloc>::Srep_LUCENT *x = basic_string<charT,traits,Alloc>::Srep_LUCENT::new_srep(target);
    x->len = d->len;
    if (d->len != 0) {
	traits::copy(x->str, d->str, d->len);
    }
    d->rcdec();
    d = x;
}

template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>::basic_string(const basic_string<charT,traits,Alloc>& s, size_t pos, size_t n) {
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
	d = basic_string<charT,traits,Alloc>::Srep_LUCENT::new_srep(n);
	if (n != 0) traits::copy(d->str, s.d->str + pos, n);
    }
}

template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>::basic_string(const charT *st, unsigned length)
{
    if (!st) length=0;
    d = basic_string<charT,traits,Alloc>::Srep_LUCENT::new_srep(length);
    if (length) traits::copy(d->str, st, length);
}

template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>::basic_string(const charT *st)
{
    register int ln = st ? traits::length(st) : 0;
    d = basic_string<charT,traits,Alloc>::Srep_LUCENT::new_srep(ln);
    if (ln) traits::copy(d->str, st, ln);
}

template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>::basic_string(size_t n, charT c) {
#if defined(EXCEPTION_LUCENT)
    if (n == npos) throw length_error(string("in constructor"));
#endif
    d = basic_string<charT,traits,Alloc>::Srep_LUCENT::new_srep(n);
    if (n != 0) memset(d->str, c, n);
}

/* SUBCHAR STUFF */
template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>::reference
basic_string<charT,traits,Alloc>::operator[](size_t i)
{
    if (d->refc!=1) {
	register basic_string<charT,traits,Alloc>::Srep_LUCENT* r = basic_string<charT,traits,Alloc>::Srep_LUCENT::new_srep(length());
	if (length() > 0)
	    traits::copy(r->str,d->str,length());
	if (d->refc > 0)
	    --(d->refc);
	d = r;
    }
    return d->str[i];
}
template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>::reference
basic_string<charT,traits,Alloc>::at(size_t i)
{
#if defined(EXCEPTION_LUCENT)
    if (i >= d->len) throw out_of_range(string("in at()"));
#endif
    if (d->refc != 1) {
	register basic_string<charT,traits,Alloc>::Srep_LUCENT* r = basic_string<charT,traits,Alloc>::Srep_LUCENT::new_srep(length());
	if (length() > 0)
	    traits::copy(r->str,d->str,length());
	if (d->refc > 0)
	    --(d->refc);
	d = r;
    }
    return d->str[i];
}

template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>
basic_string<charT,traits,Alloc>::substr(size_t pos, size_t n) const {
    if (n == npos) {
	n = d->len - pos;
    }
    if (n == d->len && pos == 0) {
	return (*this);	// the copy constructor will avoid
			// making a new string representation
    }
    register charT *sp = d->str + pos;
    return basic_string<charT,traits,Alloc>(sp, n);
}
template<class charT,class traits,class Alloc>
Pool_std* basic_string<charT,traits,Alloc>::Srep_LUCENT::Reelp = 0;
template<class charT,class traits,class Alloc>
Pool_std* basic_string<charT,traits,Alloc>::Srep_LUCENT::SPoolp = 0;
template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>::Srep_LUCENT* basic_string<charT,traits,Alloc>::Srep_LUCENT::nullrep_ = 0;

template<class charT,class traits,class Alloc>
void
basic_string<charT,traits,Alloc>::Srep_LUCENT::doinitialize() 
{
    Reelp = new Pool_std(sizeof(basic_string<charT,traits,Alloc>::Srep_LUCENT));
    SPoolp = new Pool_std(sizeof(basic_string<charT,traits,Alloc>::Srep_LUCENT) + sizeof(wchar_t)*(MAXL - MAXS));
    nullrep_ = new basic_string<charT,traits,Alloc>::Srep_LUCENT();
}

template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>::Srep_LUCENT *
basic_string<charT,traits,Alloc>::Srep_LUCENT::new_srep(size_t length)
{ 
    initialize();
    register basic_string<charT,traits,Alloc>::Srep_LUCENT* x;
    if (length == 0) return nullrep_;
    if (length >= MAXL) {
	 x = get_long(length);
    }
    else {
	if (length < MAXS) 
		x = (basic_string<charT,traits,Alloc>::Srep_LUCENT*) Reelp->alloc();
	else
		x = (basic_string<charT,traits,Alloc>::Srep_LUCENT*) SPoolp->alloc();
	x->len = length;
	x->refc = 1;
	x->max_size = length < MAXS ? MAXS : MAXL;
    }
    return x;
}

template<class charT,class traits,class Alloc>
basic_string<charT,traits,Alloc>::Srep_LUCENT *
basic_string<charT,traits,Alloc>::Srep_LUCENT::get_long(size_t length) 
{ 
    register int m = 128;
    while (m <= length) m <<= 1;

    register basic_string<charT,traits,Alloc>::Srep_LUCENT *x =
	(basic_string<charT,traits,Alloc>::Srep_LUCENT *) new char[sizeof(wchar_t)*m + sizeof(basic_string<charT,traits,Alloc>::Srep_LUCENT) - sizeof(wchar_t)*MAXS]; 
    x->len = length;
    x->max_size = m;
    x->refc = 1;
    return x;
}

template<class charT,class traits,class Alloc>
void 
basic_string<charT,traits,Alloc>::Srep_LUCENT::delete_srep() 
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
