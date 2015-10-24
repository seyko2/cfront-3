/*ident	"@(#)cls4:tools/demangler/osrc/demangle.c	1.1" */

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
 */

#include <ctype.h>
#include <setjmp.h>
#if __STDC__ == 1
#	include <stdlib.h>
#else
	extern long strtol();
#endif
#include "elf_dem.h"
#include "String.h"

#define eqop(str)		(*(str)=='o' && (*((str)+1)=='p'))
#define eq__(str)		(*(str)=='_' && (*((str)+1)=='_'))
#define eqt__(str)		(*(str)=='t' && (*((str)+1)=='_') && (*((str)+2)=='_'))


/* The variable "hold" contains the pointer to the array initially
 * handed to demangle.  It is returned if it is not possible to
 * demangle the string.  NULL is returned if a memory allocation
 * problem is encountered.  Thus one can do the following:
 *
 * char *mn = "Some mangled name";
 * char *dm = mangle(mn);
 * if (dm == NULL)
 *	printf("allocation error\n");
 * else if (dm == mn)
 * 	printf("name could not be demangled\n");
 * else
 *	printf("demangled name is: %s\n",dm);
 */
static char *hold;

/* this String is the working buffer for the demangle
 * routine.  A pointer into this String is returned
 * from demangle when it is possible to demangle the
 * String.  For this reason, the pointer should not
 * be saved between calls of demangle(), nor freed.
 */
static String *s = 0;

static int
getint(c)
char **c;
{
	return strtol(*c, c, 10);
}

/* If a mangled name has a __
 * that is not at the very beginning
 * of the string, then this routine
 * is called to demangle that part
 * of the name.  All overloaded functions,
 * and class members fall into this category.
 *
 * c should start with two underscores followed by a non-zero digit or an F.
 */
static char *
second(c)
char *c;
{
	int n;
	int ncl,nchar;
	int	const_flag = 0;

	if(!eq__(c))
		return hold;

	c += 2;

	if (!(isdigit(*c) || *c == 'F' || *c == 'Q'))
		return hold;

	if (isdigit(*c)) {
		int clen = -1;
		/* a member */
		n = getint(&c);
		if (n == 0 || (clen=strlen(c)) < n)
			return hold;
		s = prep_String("::",2,s);
		s = nprep_String(c,clen,s,n);
		c += n;
	}
	else if(*c == 'Q') {
		String *tmp;
		if (!isdigit(c[1]) || c[2] != '_') return hold;
		tmp = mk_String(0);
		ncl = c[1] - 48;
		c += 3;
		tmp = set_String(tmp,"",0);
		while (ncl--) {
			nchar = 0;
			while(isdigit(*c)) nchar = 10*nchar + (*c++ - 48);
			tmp = napp_String(tmp,c,strlen(c),nchar);
			tmp = app_String(tmp,"::",2);
			c += nchar;
		}
		s = prep_String(PTR(tmp),strlen(PTR(tmp)),s);
		free_String(&tmp);
	}
        while (*c == 'S' || *c == 'C') {
                if(*c == 'S') {
                c++;
                }
                if(*c == 'C') {
                const_flag = 1;
                c++;
                }
        }
	if(*c == 'F') {
		/* an overloaded function */
		switch (*++c) {
		case '\0':
			return hold;
		case 'v':
			s = app_String(s,"()",2);
			break;
		default:
			if(doargs(&s,c) < 0)
				return hold;
		}
	}
	if ( const_flag ) {
		s = app_String(s," const",6);
	}
	return PTR(s);
}

/* on entry: clen==strlen(c), or <0
 */
static char *no_pt_demangle_withlen(c, clen)
char *c;
int clen;
{
	register int i = 0;
	register int ncl,nchar;
	extern jmp_buf jbuf;

	if (setjmp(jbuf))
		return 0;

	hold = c;
	s = mk_String(s);
	s = set_String(s,"",0);

	if(c == 0 || *c == 0)
		return hold;

	if(!eq__(c)) {
		/* If a name does not begin with a __
		 * but it does contain one, it is either
		 * a member or an overloaded function.
		 */
		while (1) {
			while(c[i] && !eq__(c+i))
				i++;
			if (c[i]) {
				/* Advance to first non-underscore */
				while (c[i+2] == '_')
					i++;
			}
			if (strncmp(c+i, "__pt__", 6) != 0) break;
			i += 6;
		}
		
		if(eq__(c+i)) {
			/* Copy the simple name */
			s = napp_String(s,c,clen,i);
			/* Process the signature */
			return second(c+i);
		} else
			return hold;
	} else {
		char *x;
		int oplen;
		c += 2;
		clen -= 2;

		/* For automatic variables, or internal static
		 * variables, a __(number) is prepended to the
		 * name.  If this is encountered, strip this off
		 * and return.
		 */
		if(isdigit(*c)) {
			while(isdigit(*c))
				c++;
			return c;
		}

		if(*c == 'Q') {
			ncl = c[1] - 48;
			c += 3;
			while (ncl--) {
				nchar = 0;
				while(isdigit(*c)) 
					nchar = 10*nchar + (*c++ - 48);
				s = napp_String(s,c,strlen(c),nchar);
				if (ncl) s = app_String(s,"::",2);
				c += nchar;
			}
			return PTR(s);
		}
		/* Handle operator functions -- this
		 * automatically calls second, since
		 * all operator functions are overloaded.
		 */
		if(x = findop(c, &oplen)) {
			s = app_String(s,"operator",8);
			s = app_String(s,x,-1);
			c += oplen;
			return second(c);
		}

		/* Operator cast does not fit the mould
		 * of the other operators.  Its type name
		 * is encoded.  The cast function must
		 * take a void as an argument.
		 */
		if(eqop(c)) {
			int r;
			s = app_String(s,"operator ",9);
			c += 2;
			r = doarg(&s,c);
			if(r < 0)
				return hold;
			c += r;
			return second(c);
		}

		/* Constructors and Destructors are also
		 * a special case of operator name.  Note
		 * that the destructor, while overloaded,
		 * must always take the same arguments --
		 * none.
		 */
		if ((*c == 'c' || *c == 'd') && eqt__(c+1)) {
			int n;
			char *c2 = c+2;
			char cx = c[0];
			c += 4;
			clen -= 4;
			n = getint(&c);
			if(n == 0)
				return hold;
			s = napp_String(s,c,clen,n);
			if(cx == 'd')
				s = prep_String("~",1,s);
			return second(c2);
		}
		return hold;
	}
}

/*
 *	this function finds the first occurrence of __pt__ in
 *	the string.  It returns 0 if __pt__ is not found.
 */
static char *find_pt(p)
char *p;
{
	while (*p) {
		while (*p && *p != '_') p++;
		if (*p != '_') return (0);
		if (strncmp(p, "__pt__", 6) == 0) {
			return (p);
		}
		p++;
	}
	return (0);
}

static String *val1, *val2;
static String *arglist;

static char *demangle_ptstuff(c)
char *c;
{
	char *retval;
	char *p, *q;
	char *argsptr;
	int count;
	int arglen;
	int savec;
	
	p = find_pt(c);
	if (p == 0) {
		return (c);
	}
	
	val1 = mk_String(val1);
	val1 = set_String(val1,"",0);
	val1 = app_String(val1,c,p-c);
	arglist = mk_String(arglist);
	
	while (1) {
		q = find_pt(p);
		if (q == 0) break;
		val1 = app_String(val1,p,q-p); /* append all of the chars
						  up to the __pt__ */
		p = q + 6;		/* advance past the __pt__ */
		
		/* see if this is a parameterized function */
		
		if (*p == 'F') {
			p++;
			arglen = strtol(p, &p, 10);
			p += arglen;
			continue;
		}
		
		/* now demangle the PT argument list */
		
		/* first get the number of characters in the arglist */
		arglen = strtol(p, &p, 10);
		
		/* next get ready to call doargs */
		savec = p[arglen];
		p[arglen] = '\0';
		arglist = set_String(arglist,"",0);
		
		count = doargs(&arglist,p+1);
		
		/* replace the parentheses with angle brackets */
		argsptr = PTR(arglist);
		
		argsptr[0] = '<';
		argsptr[strlen(argsptr)-1] = '>';
		
		val1 = app_String(val1,argsptr,strlen(argsptr));
		
		/* cleanup */
		p[arglen] = savec;
		p += arglen;
	}
	if (p != 0 && strlen(p) > 0) {
		val1 = app_String(val1,p,strlen(p));
	}
	
	retval = PTR(val1);
	p = find_pt(retval);
	if (p == 0) {
		return (retval);
	}
	else {
		/* we need to pt-demangle again */
		val2 = mk_String(val2);
		val2 = set_String(val2,retval,strlen(retval));
		retval = PTR(val2);
		return (demangle_ptstuff(retval));
	}
}

char *demangle_withlen(c, clen)
char *c;
int clen;
{
	char *retval, *p;
	
	retval = no_pt_demangle_withlen(c, clen);
	p = find_pt(retval);
	if (p == 0) {
		return (retval);
	}
	else {
		return (demangle_ptstuff(retval));
	}
}

char *demangle(c)
char *c;
{
	return demangle_withlen(c, strlen(c));
}
