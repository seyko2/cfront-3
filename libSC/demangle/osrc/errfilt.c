/*ident	"@(#)cls4:tools/demangler/osrc/errfilt.c	1.1" */

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
/* If the filter is applied using the standard
 * shell's pipes, then to use c++filt one must
 * say:
 *
 * 	cc options 2>&1 | c++filt >&2
 *
 * But when this is done, the return code from
 * the cc command is lost.  Furthermore, the
 * stdout becomes the stderr.
 * This program is a kludge to alleviate these
 * problems.  It is invoked as follows:
 *
 * 	errfilt "cc $OPTS" c++filt
 *
 * Only the stderr is redirected to the pipe
 * for c++filt.  Furthermore, the return code
 * is what it should be for the cc command.
 */
#include <stdio.h>

main(argc, argv) int argc; char **argv; {
	int pfd[2],f = -1;
	int f2;
	int rc,rc2;
	int pid;

	if(argc != 3) {
		fprintf(stderr, "usage: %s string1 string2\n", argv[0]);
		return 0;
	}
	pipe(pfd);
	f = fork();

	/* filter program */
	if(f == 0) {
		close(0);
		dup(pfd[0]);
		close(1);
		dup(2);

		close(pfd[0]); close(pfd[1]);
		execl("/bin/sh","sh","-c",argv[2],0);
		exit(255);
	}
	f2 = fork();
	/* base program */
	if(f2 == 0) {
		close(2);
		dup(pfd[1]);

		close(pfd[1]); close(pfd[0]);
		execl("/bin/sh","sh","-c",argv[1],0);
		exit(255);
	}
	close(pfd[1]); close(pfd[0]);

	while((pid = wait(&rc)) != -1) {
		if(pid == f) {
			f = 0;
		}
		if(pid == f2) {
			f2 = 0;
			rc2 = rc;
		}
	}
	return rc2/0xff;
}
