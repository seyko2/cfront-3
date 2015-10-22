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
//extern int strcpy(char*, char*);
//extern char * strtok(char*, char*);

struct sbuf {
	sbuf* next;
	char str[1024];
	sbuf(sbuf* l, char* p);
};

sbuf::sbuf(sbuf* l, char* p)
{
	next=l;
	// strcpy(str,strtok(p," |"));
	// ``unrolled'' since strtok() is not on bsd systems
	register char* s = str;
	for (register char c = *p++; c && c!=' ' && c!='|'; c = *p++) *s++ = c;
	*s = 0;	
}

sbuf* dtor;	// list of constructors
sbuf* ctor;	// list of destructors

main (int, char*)
{
	char buf[1024];
	register char* p;

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

			register char* st = p;
			if (st[0]!='_' || st[1]!='_' || 
			    st[2]!='s' || st[3]!='t' ||
			    st[5]!='_' || st[6]!='_' ) 
				goto newline;
			switch (st[4]) {
			case 'd':
				dtor = new sbuf(dtor,st);
				goto newline;
			case 'i':
				ctor = new sbuf(ctor,st);
			default:
				goto newline;
			}
		}
		default:
			*p++ = c;
		}
	}

done:
	int cond = dtor||ctor;
	if (cond == 0) exit(1);

	printf("typedef int (*PFV)();\n");	// "int" to dodge bsd4.2 bug
	if (ctor) {
		for (sbuf* p = ctor; p; p=p->next) printf("int %s();\n",p->str);
		printf("extern PFV _ctors[];\nPFV _ctors[] = {\n");
		for (sbuf* q = ctor; q; q=q->next) printf("\t%s,\n",q->str);
		printf("\t0\n};\n");
	}

	if (dtor) {
		for (sbuf* p = dtor; p; p=p->next) printf("int %s();\n",p->str);
		printf("extern PFV _dtors[];\nPFV _dtors[] = {\n");
		for (sbuf* q = dtor; q; q=q->next) printf("\t%s,\n",q->str);
		printf("\t0\n};\n");
	}

	exit(0);
}

