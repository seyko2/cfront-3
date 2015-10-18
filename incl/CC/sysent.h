/*ident	"@(#)cls4:incl-master/proto-headers/sysent.h	1.1" */

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

#ifndef __SYSENT_H
#define __SYSENT_H

#ifndef __TYPES_H
#include <sys/types.h>
#endif


#ifndef __WAIT_H
#include <sys/wait.h>
#endif


#ifndef __UNISTD_H
#include <unistd.h>
#endif

extern "C" {
	void _exit(int);
	int access(const char*, int);
	int acct(const char*);
	int chdir(const char*);
	int chroot(const char*);
	int close(int);
	int dup(int);
	void exit(int);
	int ioctl(int, int ...);
	int link(const char*, const char*);
	int nice(int);
	int pause();
	int pipe(int*);
	void sync();
	int unlink(const char*);
	int system(const char*);
	int execl(const char*, const char* ...);
//	int execle(const char*, const char* ...);
	int execlp(const char*, const char* ...);
	int execv(const char*, const char*[]);
	int execve(const char*, const char*[], const char*[]);
	int execvp(const char*, const char*[]);

	int chmod(const char*, mode_t);
//	int read(int, void*, unsigned);
	mode_t umask(mode_t);
	int fchmod(int, mode_t);
	int creat(const char*, mode_t);
	int mkdir(const char*, mode_t);
	off_t lseek(int, off_t, int);
//	int write(int, const void*, unsigned);
}

#endif
