/*ident	"@(#)cls4:tools/demangler/osrc/dem.c	1.1" */

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
/* Demangle a single command line argument */
main(argc,argv)
int argc;
char **argv;
{
	int i;
	for(i=1;i<argc;i++)
		printf("%s == %s\n",argv[i],demangle(argv[i]));
	return (0);
}
