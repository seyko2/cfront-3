/*ident	"@(#)cls4:Munch/munch.c	1.4" */
/*******************************************************************************
 
C++ source for the C++ Language System, Release 3.0.  This product
is a new release of the original cfront developed in the computer
science research center of AT&T Bell Laboratories.

Copyright (c) 1993  UNIX System Laboratories, Inc.
Copyright (c) 1991, 1992 AT&T and UNIX System Laboratories, Inc.
Copyright (c) 1984, 1989, 1990 AT&T.  All Rights Reserved.

THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE of AT&T and UNIX System
Laboratories, Inc.  The copyright notice above does not evidence
any actual or intended publication of such source code.

*******************************************************************************/
/*
	scan nm output and detect constructors and destructors for static objects.
	the name on an nm output line is expected to be in the right hand margin.
	the name is expected to be on the form __std__ or __sti__ and less than
	1024 characters long.
	nm output lines are assumed to be less than 1024 characters long.
	constructors found are called by _main() called from main().
	destructors found are called by exit().
	return 1 if no constructor or destructor is found;
	otherwise, returns 0
	The output is redirected by CC into _ctdt.c
	
*/

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

struct sbuf {
	struct sbuf	*next;
	char 		str[1024];
};

typedef struct sbuf sbuf;
static int buf_counter = 0;

sbuf* new_sbuf(sbuf* l, char* p)
{
	sbuf* res = (sbuf*) malloc (sizeof(sbuf));
	if (!res) {
	    perror("new_buf");
	    exit(1);
	}
	res->next = l;
	char* d = res->str;
	char c = *p++;
	for (; c && c!=' ' && c!='|'; c = *p++) *d++ = c;
	*d = 0;

	buf_counter++;
	return res;
}

sbuf* dtor;	// list of constructors
sbuf* ctor;	// list of destructors

int main ()
{
	char buf[1024];
	char* p;
	sbuf* q;

newline:
	p = buf;
	for(;;) {
		int c;
		switch (c=getchar()) {
		case EOF:
			goto done;
		case '\n':
		{	*p = 0;		// terminate string
			p = buf;
			while (*p!='_') if (*p++ == 0) goto newline;
			if (p!=buf && !isspace( *(p-1))) goto newline; // '_' not first character
                        // accept __st and ___st
                        if (*++p != '_') goto newline; // need '__'
			if (*++p != '_')
                          { p--; 
                            p--;
                          }
                        else
                            p--;

			char* st = p;
			if (st[0]!='_' || st[1]!='_' || 
			    st[2]!='s' || st[3]!='t' ||
			    st[5]!='_' || st[6]!='_' ) 
				goto newline;
			switch (st[4]) {
			case 'd':
				dtor = new_sbuf(dtor,st);
				goto newline;
			case 'i':
				ctor = new_sbuf(ctor,st);
			default:
				goto newline;
			}
		}
		default:
			*p++ = c;
		}
	}

done:
	if (buf_counter == 0) return 1;

	printf("typedef int (*PFV)();\n");	// "int" to dodge bsd4.2 bug
	if (ctor) {
		for (q = ctor; q; q=q->next) printf("int %s();\n",q->str);
		printf("extern PFV _ctors[];\nPFV _ctors[] = {\n");
		for (q = ctor; q; q=q->next) printf("\t%s,\n",q->str);
		printf("\t0\n};\n");
	}

	if (dtor) {
		for (q = dtor; q; q=q->next) printf("int %s();\n",q->str);
		printf("extern PFV _dtors[];\nPFV _dtors[] = {\n");
		for (q = dtor; q; q=q->next) printf("\t%s,\n",q->str);
		printf("\t0\n};\n");
	}

	return 0;
}

