/*ident	"@(#)aoutdem:hpux/demangle.c	3.1" */
/******************************************************************************
*
* C++ Standard Components, Release 3.0.
*
* Copyright (c) 1991, 1992 AT&T and Unix System Laboratories, Inc.
* Copyright (c) 1988, 1989, 1990 AT&T.  All Rights Reserved.
*
* THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T and Unix System
* Laboratories, Inc.  The copyright notice above does not evidence
* any actual or intended publication of such source code.
*
******************************************************************************/

//  Demangle for HP-UX
//
//  Author:  D. Mancl - AT&T Bell Labs LC
//  Feb. 22, 1989
//
//  HP-UX a.out files are difficult to parse, so this program
//  simply scans through the two sections of the a.out file that
//  contain symbol table information, looking for C++-style names.
//  This program makes a list of the places where C++-style names
//  are found and then substitutes simpler names in the symbol
//  table.

//  This program must be linked with the mangle.a library distributed
//  with the C++ Translator.

#include <osfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <memory.h>
#include <a.out.h>
#include <string.h>

#define BUFLEN 512
static void getnm(FILE *, FILE *, int, int);
static char *lcldemangle(char *);
extern "C" {
	extern int demangle(char *, char *);
//	extern char *mktemp(char *);
}

main(int argc, char **argv)
{
	header h;
	FILE *f;
	FILE *tmpfile;
	char *tfilename = "/tmp/dxXXXXXX";
	
	char buf[BUFLEN + 1];
	char newbuf[BUFLEN + 1];
	char *p, *storep;
	char newp[BUFLEN + 1];
	int seekpos;

	if (argc!=2)
	{
		fprintf(stderr, "Usage: %s filename\n", argv[0]);
		exit(2);
	}

	f = fopen(argv[1], "r");
	if (f == NULL)
	{
		fprintf(stderr,
			"Can't open executable file %s for reading\n",
			argv[optind]);
		exit(2);
	}
	
	mktemp(tfilename);
	
	tmpfile = fopen(tfilename, "w");
	if (tmpfile == NULL)
	{
		fprintf(stderr,
			"Trouble making temporary file %s\n", tfilename);
		exit(2);
	}
	
	
	// read in header information from executable file
	fread((char *)&h, sizeof(header), 1, f);
	
	// figure out what changes should be made to the
	// string areas of the executable file
	
	if (h.symbol_strings_size != 0)
	{
		getnm(f, tmpfile, h.symbol_strings_location,
			h.symbol_strings_location + h.symbol_strings_size);
	}
	if (h.unloadable_sp_size != 0)
	{
		getnm(f, tmpfile, h.unloadable_sp_location,
			h.unloadable_sp_location + h.unloadable_sp_size);
	}

	// tmpfile now contains a list of all of the C++-style
	// names in the symbol table sections of the a.out file
	
	fclose(tmpfile);
	fclose(f);

	
//------------------------------------------------

	// This part of the program scans through the temporary
	// file created in the first part.  The C++-style names
	// are overwritten by simpler names generated by either the
	// demangle subroutine (if the -x option is set) or
	// the lcldemangle subroutine below.
	
	f = fopen(argv[optind], "r+");
	if (f == NULL)
	{
		fprintf(stderr,
			"Can't open executable file %s for update\n",
			argv[optind]);
		exit(2);
	}
	
	tmpfile = fopen(tfilename, "r");
	if (tmpfile == NULL)
	{
		fprintf(stderr,
			"Trouble opening temporary file %s\n", tfilename);
		exit(2);
	}
	
	while (fgets(buf, BUFLEN, tmpfile) != NULL)
	{
		storep = newbuf;
		p = buf;
		seekpos = atoi(buf);
		while (*p != ' ')
		{
			p++;
		}
		p++;
		while (isalnum(*p) || *p == '_')
		{
			*storep++ = *p++;
		}
		*storep = '\0';
		
		fseek(f, seekpos, 0);
		strcpy(newp, lcldemangle(newbuf));
		if (strcmp(newp, newbuf) != 0)
		{
			int oldlen = strlen(newbuf);
			int newlen = strlen(newp);
			if (newlen>oldlen)
			{
				fprintf(stderr,
					"%s not demangled (%s is too long)\n",
					newbuf, newp);
				continue;
			}
			fprintf(f, "%s", newp);
			for (int i=oldlen-newlen+1; i>0; i--)
			{
				putc('\0', f);
			}
		}
	}
	unlink(tfilename);
	fclose(f);
}

// getnm will find all of the C++-style names in the a.out
// file between the positions startloc and endloc.
// startloc and endloc are offsets from the beginning of
// the file.  getnm writes the name and starting position
// to tmpfile for later processing in the main program.

static void
getnm(FILE *f, FILE *tmpfile, int startloc, int endloc)
{
	char buf[2 * BUFLEN + 1];
	char *p, *startp;
	int bcount;
	char *bufend;
	char *restartp = (char *) 0;
	int offset;
	
	fseek(f, startloc, 0);
	offset = startloc;

	while ((ftell(f) < endloc) &&
		((bcount = fread(&buf[BUFLEN], 1, BUFLEN, f)) != 0))
	{
		bufend = &buf[BUFLEN] + bcount;

		if (restartp != (char *) 0)
		{
			p = restartp;
			while (p < bufend && (isalnum(*p) || *p == '_'))
			{
				p++;
			}
			*p++ = '\0';
			fprintf(tmpfile, "%d %s\n",
				offset + (restartp - &buf[BUFLEN]),
				restartp);
			fflush(tmpfile);
			restartp = (char *) 0;
		}
		else
		{
			p = &buf[BUFLEN];
		}
		
		while (1)
		{
			while (p < bufend && *p != '_')
			{
				p++;
			}
			if (p >= bufend)
			{
				break;
			}
			p++;
			if (*p != '_')
			{
				continue;
			}
			startp = p - 1;
			p++;
			while (p < bufend &&
				(isalnum(*p) || *p == '_'))
			{
				p++;
			}
			*p++ = '\0';
			while (startp > buf &&
				(isalnum(*(startp-1)) ||
					*(startp-1) == '_'))
			{
				startp--;
			}
			if (p >= bufend && ftell(f) < endloc)
			{
				restartp = startp - BUFLEN;
			}
			else
			{
				fprintf(tmpfile, "%d %s\n",
					offset + (startp - &buf[BUFLEN]),
					startp);
				fflush(tmpfile);
			}
		}
		offset += BUFLEN;
		memcpy(buf, &buf[BUFLEN], BUFLEN);
	}
}

// simplified demangling for HP-UX
//    1. convert :: to __
//    2. leave the function argument types off

static char *
lcldemangle(char *nameptr)
{
	static char buf[BUFLEN + 1];
	char *p, *p2, *retval = buf;
	
	demangle(nameptr, buf);

	if (p = strchr(retval, '('))	// a function name ?
	{
		// special case of 'operator()(args)' ?
		if (p[1]==')' && p[2]=='(')
		{
			p = p+2;
		}
		*p = '\0';		// elide arguments
	}
	else	// find rightmost '::' and remove qualification
	{
		p = p2 = retval;
		while (p2 = strstr(p, "::"))
		{
			p = p2+2;
		}
		retval = p;
	}

	// try to shorten those demangled names whose lengths are
	// greater than their mangled counterparts

	p = retval;

	// 'operator [new|delete|T]' or 'operator@' ?
	if (p2 = strstr(p, "operator"))
	{
		if (p2[8]==' ')
		{
			strcpy(p2, p2+9);
		}
		else if (p2[8]!='_' && !isalnum(p2[8]))
		{
			strcpy(p2, p2+8);
		}
	}
	else
	{
		// ctor or dtor ?
		while (p2 = strstr(p, "::"))
		{
			int l1 = strlen(p);
			int l2 = p2 - p;
			if (
				(l1==2*l2+2 && strncmp(p, p2+2, l2)==0)
				||
				(l1==2*l2+3 && p2[2]=='~' && strncmp(p, p2+3, l2)==0)
			)
			{
				p2[4] = p2[2]=='~' ? 'd' : 'c';
				p2[2] = '_';
				p2[3] = '_';
				p2[5] = 't';
				p2[6] = '\0';
			}
			p = p2+2;
		}
	}

	// s/::/__/g
	p = retval;
	while (p = strstr(p, "::"))
	{
		*p++ = '_';
		*p++ = '_';
	}
	return retval;
}
