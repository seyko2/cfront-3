/*ident	"@(#)cls4:src/size.c	1.4" */
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

size.c:

	initialize alignment and sizeof "constants"

**********************************************************************/

#include "cfront.h"
#include "size.h"

int BI_IN_WORD = DBI_IN_WORD;
int BI_IN_BYTE = DBI_IN_BYTE;
int SZ_CHAR = DSZ_CHAR;
int AL_CHAR = DAL_CHAR;
int SZ_SHORT = DSZ_SHORT;
int AL_SHORT = DAL_SHORT;
int SZ_INT = DSZ_INT;
int AL_INT = DAL_INT;
int SZ_LONG = DSZ_LONG;
int AL_LONG = DAL_LONG;
int SZ_VLONG = DSZ_VLONG;
int AL_VLONG = DAL_VLONG;
int SZ_FLOAT = DSZ_FLOAT;
int AL_FLOAT = DAL_FLOAT;
int SZ_DOUBLE = DSZ_DOUBLE;
int AL_DOUBLE = DAL_DOUBLE;
int SZ_LDOUBLE = DSZ_LDOUBLE;
int AL_LDOUBLE = DAL_LDOUBLE;
int SZ_STRUCT = DSZ_STRUCT;
int AL_STRUCT = DAL_STRUCT;
//int SZ_FRAME = DSZ_FRAME;
//int AL_FRAME = DAL_FRAME;
int SZ_WORD = DSZ_WORD;
int SZ_WPTR = DSZ_WPTR;
int AL_WPTR = DAL_WPTR;
int SZ_BPTR = DSZ_BPTR;
int AL_BPTR = DAL_BPTR;
//int SZ_TOP = DSZ_TOP;
//int SZ_BOTTOM = DSZ_BOTTOM;
char* LARGEST_INT = DLARGEST_INT;
int F_SENSITIVE = DF_SENSITIVE;
int F_OPTIMIZED =  DF_OPTIMIZED;

static int arg1 = 0;

int get_line(FILE* fp, char *f)
{
	char s[32];
	char s2[32];

	if (fscanf(fp,"%s ",s) == EOF) return 0;

	if (strcmp("DLARGEST_INT",s) == 0) { 
		if (fscanf(fp," %s",s2)==EOF) return 0;
//// get rid of quotes
		for(int i=0;i<=strlen(s2)+1;i++)
			if (s2[i]== '"') {
				for (int j=i;j<=strlen(s2)+1;j++)
					s2[j]=s2[j+1];
			}
		}
	else 
		if (fscanf(fp,"%d ",&arg1) == EOF) return 0;

	if (strcmp("DSZ_CHAR",s) == 0) {
		SZ_CHAR = arg1;
		return 1;
	}

	if (strcmp("DAL_CHAR",s) == 0) {
		AL_CHAR = arg1;
		return 1;
	}

	if (strcmp("DSZ_SHORT",s) == 0) {
		SZ_SHORT = arg1;
		return 1;
	}
	
	if (strcmp("DAL_SHORT",s) == 0) {
		AL_SHORT = arg1;
		return 1;
	}

	if (strcmp("DSZ_INT",s) == 0) {
		SZ_INT = arg1;
		return 1;
	}

	if (strcmp("DAL_INT",s) == 0) {
		AL_INT=arg1;
		return 1;
	}

	if (strcmp("DLARGEST_INT",s) == 0) {
		LARGEST_INT = new char[strlen(s2)+1];
		strcpy(LARGEST_INT,s2);
		return 1;
	}

	if (strcmp("DSZ_LONG",s) == 0) {
		SZ_LONG = arg1;
		return 1;
	}

	if (strcmp("DAL_LONG",s) == 0) {
		AL_LONG = arg1;
		return 1;
	}

	if (strcmp("DSZ_VLONG",s) == 0) {
		SZ_VLONG = arg1;
		return 1;
	}

	if (strcmp("DAL_VLONG",s) == 0) {
		AL_VLONG = arg1;
		return 1;
	}

	if (strcmp("DSZ_FLOAT",s) == 0) {
		SZ_FLOAT = arg1;
		return 1;
	}

	if (strcmp("DAL_FLOAT",s) == 0) {
		AL_FLOAT = arg1;
		return 1;
	}

	if (strcmp("DSZ_DOUBLE",s) == 0) {
		SZ_DOUBLE = arg1;
		return 1;
	}

	if (strcmp("DAL_DOUBLE",s) == 0) {
		AL_DOUBLE = arg1;
		return 1;
	}
	
	if (strcmp("DSZ_LDOUBLE",s) == 0) {
		SZ_LDOUBLE = arg1;
		return 1;
	}

	if (strcmp("DAL_LDOUBLE",s) == 0) {
		AL_LDOUBLE = arg1;
		return 1;
	}

	if (strcmp("DBI_IN_BYTE",s) == 0) {
		BI_IN_BYTE = arg1;
		return 1;
	}

	if (strcmp("DBI_IN_WORD",s) == 0) {
		BI_IN_WORD= arg1;
		return 1;
	}

	if (strcmp("DSZ_STRUCT",s) == 0) {
		SZ_STRUCT = arg1;
		return 1;
	}

	if (strcmp("DAL_STRUCT",s) == 0) {
		AL_STRUCT = arg1;
		return 1;
	}

	if (strcmp("DF_SENSITIVE",s) == 0) {
		F_SENSITIVE = arg1;
		return 1;
	}

	if (strcmp("DF_OPTIMIZED",s) == 0) {
		F_OPTIMIZED = arg1;
		return 1;
	}

	if (strcmp("frame",s) == 0) {
//		SZ_FRAME = arg1;
//		AL_FRAME = arg2;
		return 1;
	}

	if (strcmp("DSZ_WORD",s) == 0) {
		SZ_WORD = arg1;
		return 1;
	}

	if (strcmp("DSZ_WPTR",s) == 0) {
		SZ_WPTR = arg1;
		return 1;
	}

	if (strcmp("DAL_WPTR",s) == 0) {
		AL_WPTR = arg1;
		return 1;
	}

	if (strcmp("DSZ_BPTR",s) == 0) {
		SZ_BPTR = arg1;
		return 1;
	}

	if (strcmp("DAL_BPTR",s) == 0) {
		AL_BPTR = arg1;
		return 1;
	}

	if (strcmp("top",s) == 0) {
//		SZ_TOP = arg1;
//		SZ_BOTTOM = arg2;
		return 1;
	}
	fprintf(stderr, "%s: %s unknown alignment name\n", f, s);
	return 0;
}

int read_align(char* f)
{
	char* p = f;
	if (*p == 0) {
		fprintf(stderr,"size/align file missing\n");
		ext(1);
	}
	FILE* fp = fopen(f,"r");
	if (fp == 0) return 1;
	while (get_line(fp, f)) ;
	return 0;
}
/*
print_align(char* s)
{
	fprintf(stderr,"%s sizes and alignments\n\n",s);

	fprintf(stderr,"	size	align	largest\n");
	fprintf(stderr,"char	%d	%d\n",SZ_CHAR,AL_CHAR);
	fprintf(stderr,"short	%d	%d\n",SZ_SHORT,AL_SHORT);
	fprintf(stderr,"int	%d	%d	%s\n",SZ_INT,AL_INT,LARGEST_INT);
	fprintf(stderr,"long	%d	%d\n",SZ_LONG,AL_LONG);
	fprintf(stderr,"float	%d	%d\n",SZ_FLOAT,AL_FLOAT);
	fprintf(stderr,"double	%d	%d\n",SZ_DOUBLE,AL_DOUBLE);
	fprintf(stderr,"ldouble	%d	%d\n",SZ_LDOUBLE,AL_LDOUBLE);
	fprintf(stderr,"bptr	%d	%d\n",SZ_BPTR,AL_BPTR);
	fprintf(stderr,"wptr	%d	%d\n",SZ_WPTR,AL_WPTR);
	fprintf(stderr,"struct	%d	%d\n",SZ_STRUCT,AL_STRUCT);
	fprintf(stderr,"struct2	%d	%d\n",F_SENSITIVE,F_OPTIMIZED);
//	fprintf(stderr,"frame	%d	%d\n",SZ_FRAME,AL_FRAME);

	fprintf(stderr,"%d bits in a byte, %d bits in a word, %d bytes in a word\n",
			BI_IN_BYTE, BI_IN_WORD, SZ_WORD);
	return 1;
}
*/

int c_strlen(const char* s)
/*
	return sizeof(s) with escapes processed
	sizeof("") == 1		the terminating 0
	sizeof("a") == 2
	sizeof("\0x") == 3	0 x 0
	sizeof("\012") == 2	'\012'
	sizeof("\01")		'\001'
	sizeof("\x") == 2 	\ ignored

*/
{
	int i = 1;
        for (const char* p = s; *p; i++,p++) {
		if (*p == '\\')	{					// '\?
			switch (*++p) {
			case '0':
				switch (p[1]) {		// '\01' or '\012'
				case '0': case '1': case '2': case '3':
				case '4': case '5': case '6': case '7':
					break;
				default:
					continue;	// '\0'
				}
				/* no break */
			case '1': case '2': case '3':
			case '4': case '5': case '6': case '7':		// '\123'
				switch (*++p) {
				case '0': case '1': case '2': case '3':
				case '4': case '5': case '6': case '7':
					switch (*++p) {
					case '0': case '1': case '2': case '3':
					case '4': case '5': case '6': case '7':
						break;
					default:
						--p;
					}
					break;
				default:
					--p;
					break;
				}
				break;
			case '\n':			// \newline doesn't count
				i--;
				break;
			}
		}
	}
	return i;
}
