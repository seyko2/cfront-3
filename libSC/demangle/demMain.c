/*ident	"@(#) demangle: demMain.c	3.0" */

/*******************************************************************************
 
C++ source for the C++ Standard Component, Release 3.0.  This product
is a new release of the original cfront developed in the computer
science research center of AT&T Bell Laboratories.

Copyright (c) 1991 AT&T and UNIX System Laboratories, Inc.
Copyright (c) 1984, 1989, 1990 AT&T.  All Rights Reserved.

THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE of AT&T and UNIX System
Laboratories, Inc.  The copyright notice above does not evidence
any actual or intended publication of such source code.

*******************************************************************************/
#include <stdio.h>
#include "dem.h"
	
/* Demangle a single command line argument */
main( argc, argv)
int argc;
char** argv;
{
	char buf[1024];
	int i;

	for(i=1;i<argc;i++)
	{
		demangle( argv[i], buf);
		printf("%s == %s\n",argv[i], buf);
	}
	return(0);
}
