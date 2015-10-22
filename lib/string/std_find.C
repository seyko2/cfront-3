/*ident	"@(#) @(#)std_find.c	1.1.1.2" */
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
#ifdef NAMESPACES_LUCENT
namespace std {
#endif

size_t
string::find_first_of(const string &s, size_t pos) const
{
    char  tab[256];
    register const char *p, *q;
    register char    *ptr, *eptr;
    for (ptr = tab, eptr=&tab[256]; ptr<eptr; )
        *ptr++ = 0;
    for (p = s.d->str, q=s.d->str+s.d->len; p<q; )
        tab[(unsigned char)*p++] = 1;
    for (p = d->str-1+pos, q=d->str+d->len; ++p < q; )
        if (tab[(unsigned char)*p])
            return p - d->str;
    return npos;
}

size_t
string::find_first_of(const char *s, size_t pos, size_t n) const
{
    char  tab[256];
    register const char *p, *q;
    register char    *ptr, *eptr;
    for (ptr = tab, eptr=&tab[256]; ptr<eptr; )
        *ptr++ = 0;
    for (p = s, q=s+n; p<q; )
        tab[(unsigned char)*p++] = 1;
    for (p = d->str-1+pos, q=d->str+d->len; ++p < q; )
        if (tab[(unsigned char)*p])
            return p - d->str;
    return npos;
}

size_t
string::find_first_of(const char *s, size_t pos) const
{
    char  tab[256];
    register const char *p, *q;
    register char    *ptr, *eptr;
    for (ptr = tab, eptr=&tab[256]; ptr<eptr; )
        *ptr++ = 0;
    for (p = s; *p != '\0'; )
        tab[(unsigned char)*p++] = 1;
    for (p = d->str-1+pos, q=d->str+d->len; ++p < q; )
        if (tab[(unsigned char)*p])
            return p - d->str;
    return npos;
}

size_t
string::find_first_of(char c, size_t pos) const {
    return (find(c, pos));
}

size_t
string::find_last_of(const string &s, size_t pos) const
{
    char  tab[256];
    register const char *p, *q;
    register char    *ptr, *eptr;
    for (ptr = tab, eptr=&tab[256]; ptr<eptr; )
        *ptr++ = 0;
    for (p = s.d->str, q=s.d->str+s.d->len; p<q; )
        tab[(unsigned char)*p++] = 1;
    for (p = d->str+d->len, q=d->str+pos; --p >= q; )
        if (tab[(unsigned char)*p] )
            return p - d->str;
    return npos;
}

size_t
string::find_last_of(const char *s, size_t pos, size_t n) const
{
    char  tab[256];
    register const char *p, *q;
    register char    *ptr, *eptr;
    for (ptr = tab, eptr=&tab[256]; ptr<eptr; )
        *ptr++ = 0;
    for (p = s, q=s+n; p<q; )
        tab[(unsigned char)*p++] = 1;
    for (p = d->str+d->len, q=d->str+pos; --p >= q; )
        if (tab[(unsigned char)*p])
            return p - d->str;
    return npos;
}

size_t
string::find_last_of(const char *s, size_t pos) const
{
    char  tab[256];
    register const char *p, *q;
    register char    *ptr, *eptr;
    for (ptr = tab, eptr=&tab[256]; ptr<eptr; )
        *ptr++ = 0;
    for (p = s; *p != '\0'; )
        tab[(unsigned char)*p++] = 1;
    for (p = d->str+d->len, q=d->str+pos; --p >= q; )
        if (tab[(unsigned char)*p])
            return p - d->str;
    return npos;
}

size_t
string::find_last_of(char c, size_t pos) const {
    return (rfind(c, pos));
}

size_t
string::find_first_not_of(const string &s, size_t pos) const
{
    char    tab[256];
    register const char    *p, *q;
    register char    *ptr, *eptr;
    for (ptr = tab, eptr=&tab[256]; ptr<eptr; )
        *ptr++ = 0;
    for (p = s.d->str, q=s.d->str+s.d->len; p<q; )
        tab[(unsigned char)*p++] = 1;
    for (p = d->str-1+pos, q=d->str+d->len; ++p < q; )
        if (!tab[(unsigned char)*p])
            return p - d->str;
    return npos;
}

size_t
string::find_first_not_of(const char *s, size_t pos, size_t n) const
{
    char    tab[256];
    register const char    *p, *q;
    register char    *ptr, *eptr;
    for (ptr = tab, eptr=&tab[256]; ptr<eptr; )
        *ptr++ = 0;
    for (p = s, q=s+n; p<q; )
        tab[(unsigned char)*p++] = 1;
    for (p = d->str-1+pos, q=d->str+d->len; ++p < q; )
        if (!tab[(unsigned char)*p])
            return p - d->str;
    return npos;
}

size_t
string::find_first_not_of(const char *s, size_t pos) const
{
    char    tab[256];
    register const char    *p, *q;
    register char    *ptr, *eptr;
    for (ptr = tab, eptr=&tab[256]; ptr<eptr; )
        *ptr++ = 0;
    for (p = s; *p != '\0'; )
        tab[(unsigned char)*p++] = 1;
    for (p = d->str-1+pos, q=d->str+d->len; ++p < q; )
        if (!tab[(unsigned char)*p])
            return p - d->str;
    return npos;
}

size_t
string::find_first_not_of(char c, size_t pos) const
{
    register const char    *p, *q;
    for (p = d->str-1+pos, q=d->str+d->len; ++p < q; )
        if (*p != c)
            return p - d->str;
    return npos;
}

size_t
string::find_last_not_of(const string &s, size_t pos) const
{
    char    tab[256];
    register const char    *p, *q;
    register char    *ptr, *eptr;
    for (ptr = tab, eptr=&tab[256]; ptr<eptr; )
        *ptr++ = 0;
    for (p = s.d->str, q=s.d->str+s.d->len; p<q; )
        tab[(unsigned char)*p++] = 1;
    for (p = d->str+d->len, q=d->str+pos; --p >= q; )
        if (!tab[(unsigned char)*p])
            return p - d->str;
    return npos;
}

size_t
string::find_last_not_of(const char *s, size_t pos, size_t n) const
{
    char    tab[256];
    register const char    *p, *q;
    register char    *ptr, *eptr;
    for (ptr = tab, eptr=&tab[256]; ptr<eptr; )
        *ptr++ = 0;
    for (p = s, q=s+n; p<q; )
        tab[(unsigned char)*p++] = 1;
    for (p = d->str+d->len, q=d->str+pos; --p >= q; )
        if (!tab[(unsigned char)*p])
            return p - d->str;
    return npos;
}

size_t
string::find_last_not_of(const char *s, size_t pos) const
{
    char    tab[256];
    register const char    *p, *q;
    register char    *ptr, *eptr;
    for (ptr = tab, eptr=&tab[256]; ptr<eptr; )
        *ptr++ = 0;
    for (p = s; *p != '\0'; )
        tab[(unsigned char)*p++] = 1;
    for (p = d->str+d->len, q=d->str+pos; --p >= q; )
        if (!tab[(unsigned char)*p])
            return p - d->str;
    return npos;
}

size_t
string::find_last_not_of(char c, size_t pos) const
{
    register char    *p, *q;
    for (p = d->str+d->len, q=d->str+pos; --p >= q; )
        if (*p != c)
            return p - d->str;
    return npos;
}
#ifdef NAMESPACES_LUCENT
}
#endif
