/*ident	"@(#)cls4:tools/demangler/osrc/String.h	1.1" */

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
#define STRING_START 32
#define PTR(S) ((S)->data + (S)->sg.start)

typedef struct {
	int start,end,max;
} StringGuts;

typedef struct {
	StringGuts sg;
	char data[1];
} String;

extern String *prep_String();
extern String *nprep_String();
extern String *app_String();
extern String *napp_String();
extern String *mk_String();
extern void free_String();
extern String *set_String();
extern String *trunc_String();
extern char *findop();
