/*ident	"@(#)Path:v9-mktemp.c	3.1" */
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

char *
mktemp(as)
char *as;
{
        register char *s;
        register unsigned pid;
        register i;
        extern int errno;
        register int oerrno = errno;

        pid = getpid();
        s = as;
        while (*s++)
                ;
        s--;
        while (*--s == 'X') {
                *s = (pid%10) + '0';
                pid /= 10;
        }
        s++;
        i = 'a';
        while (access(as, 0) != -1) {
                if (i=='z')
                        return("/");
                *s = i++;
        }
        errno = oerrno;
        return(as);
}
