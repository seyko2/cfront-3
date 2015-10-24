/*ident	"@(#)Path:tmppath.c	3.1" */
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

#include "Pathlib.h"
#include <Tmppath.h>

//#include <stdio.h>
//#ifndef P_tmpdir
//#define P_tmpdir 0
//#endif

// WARNING: statically initialized objects!
static Path usedir;
static String usepfx;

static int diron = 0;
static int pfxon = 0;

static int pid = -1;

void Tmppath::use() {
	diron = pfxon = 0;
}

void Tmppath::use(const char * pfx) {
	diron = 0;
	pfxon = 1;
	usepfx = pfx;
}

void Tmppath::use(const Path & dir) {
	pfxon = 0;
	diron = 1;
	usedir = dir;
}

void Tmppath::use(const Path & dir, const char * pfx) {
	diron = pfxon = 1;
	usedir = dir;
	usepfx = pfx;
}

void Tmppath::usepid(int pid_) {
	pid = pid_;
}

static char cookie[] = { 'a', 'a', 'a', '\0' };
static const int numcookies = 17576;  // ahem, 26**3

static void 
advance_cookie() {
	char *q = cookie;
	while(*q == 'z')
		*q++ = 'a';
	if (*q != '\0')
		++*q;
}

static void 
makeapath(String &ret, const char *dir, const char *pfx, const char *pidbuf, const char *ext) {
	ret = dir;
	ret += '/';
	ret += pfx;
	ret += cookie;
	ret += pidbuf;
	ret += ext;
	advance_cookie();
}

int
Tmppath::build(const char *dir, const char *pfx, const char *ext) {
	char pidbuf[10];
	sprintf(pidbuf, "%d", (pid == -1? getpid() : pid));
	int tries = 0;
	makeapath(rep, dir, pfx, pidbuf, ext);
	while (ksh_test(rep)) { 
		if (++tries > numcookies) {
			rep.assign(".", 1);
			return 0;
		}
		makeapath(rep, dir, pfx, pidbuf, ext);
        }
	return 1;
}

Tmppath::Tmppath() {
	if (diron) {
		if (pfxon)
			construct(usedir, usepfx, "");
		else
			construct(usedir, "", "");
	}
	else {
		if (pfxon)
			construct(usepfx, "");
		else
			construct("", "");
	}
}

Tmppath::Tmppath(const char * pfx, const char * ext) {
	if (diron)
		construct(usedir, pfx, ext);
	else
		construct(pfx, ext);
}


Tmppath::Tmppath(const Path & dir) {
	if (pfxon)
		construct(dir, usepfx, "");
	else
		construct(dir, "", "");
}

Tmppath::Tmppath(const Path & dir, const char * pfx, const char * ext) {
	construct(dir, pfx, ext);
}

void
Tmppath::construct(const char * pfx, const char * ext) {
	const char *TMPDIR = getenv("TMPDIR");
		           // null paths are not legal
	if (TMPDIR != 0 && *TMPDIR != '\0') {
		if (!build(TMPDIR, pfx, ext))
			if (strcmp(TMPDIR, "/usr/tmp") == 0 || !build("/usr/tmp", pfx, ext))
				if (strcmp(TMPDIR, "/tmp") != 0)
					build("/tmp", pfx, ext);
	}
	else {
		if (!build("/usr/tmp", pfx, ext))
			build("/tmp", pfx, ext);
	}
	canonicalize();
}

void
Tmppath::construct(const Path & dir, const char * pfx, const char * ext) {
	build(dir, pfx, ext);
	canonicalize();
}
