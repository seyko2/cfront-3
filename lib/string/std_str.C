/*ident	"@(#) @(#)std_str.c	1.1.1.2" */
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
#define STRING string
#else
#define STRING string::traits_type
#endif

// Addition:

#ifdef NAMESPACES_LUCENT
namespace std {
#endif
string
operator+(const string& s, const string& r)
{
    register int slen = s.d->len;
    register int rlen = r.d->len;

    if(rlen == 0) return string(s);
    if(slen == 0) return string(r);

    register int newln = rlen + slen;
    register string::Srep_LUCENT* rd =
	string::Srep_LUCENT::new_srep(newln);

    STRING::copy(rd->str, s.d->str, slen);
    STRING::copy(rd->str+slen, r.d->str, rlen);

    return string(rd);
}

string
operator+(const string& s, const char *st)
{
    register int slen = s.d->len;
    register int ln = st ? STRING::length(st) : 0;

    if(ln==0) return string(s);

    register int newln = ln + slen;
    register string::Srep_LUCENT* rd =
	string::Srep_LUCENT::new_srep(newln);

    STRING::copy(rd->str, s.d->str, slen);
    STRING::copy(rd->str+slen, st, ln);

    return string(rd);
}

string
operator+(const char *st, const string& s)
{
    register int ln = st ? STRING::length(st) : 0;
    register int slen = s.d->len;

    if(ln==0) return string(s);

    register int newln = ln + slen;
    register string::Srep_LUCENT* rd = string::Srep_LUCENT::new_srep(newln);

    STRING::copy(rd->str, st, ln);
    STRING::copy(rd->str+ln, s.d->str, slen);

    return string(rd);
}

string
operator+(const string& s, char c)
{
    register int slen = s.d->len;

    register int newln = 1 + slen;
    register string::Srep_LUCENT* rd = string::Srep_LUCENT::new_srep(newln);

    STRING::copy(rd->str, s.d->str, slen);
    rd->str[slen] = c;

    return string(rd);
}

string
operator+(char c, const string& s)
{
    register int slen = s.d->len;

    register int newln = 1 + slen;
    register string::Srep_LUCENT* rd = string::Srep_LUCENT::new_srep(newln);

    STRING::copy(rd->str+1, s.d->str, slen);
    rd->str[0] = c;

    return string(rd);
}

// Appending:

string&
string::newcopy(char c)
{
    register int oldlen = d->len;
    register string::Srep_LUCENT *x = string::Srep_LUCENT::new_srep((oldlen+1));
    if (oldlen > 0) string::traits_type::copy(x->str, d->str, oldlen);
    x->str[oldlen] = c;
    d->rcdec();
    d = x;
    return *this;
}

string&
string::append(const string &s, size_t pos, size_t n) {
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
	register string::Srep_LUCENT *x = string::Srep_LUCENT::new_srep(newln);
	if (oldlen > 0) string::traits_type::copy(x->str, d->str, oldlen);
	string::traits_type::copy(x->str+oldlen, s.d->str+pos, n);
	d->rcdec();
	d = x;
	return (*this);
    }
    string::traits_type::copy(d->str+oldlen, s.d->str+pos, n);
    d->len += n;
    return (*this);
}

string&
string::append(const char *s, size_t ln)
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
	register string::Srep_LUCENT *x = string::Srep_LUCENT::new_srep(newln);
	if (oldlen > 0) string::traits_type::copy(x->str, d->str, oldlen);
	if (ln > 0) string::traits_type::copy(x->str+oldlen, s, ln);
	d->rcdec();
	d = x;
	return (*this);
    }
    if (ln > 0) string::traits_type::copy(d->str+oldlen, s, ln);
    d->len += ln;
    return (*this);
}
string&
string::append(const char *s) {
    return (append(s, string::traits_type::length(s)));
}
string&
string::append(size_t n, char c) {
    if (n == 0) 
	return (*this);

    register int oldlen = d->len;
    register int newln = n + oldlen;

    if (d->refc > 1 || newln >= d->max_size) {
	register string::Srep_LUCENT *x = string::Srep_LUCENT::new_srep(newln);
	if (oldlen > 0) string::traits_type::copy(x->str, d->str, oldlen);
	memset(x->str+oldlen, c, n);
	d->rcdec();
	d = x;
	return (*this);
    }
    memset(d->str+oldlen, c, n);
    d->len += n;
    return (*this);
}

string&
string::operator+=(const char * s)
{
    register int ln = s ? string::traits_type::length(s) : 0;
    append(s, ln);
    return *this;
}

string&
string::operator+=(const string& s)
{
    append(s.d->str, s.d->len);
    return *this;
}

// STRING OPERATORS:

// Assignment:

string&
string::assign(const string &s, size_t pos, size_t n) {
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
	d = string::Srep_LUCENT::new_srep(n);
    }
    d->len = n;
    if (n != 0) string::traits_type::copy(d->str, s.d->str + pos, n);
    return (*this);
}

string&
string::assign(const char *s, size_t ln)
{
    if (ln == 1) {  // optimization
	*this = *s;
	return (*this);
    }
    if (d->refc > 1 || ln >= d->max_size) {
	d->rcdec();
	d = string::Srep_LUCENT::new_srep(ln);
    }
    d->len = ln;
    if (ln > 0) string::traits_type::copy(d->str,s,ln);
    return (*this);
}

string&
string::assign(const char *s) {
    return (assign(s, string::traits_type::length(s)));
}

string&
string::assign(size_t n, char c) {
    if (d->refc > 1 || n >= d->max_size) {
	d->rcdec();
	d = string::Srep_LUCENT::new_srep(n);
    }
    d->len = n;
    if (n != 0) memset(d->str,c,n);
    return (*this);
}

string&
string::operator=(const char *s)
{
    register int ln = s ? string::traits_type::length(s) : 0;
    assign(s, ln);
    return *this;
}

/* position of first occurrence of char */
size_t
string::find(char c, size_t pos) const
{
    register char *p = d->str - 1 + pos;
    register char *q = d->str + d->len;
    while (++p < q)
        if (string::traits_type::eq(*p, c))
            return p - d->str;
    return npos;
}

size_t
string::find(const string& pattern, size_t pos) const
{
    if (pos == npos) pos = d->len;
    register int plen = pattern.d->len;
    register int slen = d->len;
    register const char *pp = pattern.d->str;
    register const char *sbp = d->str + pos;
    register const char *sep = d->str + slen;

    int i;
    for (i=pos; sbp<sep-plen+1; i++) {
        if (string::traits_type::compare(sbp++, pp, plen) == 0) return i;
    }
    return npos;
}
size_t
string::find(const char *s, size_t pos, size_t n) const
{
    register int plen = n;
    register int slen = d->len;
    register const char *pp = s;
    register const char *sbp = d->str + pos;
    register const char *sep = d->str + slen;

    int i;
    for (i=pos; sbp<sep-plen+1; i++) {
        if (string::traits_type::compare(sbp++, pp, plen) == 0) return i;
    }
    return npos;
}
size_t
string::find(const char *s, size_t pos) const
{
    register int plen = string::traits_type::length(s);
    register int slen = d->len;
    register const char *pp = s;
    register const char *sbp = d->str + pos;
    register const char *sep = d->str + slen;

    int i;
    for (i=pos; sbp<sep-plen+1; i++) {
        if (string::traits_type::compare(sbp++, pp, plen) == 0) return i;
    }
    return npos;
}

/* ... last ... (npos if char not there) */
size_t
string::rfind(char c, size_t pos) const
{
    register char *p;
    if (pos == npos)
	p = d->str + d->len;
    else
	p = d->str + pos;
    register char *q = d->str;
    while (--p >= q)
        if (string::traits_type::eq(*p, c))
            return p - d->str;
    return npos;
}
size_t
string::rfind(const string& pattern, size_t pos) const
{
    if (pos == npos) pos = d->len;
    register int plen = pattern.d->len;
    register int slen = d->len;
    register const char *pp = pattern.d->str;
    register const char *sep = d->str + pos;
    register const char *sbp = d->str + slen - plen;

    int i;
    for(i=slen-plen; sbp>=sep; i--) {
        if (string::traits_type::compare(sbp--, pp, plen) == 0) return i;
    }
    return npos;
}
size_t
string::rfind(const char *s, size_t pos, size_t n) const
{
    register int plen = n;
    register int slen = d->len;
    register const char *pp = s;
    register const char *sep = d->str + pos;
    register const char *sbp = d->str + slen - plen;

    int i;
    for (i=slen-plen; sbp>=sep; i--) {
        if(string::traits_type::compare(sbp--, pp, plen) == 0) return i;
    }
    return npos;
}
size_t
string::rfind(const char *s, size_t pos) const
{
    register int plen = string::traits_type::length(s);
    register int slen = d->len;
    register const char *pp = s;
    register const char *sep = d->str + pos;
    register const char *sbp = d->str + slen - plen;

    int i;
    for (i=slen-plen; sbp>=sep; i--) {
        if (string::traits_type::compare(sbp--, pp, plen) == 0) return i;
    }
    return npos;
}

// Inserting:

string&
string::insert(size_t pos1, const string &str, size_t pos2, size_t n) {
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
	register string::Srep_LUCENT *x = string::Srep_LUCENT::new_srep(newln);
	if (pos1 > 0) string::traits_type::copy(x->str, d->str, pos1);
	string::traits_type::copy(x->str+pos1, str.d->str+pos2, n);
	string::traits_type::copy(x->str+pos1+n, d->str+pos1, oldlen-pos1);
	d->rcdec();
	d = x;
	return (*this);
    }
    int diff = oldlen - pos1;
    char *tmp;
    if (diff > 0) {
	tmp = new char[diff];
	string::traits_type::copy(tmp, d->str+pos1, diff);
    }
    string::traits_type::copy(d->str+pos1, str.d->str+pos2, n);
    if (diff > 0) {
	string::traits_type::copy(d->str+pos1+n, tmp, diff);
	delete tmp;
    }
    d->len += n;
    return (*this);
}
string&
string::insert(size_t pos, const char *s, size_t n)
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
	register string::Srep_LUCENT *x = string::Srep_LUCENT::new_srep(newln);
	if (pos > 0) string::traits_type::copy(x->str, d->str, pos);
	string::traits_type::copy(x->str+pos, s, n);
	string::traits_type::copy(x->str+pos+n, d->str+pos, oldlen-pos);
	d->rcdec();
	d = x;
	return (*this);
    }
    int diff = oldlen - pos;
    char *tmp;
    if (diff > 0) {
	tmp = new char[diff];
	string::traits_type::copy(tmp, d->str+pos, diff);
    }
    string::traits_type::copy(d->str+pos, s, n);
    if (diff > 0) {
	string::traits_type::copy(d->str+pos+n, tmp, diff);
	delete tmp;
    }
    d->len += n;
    return (*this);
}
string&
string::insert(size_t pos, const char *s) {
    return (insert(pos, s, string::traits_type::length(s)));
}
string&
string::insert(size_t pos, size_t n, char c) {
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
	register string::Srep_LUCENT *x = string::Srep_LUCENT::new_srep(newln);
	if (pos > 0) string::traits_type::copy(x->str, d->str, pos);
	memset(x->str+pos, c, n);
	string::traits_type::copy(x->str+pos+n, d->str+pos, oldlen-pos);
	d->rcdec();
	d = x;
	return (*this);
    }
    int diff = oldlen - pos;
    char *tmp;
    if (diff > 0) {
	tmp = new char[diff];
	string::traits_type::copy(tmp, d->str+pos, diff);
    }
    memset(d->str+pos, c, n);
    if (diff > 0) {
	string::traits_type::copy(d->str+pos+n, tmp, diff);
	delete tmp;
    }
    d->len += n;
    return (*this);
}
string::iterator
string::insert(iterator p, char c) {
    return (insert(p, 1, c));
}
string::iterator
string::insert(iterator p, size_t n, char c) {
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
	register string::Srep_LUCENT *x = string::Srep_LUCENT::new_srep(newln);
	if (pos > 0) string::traits_type::copy(x->str, d->str, pos);
	memset(x->str+pos, c, n);
	string::traits_type::copy(x->str+pos+n, d->str+pos, oldlen-pos);
	d->rcdec();
	d = x;
	retval = x->str + pos;
	return (retval);
    }
    int diff = oldlen - pos;
    char *tmp;
    if (diff > 0) {
	tmp = new char[diff];
	string::traits_type::copy(tmp, d->str+pos, diff);
    }
    memset(d->str+pos, c, n);
    if (diff > 0) {
	string::traits_type::copy(d->str+pos+n, tmp, diff);
	delete tmp;
    }
    d->len += n;
    return (retval);
}

const size_t string::npos = (size_t) -1;

size_t
string::copy(char *s, size_t n, size_t pos) {
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
    string::traits_type::copy(s, d->str+pos, n); 
    return (n);
}

void
string::swap(string &s) {
    string::Srep_LUCENT *p = s.d;
    s.d = this->d;
    this->d = p;
}

int
string::compare(const string& s, size_t pos, size_t n) const {
	int retval;
	if (pos == npos)
		pos = d->len;
	register int dlen = d->len - pos;
	register int slen = s.d->len;
	if (n == npos || n > dlen || n > slen)
		n = (dlen < slen) ? dlen : slen;
	retval = string::traits_type::compare(d->str + pos, s.d->str, n);
	if (retval == 0)
		retval = dlen - slen;
	return retval;
}

int
string::compare(const char *s, size_t pos, size_t n) const {
	int retval;
	if (pos == npos)
		pos = d->len;
	register int dlen = d->len - pos;
	register int slen = n;
	if (n > dlen)
		n = dlen;
	retval = string::traits_type::compare(d->str + pos, s, n);
	if (retval == 0)
		retval = dlen - slen;
	return retval;
}

int
string::compare(const char *s, size_t pos) const {
	register int dlen = d->len - pos;
	register int n = s ? string::traits_type::length(s) : 0;
	register int slen = n;
	if (n > dlen)
		n = dlen;
	int retval = string::traits_type::compare(d->str + pos, s, n);
	if (retval == 0)
		retval = dlen - slen;
	return retval;
}
void
string::resize(size_t n, char c) {
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
	register string::Srep_LUCENT *x = string::Srep_LUCENT::new_srep(n);
	string::traits_type::copy(x->str, d->str, copycount);
	d->rcdec();
	d = x;
    }
    if (n > copycount) memset(d->str + copycount, c, n - copycount);
    d->len = n;
}

// Removing:

string&
string::remove(size_t pos, size_t n) {
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
    if (pos == 0 && n == d->len)  // clear the entire string
	return (assign(d->str, 0));
	
    register int newln = d->len - n;

    if (d->refc > 1) {
	register string::Srep_LUCENT *x = string::Srep_LUCENT::new_srep(newln);
	if (pos > 0) string::traits_type::copy(x->str, d->str, pos);
	string::traits_type::copy(x->str+pos, d->str+pos+n, newln-pos);
	d->rcdec();
	d = x;
	return (*this);
    }
    string::traits_type::copy(d->str+pos, d->str+pos+n, newln-pos);
    d->len -= n;
    return (*this);
}
string&
string::remove(iterator p) {
    return (remove(p - d->str, 1));
}
string&
string::remove(iterator first, iterator last) {
    return (remove(first - d->str, last - first));
}

// Removing:

string&
string::replace(size_t pos1, size_t n1, const string &str, size_t pos2, size_t n2) {
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
	register string::Srep_LUCENT *x = string::Srep_LUCENT::new_srep(newln);
	if (pos1 > 0) string::traits_type::copy(x->str, d->str, pos1);
	if (n2 > 0) string::traits_type::copy(x->str+pos1, str.d->str+pos2, n2);
	if (taillen != 0) string::traits_type::copy(x->str+pos1+n2, d->str+pos1+n1, taillen);
	d->rcdec();
	d = x;
	return (*this);
    }
    char *tmp;
    if (taillen > 0) {
	tmp = new char[taillen];
	string::traits_type::copy(tmp, d->str+pos1+n1, taillen);
    }
    if (n2 > 0) string::traits_type::copy(d->str+pos1, str.d->str+pos2, n2);
    if (taillen > 0) {
	string::traits_type::copy(d->str+pos1+n2, tmp, taillen);
	delete tmp;
    }
    d->len = newln;
    return (*this);
}
string&
string::replace(size_t pos, size_t n1, const char *s, size_t n2) {
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
	register string::Srep_LUCENT *x = string::Srep_LUCENT::new_srep(newln);
	if (pos > 0) string::traits_type::copy(x->str, d->str, pos);
	if (n2 > 0) string::traits_type::copy(x->str+pos, s, n2);
	if (taillen != 0) string::traits_type::copy(x->str+pos+n2, d->str+pos+n1, taillen);
	d->rcdec();
	d = x;
	return (*this);
    }
    char *tmp;
    if (taillen > 0) {
	tmp = new char[taillen];
	string::traits_type::copy(tmp, d->str+pos+n1, taillen);
    }
    if (n2 > 0) string::traits_type::copy(d->str+pos, s, n2);
    if (taillen > 0) {
	string::traits_type::copy(d->str+pos+n2, tmp, taillen);
	delete tmp;
    }
    d->len = newln;
    return (*this);
}
string&
string::replace(size_t pos, size_t n1, const char *s) {
    return (replace(pos, n1, s, string::traits_type::length(s)));
}
string&
string::replace(size_t pos, size_t n, char c) {
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
	register string::Srep_LUCENT *x = string::Srep_LUCENT::new_srep(oldlen);
	if (pos > 0) string::traits_type::copy(x->str, d->str, pos);
	memset(x->str+pos, c, n);
	if (taillen != 0) string::traits_type::copy(x->str+pos+n, d->str+pos+n, taillen);
	d->rcdec();
	d = x;
	return (*this);
    }
    memset(d->str+pos, c, n);
    return (*this);
}
string&
string::replace(iterator first, iterator last, const string &str) {
    int n2 = str.d->len;
	
    register int oldlen = d->len;
    register int newln = oldlen - (last - first) + n2;
    int taillen = oldlen - (last - d->str);
    int pos = first - d->str;

    if (d->refc > 1 || newln >= d->max_size) {
	register string::Srep_LUCENT *x =
	    string::Srep_LUCENT::new_srep(newln);
	if (pos > 0) string::traits_type::copy(x->str, d->str, pos);
	if (n2 > 0) string::traits_type::copy(x->str+pos, str.d->str, n2);
	if (taillen != 0) string::traits_type::copy(x->str+pos+n2, last, taillen);
	d->rcdec();
	d = x;
	return (*this);
    }
    char *tmp;
    if (taillen > 0) {
	tmp = new char[taillen];
	string::traits_type::copy(tmp, last, taillen);
    }
    if (n2 > 0) string::traits_type::copy(d->str+pos, str.d->str, n2);
    if (taillen > 0) {
	string::traits_type::copy(d->str+pos+n2, tmp, taillen);
	delete tmp;
    }
    d->len = newln;
    return (*this);
}
string&
string::replace(iterator first, iterator last, const char *s, size_t n) {
    register int oldlen = d->len;
    register int newln = oldlen - (last - first) + n;
    int taillen = oldlen - (last - d->str);
    int pos = first - d->str;

    if (d->refc > 1 || newln >= d->max_size) {
	register string::Srep_LUCENT *x =
	    string::Srep_LUCENT::new_srep(newln);
	if (pos > 0) string::traits_type::copy(x->str, d->str, pos);
	if (n > 0) string::traits_type::copy(x->str+pos, s, n);
	if (taillen != 0) string::traits_type::copy(x->str+pos+n, last, taillen);
	d->rcdec();
	d = x;
	return (*this);
    }
    char *tmp;
    if (taillen > 0) {
	tmp = new char[taillen];
	string::traits_type::copy(tmp, last, taillen);
    }
    if (n > 0) string::traits_type::copy(d->str+pos, s, n);
    if (taillen > 0) {
	string::traits_type::copy(d->str+pos+n, tmp, taillen);
	delete tmp;
    }
    d->len = newln;
    return (*this);
}
string&
string::replace(iterator first, iterator last, const char *s) {
    return(replace(first, last, s, string::traits_type::length(s)));
}
string&
string::replace(iterator first, iterator last, char c) {
    int n = last - first;
    if (n == 0)  // do nothing
	return (*this);
	
    register int oldlen = d->len;
    int taillen = oldlen - (last - d->str);
    int pos = first - d->str;

    if (d->refc > 1) {
	register string::Srep_LUCENT *x = string::Srep_LUCENT::new_srep(oldlen);
	if (pos > 0) string::traits_type::copy(x->str, d->str, pos);
	memset(x->str+pos, c, n);
	if (taillen != 0) string::traits_type::copy(x->str+pos+n, last, taillen);
	d->rcdec();
	d = x;
	return (*this);
    }
    memset(d->str+pos, c, n);
    return (*this);
}

void
string::reserve_grow(int target)
{
    string::Srep_LUCENT *x = string::Srep_LUCENT::new_srep(target);
    x->len = d->len;
    if (d->len != 0) {
	string::traits_type::copy(x->str, d->str, d->len);
    }
    d->rcdec();
    d = x;
}

#if 1
string::string(const string& s, size_t pos, size_t n) {
#else
string::basic_string(const string& s, size_t pos, size_t n) {
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
	d = string::Srep_LUCENT::new_srep(n);
	if (n != 0) string::traits_type::copy(d->str, s.d->str + pos, n);
    }
}

#if 1
string::string(const char *st, unsigned length)
#else
string::basic_string(const char *st, unsigned length)
#endif
{
    if (!st) length=0;
    d = string::Srep_LUCENT::new_srep(length);
    if (length) string::traits_type::copy(d->str, st, length);
}

#if 1
string::string(const char *st)
#else
string::basic_string(const char *st)
#endif
{
    register int ln = st ? string::traits_type::length(st) : 0;
    d = string::Srep_LUCENT::new_srep(ln);
    if (ln) string::traits_type::copy(d->str, st, ln);
}

#if 1
string::string(size_t n, char c) {
#else
string::basic_string(size_t n, char c) {
#endif
#if defined(EXCEPTION_LUCENT)
    if (n == npos) throw length_error(string("in constructor"));
#endif
    d = string::Srep_LUCENT::new_srep(n);
    if (n != 0) memset(d->str, c, n);
}

/* SUBCHAR STUFF */
string::reference
string::operator[](size_t i)
{
    if (d->refc!=1) {
	register string::Srep_LUCENT* r = string::Srep_LUCENT::new_srep(length());
	if (length() > 0)
	    string::traits_type::copy(r->str,d->str,length());
	if (d->refc > 0)
	    --(d->refc);
	d = r;
    }
    return d->str[i];
}
string::reference
string::at(size_t i)
{
#if defined(EXCEPTION_LUCENT)
    if (i >= d->len) throw out_of_range(string("in at()"));
#endif
    if (d->refc != 1) {
	register string::Srep_LUCENT* r = string::Srep_LUCENT::new_srep(length());
	if (length() > 0)
	    string::traits_type::copy(r->str,d->str,length());
	if (d->refc > 0)
	    --(d->refc);
	d = r;
    }
    return d->str[i];
}

string
string::substr(size_t pos, size_t n) const {
    if (n == npos) {
	n = d->len - pos;
    }
    if (n == d->len && pos == 0) {
	return (*this);	// the copy constructor will avoid
			// making a new string representation
    }
    register char *sp = d->str + pos;
    return string(sp, n);
}
Pool_std* string::Srep_LUCENT::Reelp = 0;
Pool_std* string::Srep_LUCENT::SPoolp = 0;
string::Srep_LUCENT* string::Srep_LUCENT::nullrep_ = 0;

void
string::Srep_LUCENT::doinitialize() 
{
    Reelp = new Pool_std(sizeof(string::Srep_LUCENT));
    SPoolp = new Pool_std(sizeof(string::Srep_LUCENT) + sizeof(wchar_t)*(MAXL - MAXS));
    nullrep_ = new string::Srep_LUCENT();
}

string::Srep_LUCENT *
string::Srep_LUCENT::new_srep(size_t length)
{ 
    initialize();
    register string::Srep_LUCENT* x;
    if (length == 0) return nullrep_;
    if (length >= MAXL) {
	 x = get_long(length);
    }
    else {
	if (length < MAXS) 
		x = (string::Srep_LUCENT*) Reelp->alloc();
	else
		x = (string::Srep_LUCENT*) SPoolp->alloc();
	x->len = length;
	x->refc = 1;
	x->max_size = length < MAXS ? MAXS : MAXL;
    }
    return x;
}

string::Srep_LUCENT *
string::Srep_LUCENT::get_long(size_t length) 
{ 
    register int m = 128;
    while (m <= length) m <<= 1;

    register string::Srep_LUCENT *x =
	(string::Srep_LUCENT *) new char[sizeof(wchar_t)*m + sizeof(string::Srep_LUCENT) - sizeof(wchar_t)*MAXS]; 
    x->len = length;
    x->max_size = m;
    x->refc = 1;
    return x;
}

void 
string::Srep_LUCENT::delete_srep() 
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
