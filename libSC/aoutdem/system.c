/*ident	"@(#)aoutdem:system.c	3.1" */
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

// implementations of system() stuff

#include <libc.h>
#include <osfcn.h>
#include "osfcn2.h"
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include "system.h"

mv(char *from, char *to)
{
	return system("mv", from, to);
}
cp(char *from, char *to)
{
	return system("cp", from, to);
}
ap(char *from, char *to)
{
	return system("cat", from, ">>", to);
}


/* write first l bytes of file from (or all of from, if its length is
*  less than l) to file to.  The previous contents of file to are lost.
*/
cp_truncate(char *from, char *to, long l)
{
	FILE *f = fopen(from, "r");
	FILE *t = fopen(to, "w+");
	if (f == NULL || t == NULL)	
	{
		fclose(f);
		fclose(t);
		return -1;
	}
	char c;
	while (l-- > 0 && fread(&c, 1, 1, f) > 0)
		fwrite(&c, 1, 1, t);
	fclose(f);
	fclose(t);
	return 0;
}

 

system(char *s, char *t)
{
	return system(s, t, "");
}
system(char *s, char *t, char *u)
{
	return system(s, t, u, "");
}
system(char *s, char *t, char *u, char *v)
{
	char *buf = new char[strlen(s)+strlen(t)+strlen(u)+strlen(v)+4];
	sprintf(buf, "%s %s %s %s", s, t, u, v);
	int i = system(buf);
	delete buf;
	return i;
}

	
int copy_access(char *to, char *from)
{
	struct stat s;
	if (::stat(from, &s) < 0)
		return -1;
	if (chmod(to, s.st_mode&0777) < 0)
		return -1;
	return 0;
}
