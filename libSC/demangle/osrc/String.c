/*ident	"@(#)cls4:tools/demangler/osrc/String.c	1.1" */

/*******************************************************************************
 
C++ source for the C++ Language System, Release 3.0.  This product
is a new release of the original cfront developed in the computer
science research center of AT&T Bell Laboratories.

Copyright (c) 1991 AT&T and UNIX System Laboratories, Inc.
Copyright (c) 1984, 1989, 1990 AT&T.  All Rights Reserved.

THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE of AT&T and UNIX System
Laboratories, Inc.  The copyright notice above does not evidence
any actual or intended publication of such source code.

*******************************************************************************/

/*
 * C++ Demangler Source Code
 * @(#)master	1.5
 * 7/27/88 13:54:37
 */
#include <stdio.h>
#include <setjmp.h>
#include <assert.h>
#ifdef BSD
#include <strings.h>
#else
#include <string.h>
#endif
#include "elf_dem.h"
#include "String.h"

/* This code emulates the C++ String package
 * in a crude way.
 */

jmp_buf jbuf;

/* This function will expand the space
 * available to a String so that more data
 * can be appended to it
 */
static String *
grow(s)
String *s;
{
	String *ns;
	int sz = s->sg.max * 2;
	assert(sz > 0);
#ifdef ELF
	if ((ns = (String *) malloc(sz + sizeof(StringGuts)+1)) == NULL)
		longjmp(jbuf, 1);
	memcpy(ns, s, s->sg.max + sizeof(StringGuts)+1);
	free(s);
#else
	if ((ns = (String *)realloc(s,sz + sizeof(StringGuts)+1)) == NULL)
		longjmp(jbuf, 1);
#endif
	ns->sg.max = sz;
	return ns;
}

/* This function will expand the space
 * available to a String so that more data
 * can be prepended to it.
 */
static String *
ror(s,n)
String *s;
int n;
{
	int i;
	assert(s != 0);
	while(s->sg.end + n > s->sg.max)
		s = grow(s);
#if __STDC__ == 1
	assert(n >= 0);
	assert(s->sg.end >= s->sg.start);
	memmove(s->data + n, s->data, s->sg.end - s->sg.start);
#else
	for(i = s->sg.end - 1;i >= s->sg.start;i--)
		s->data[i+n] = s->data[i];
#endif
	s->sg.end += n;
	s->sg.start += n;
	s->data[s->sg.end] = 0;
	return s;
}

/* This function will prepend c to s
 * clen==strlen(c), or <0
 */
String *
prep_String(c,clen,s)
char *c;
int clen;
String *s;
{
	return nprep_String(c,clen,s,NAME_MAX);
}

/* This function will prepend the
 * first n characters of c to s
 * len==strlen(c), or <0
 */
String *
nprep_String(c,len,s,n)
char *c;
int len;
String *s;
int n;
{
	if (len < 0) len = strlen(c);
	assert(s != 0);
	if(len > n)
		len = n;
	if(len > s->sg.start)
		s = ror(s, len - s->sg.start);
	s->sg.start -= len;
#ifdef alliant
	bcopy(s->data + s->sg.start, c, len);
#else
	memcpy(s->data + s->sg.start, c, len);
#endif
	return s;
}

/* This function will append c to s.
 * clen==strlen(c), or <0
 */
String *
app_String(s,c,clen)
String *s;
char *c;
int clen;
{
	return napp_String(s,c,clen,NAME_MAX);
}

/* This function will append the
 * first n characters of c to s
 * len==strlen(c), or <0
 */
String *
napp_String(s,c,len,n)
String *s;
char *c;
int len;
{
	int catlen;
	if (len < 0) len = strlen(c);
	assert(s != 0);
	if(n < len)
		len = n;
	catlen = s->sg.end + len;
	while(catlen > s->sg.max)
		s = grow(s);
#ifdef alliant
	bcopy(s->data + s->sg.end, c, len);
#else
	memcpy(s->data + s->sg.end, c, len);
#endif
	s->sg.end += len;
	s->data[s->sg.end] = '\0';
	return s;
}

/* This function initializes a
 * String.  It returns its argument if
 * its argument is non-zero.
 * This prevents the same string
 * from being re-initialized.
 */
String *
mk_String(s)
String *s;
{
	if(s)
		return s;
	s = (String *)malloc(STRING_START + sizeof(StringGuts)+1);
	if (s == NULL)
		longjmp(jbuf, 1);
	s->sg.start = s->sg.end = STRING_START/2;
	s->sg.max = STRING_START;
	s->data[s->sg.end] = '\0';
	return s;
}

void
free_String(s)
String **s;
{
	if(*s)
	{
		free(*s);
		*s = 0;
	}
}

/* This function copies
 * c into s.
 * Used for initialization.
 * clen==strlen(c), or <0
 */
String *
set_String(s,c,clen)
String *s;
char *c;
int clen;
{
	int len;
	if (clen < 0) clen = strlen(c);
	len = clen*2;
	while(len > s->sg.max)
		s = grow(s);
	s->sg.start = s->sg.end = s->sg.max / 2;
	s = app_String(s,c,clen);
	return s;
}

/* Chop n characters off the end of a string.
 * Return the truncated string.
 */

String *
trunc_String(s, n)
String *s;
{
	assert(n <= s->sg.end - s->sg.start);
	s->sg.end -= n;
	s->data[s->sg.end] = '\0';
	return s;
}
