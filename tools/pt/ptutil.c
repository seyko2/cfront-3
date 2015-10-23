/*ident	"@(#)cls4:tools/pt/ptutil.c	1.15" */
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

*******************************************************************************/
#include <stdio.h>
#include <ctype.h>
#include "pt.h"

#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>

#ifndef NO_LOCK
#ifdef LOCK_FCNTL
#include <fcntl.h>
#endif
#ifdef LOCK_FLOCK
#include <sys/file.h>
#endif
#ifdef LOCK_LOCKF
#include <fcntl.h>
#endif
#endif

/************************* UTILITIES *************************/

extern char* tdir;
extern int verbose;
extern long start_time;
extern char* PROGNAME;

/* get time */
long get_time()
{
	long time();
	return time((long)0);
}

/* fatal errors */
void fatal(msg, arg1, arg2)
char* msg;
char* arg1;
char* arg2;
{
	char buf[MAXLINE];
	void exit();

	sprintf(buf, msg, arg1, arg2);
	fprintf(stderr, "%s fatal error: %s\n", PROGNAME, buf);

	exit(1);
}

static long scnt = 0;
/* get space */
char* gs(s)
int s;
{
	char* p;
	char* malloc();

	if (s < 1)
		fatal("bad argument to gs()", (char*)0, (char*)0);

	if ((p = malloc(s)) == NULL)
		fatal("malloc() out of space", (char*)0, (char*)0);

	scnt++;

	return p;
}

/* free space */
void fs(p)
char* p;
{
	if (p == NULL)
		fatal("bad argument to fs()", (char*)0, (char*)0);
	if (scnt < 1)
		fatal("too many calls of fs()", (char*)0, (char*)0);

	free(p);

	scnt--;
}

/* copy a string */
char* copy(s)
char* s;
{
	char* p;

	if (s == NULL || !*s)
		fatal("bad argument to copy()", (char*)0, (char*)0);

	p = gs(strlen(s) + 1);

	strcpy(p, s);

	return p;
}

/* basename of a file */
char* basename(f, first)
char* f;
int first;
{
	char* s;

	if (f == NULL || !*f || first < 0 || first > 1)
		fatal("bad argument to basename()", (char*)0, (char*)0);

	s = f + strlen(f) - 1;
	while (s > f && *s != '/') {

		/* maybe clip extension */

		if (*s == '.' && first) {
			*s = 0;
			if (s > f && s[-1] == '.') {
				s[-1] = 0;
				s--;
			}
			first = 0;
		}
		s--;
	}
	if (s[0] == '/' && s[1])
		return s + 1;
	else
		return f;
}

#if 0
/* replace one file extension with another */
void replace_ext(f, ext)
char* f;
char* ext;
{
	char* s;
	char nwext[25];
	char* t;
	int i;

	if (f == NULL || !*f || ext == NULL || !*ext)
		fatal("bad argument to replace_ext()", (char*)0, (char*)0);

	s = f + strlen(f) - 1;
	while (s > f && *s != '.')
		s--;
	if (s == f)
		return;

	/* preserve case when replacing extension */

	strcpy(nwext, ext);
	t = s;
	i = 0;
	while (*t && nwext[i]) {
		if (isupper(*t) && islower(nwext[i]))
			nwext[i] = toupper(nwext[i]);
		else if (islower(*t) && isupper(nwext[i]))
			nwext[i] = tolower(nwext[i]);
		i++;
		t++;
	}
	strcpy(s, nwext);
}
#endif

/************************* FILE MANIPULATION *************************/

/* copy a file */
int cp(old, new)
char* old;
char* new;
{
	FILE* fpin;
	FILE* fpout;
	int c;

	if (old == NULL || !*old || new == NULL || !*new)
		fatal("bad argument to cp()", (char*)0, (char*)0);

	if ((fpin = fopen(old, "r")) == NULL)
		return -1;
	if ((fpout = fopen(new, "w")) == NULL) {
		fclose(fpin);
		return -1;
	}

	/* actual copy - needs to work on binary files */

	for (;;) {
		c = getc(fpin);
		if (feof(fpin))
			break;
		putc(c, fpout);
	}
	fclose(fpin);
	fclose(fpout);

	return 0;
}

/* move a file */
int mv(old, new, d1, d2)
char* old;
char* new;
char* d1;
char* d2;
{
	struct stat sb1;
	struct stat sb2;
	int flag;

	if (old == NULL || !*old || new == NULL || !*new ||
	    d1 == NULL || !*d1 || d2 == NULL || !*d2)
		fatal("bad argument to mv()", (char*)0, (char*)0);

	unlink(new);

	/* can we use links? */

	flag = 0;
	if (!strcmp(d1, d2))
		flag = 1;
	if (!flag) {
		if (stat(d1, &sb1) < 0 || stat(d2, &sb2) < 0)
			return -1;
		flag = (sb1.st_dev == sb2.st_dev);
	}

	/* links */

#if 0
	if (flag) {
		if (link(old, new) < 0 || unlink(old) < 0)
			return -1;
	}

	/* actual copy */

	else {
		if (cp(old, new) < 0)
			return -1;
		unlink(old);
	}
#else
	if (flag) {
		if (link(old, new) >= 0 && unlink(old) >= 0)
			return 0;
	}
	if (cp(old, new) < 0)
		return -1;
	unlink(old);
#endif
	return 0;
}

/* compare two files to see if they are the same physical file */
int icmp(f1, f2)
char* f1;
char* f2;
{
	struct stat sb1;
	struct stat sb2;

	if (f1 == NULL || !*f1 || f2 == NULL || !*f2)
		fatal("bad argument to icmp()", (char*)0, (char*)0);

	if (stat(f1, &sb1) < 0 || stat(f2, &sb2) < 0)
		return 0;
	return sb1.st_dev == sb2.st_dev && sb1.st_ino == sb2.st_ino;
}

/* get the device and inode for a file */
void get_devnum(f, dev, num)
char* f;
unsigned long* dev;
unsigned long* num;
{
	struct stat sb;

	if (f == NULL || !*f || dev == NULL || num == NULL)
		fatal("bad argument to get_devnum()", (char*)0, (char*)0);

	*dev = 0;
	*num = 0;
	if (stat(f, &sb) < 0)
		return;
	*dev = sb.st_dev;
	*num = sb.st_ino;
}

/* get current working directory */
void get_cwd(p)
char* p;
{
	char* getcwd();
	if (p == NULL)
		fatal("bad argument to get_cwd()", (char*)0, (char*)0);

	if (getcwd(p, MAXPATH) == NULL)
		fatal("could not get current working directory", (char*)0, (char*)0);
}

/* compare two files for equality */
int fcmp(f1, f2)
char* f1;
char* f2;
{
	FILE* fp1;
	FILE* fp2;
	int c1;
	int c2;

	if (f1 == NULL || !*f1 || f2 == NULL || !*f2)
		fatal("bad argument to fcmp()", (char*)0, (char*)0);

	if ((fp1 = fopen(f1, "r")) == NULL)
		return 0;
	if ((fp2 = fopen(f2, "r")) == NULL) {
		fclose(fp1);
		return 0;
	}
	for (;;) {
		c1 = getc(fp1);
		c2 = getc(fp2);
		if (c1 != c2 || c1 == EOF)
			break;
	}

	fclose(fp1);
	fclose(fp2);

	return c1 == c2;
}

/* delete a file */
void del_file(f)
char* f;
{
	if (f == NULL || !*f)
		fatal("bad argument to del_file()", (char*)0, (char*)0);

	unlink(f);
}

/* get a modification timestamp for a file;  0 if file does not exist */
unsigned long timestamp(s)
char* s;
{
	struct stat sb;

	if (s == NULL || !*s)
		fatal("bad argument to timestamp()", (char*)0, (char*)0);

	if (stat(s, &sb) < 0)
		return 0;
	if (sb.st_mtime == 0) {
		fprintf(stderr, "%s warning: ##### timestamp of 0 for %s #####\n", PROGNAME, s);
		return 1;
	}
	return sb.st_mtime;	
}

/* set the umask for file creation */
void set_umask(r)
char* r;
{
	struct stat sb;

	if (r == NULL || !*r)
		fatal("bad argument to set_umask()", (char*)0, (char*)0);
	if (stat(r, &sb) < 0)
		fatal("could not stat %s", r, (char*)0);

	umask(~sb.st_mode & 0777);
}

/* set repository permissions to those of the parent directory */
void set_rep_perms(r)
char* r;
{
	struct stat sb;

	if (r == NULL || !*r)
		fatal("bad argument to set_rep_perms()", (char*)0, (char*)0);

	/* change the mode */

	umask(0);
	if (stat(".", &sb) < 0)
		fatal("could not stat %s", ".", (char*)0);
	if (chmod(r, sb.st_mode & 0777) < 0)
		fatal("could not chmod %s", r, (char*)0);

	/* change the group - no error if doesn't work */

	chown(r, (int)getuid(), (int)sb.st_gid);
}

/* check whether a file exists */
int facc(f)
char* f;
{
	struct stat sb;

	if (f == NULL || !*f)
		fatal("bad argument to facc()", (char*)0, (char*)0);

	return stat(f, &sb) == 0;
}

/* hash a string */
unsigned long hash(s, n)
char* s;
long n;
{
	unsigned long h;

	if (s == NULL || !*s || n < 1)
		fatal("bad argument to hash()", (char*)0, (char*)0);

	h = 0;
	while (*s)
		h = h * 271 + *s++;
	return h % n;
}

/* give progress messages */
void progress(s, a1, a2)
char* s;
char* a1;
char* a2;
{
	char buf[MAXLINE];

	if (s == NULL || !*s)
		fatal("bad argument to progress()", (char*)0, (char*)0);
	if (!verbose)
		return;

	sprintf(buf, s, a1, a2);

	fprintf(stderr, "%s %s [%d] ...\n", PROGNAME, buf, get_time() - start_time);
	start_time = get_time();
}
void progress2()
{
	if (verbose)
		fprintf(stderr, "\n");
}

/************************* SIGNALS *************************/

#ifdef NO_SIGNAL
void signal_block()
{
}
void signal_unblock()
{
}
#else
typedef void (*T)();
static T old_int;
static T old_quit;
static int sig_flag = 0;
void signal_block(t)
T t;
{
	if (t == NULL)
		fatal("bad argument to signal_block()", (char*)0, (char*)0);

	if (sig_flag)
		fatal("nested call of signal_block()", (char*)0, (char*)0);
	old_int = (T)signal(SIGINT, t);
	old_quit = (T)signal(SIGQUIT, t);
	sig_flag = 1;
}
void signal_unblock()
{
	if (!sig_flag)
		fatal("signal_unblock() called with no prior block", (char*)0, (char*)0);
	signal(SIGINT, old_int);
	signal(SIGQUIT, old_quit);
	sig_flag = 0;
}
#endif

/************************* LOCKING *************************/

#ifdef LOCK_FCNTL
#undef LOCK_FLOCK
#undef LOCK_LOCKF
#endif

#ifdef LOCK_FLOCK
#undef LOCK_FCNTL
#undef LOCK_LOCKF
#endif

#ifdef LOCK_LOCKF
#undef LOCK_FCNTL
#undef LOCK_FLOCK
#endif

static int lock_cnt = 0;
static int lock_fd = -1;
static char* lock_str = "_lock";
#ifdef NO_LOCK
/* lock a file */
int lock_file(f)
char* f;
{
	return 0;
}
/* unlock a file */
int unlock_file()
{
	return 0;
}
/* lock a repository */
void lock_rep(r)
char* r;
{
}
/* unlock a repository */
void unlock_rep(r)
char* r;
{
}
#else
/* lock a file */
int lock_file(f)
char* f;
{
#ifdef LOCK_FCNTL
	struct flock fl;
#endif

	if (lock_fd < 0)
		lock_fd = open(f, O_WRONLY | O_CREAT, 0666);
	if (lock_fd < 0)
		fatal("could not get file descriptor to lock %s - possible permissions problem", f, (char*)0);
#ifdef LOCK_FCNTL
	fl.l_type = F_WRLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 0;

	return fcntl(lock_fd, F_SETLK, &fl);
#endif
#ifdef LOCK_FLOCK
	return flock(lock_fd, LOCK_EX | LOCK_NB);
#endif
#ifdef LOCK_LOCKF
	return lockf(lock_fd, F_TLOCK, 0);
#endif
}
/* unlock a file */
int unlock_file()
{
	int ret;

#ifdef LOCK_FCNTL
	struct flock fl;

	fl.l_type = F_UNLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 0;

	ret = fcntl(lock_fd, F_SETLK, &fl);
#endif
#ifdef LOCK_FLOCK
	ret= flock(lock_fd, LOCK_UN);
#endif
#ifdef LOCK_LOCKF
	ret = lockf(lock_fd, F_ULOCK, 0);
#endif
	if (ret < 0 || close(lock_fd) < 0) {
		lock_fd = -1;
		return -1;
	}

	lock_fd = -1;
	return 0;
}
/* lock a repository */
void lock_rep(r)
char* r;
{
	char fbuf[MAXPATH];
	int i;

	if (r == NULL || !*r)
		fatal("bad argument to lock_rep()", (char*)0, (char*)0);

	if (lock_cnt++ > 0)
		fatal("repository %s already locked", r, (char*)0);

	sprintf(fbuf, "%s/%s", r, lock_str);

	i = LOCK_MAX;
	while (i-- > 0) {
		if (lock_file(fbuf) >= 0)
			return;
		fprintf(stderr, "%s: waiting to lock repository %s, sleeping ...\n", PROGNAME, r);
		sleep(LOCK_SLEEP);
	}

	fatal("could not get lock on repository %s", r, (char*)0);
}
/* unlock a repository */
void unlock_rep(r)
char* r;
{
	if (r == NULL || !*r)
		fatal("bad argument to unlock_rep()", (char*)0, (char*)0);

	if (lock_cnt-- != 1)
		fatal("unlock_rep() called when not locked", (char*)0, (char*)0);

	if (unlock_file() < 0)
		fatal("cannot unlock repository %s", r, (char*)0);
}
#endif

/************************* SYSTEM AND POPEN *************************/

#ifndef SLOW_SYSTEM
/* fast system() function */
int fast_system(s)
char* s;
{
	int status;
	int pid;
	int w;
	char cbuf[MAXCBUF];
	char* cbufp[MAXCBUF/4];
	int n;
	char* t;
	int fdcnt;
	int fdlist[3][2];
	int fd;
	int i;
	int j;

	if (s == NULL || !*s)
		fatal("bad argument to fast_system()", (char*)0, (char*)0);

	/* split apart input */

	strcpy(cbuf, s);
	t = cbuf;
	n = 0;
	for (;;) {
		while (*t && *t <= ' ')
			t++;
		if (!*t)
			break;
		cbufp[n++] = t;
		while (*t > ' ')
			t++;
		if (*t)
			*t++ = 0;
	}
	cbufp[n] = 0;
	if (n < 2)
		fatal("missing argument to fast_system()", (char*)0, (char*)0);

	/* start up child */

	if ((pid = fork()) == 0) {

		/* handle I/O redirection */

		fdcnt = 0;
		for (i = 0; i < n; i++) {
			if (isdigit(cbufp[i][0]) && cbufp[i][1] == '>') {
				fd = creat(&cbufp[i][2], 0666);
				if (fd < 0)
					return -1;
				fdlist[fdcnt][0] = cbufp[i][0] - '0';
				fdlist[fdcnt][1] = fd;
				fdcnt++;
				cbufp[i] = 0;
			}
		}
		for (i = 0; i <= 2; i++) {
			for (j = 0; j < fdcnt; j++) {
				if (i == fdlist[j][0])
					break;
			}
			if (j < fdcnt) {
				close(i);
				dup(fdlist[j][1]);
				close(fdlist[j][1]);
			}
		}

		/* execute */

		execvp(cbufp[0], cbufp);
		_exit(127);
	}

	/* wait on child */

	while ((w = wait(&status)) != pid && w != -1)
		;
	if (w == -1)
		status = -1;

	return status;
}

/* fast popen() */
FILE* fast_popen(cmd, mode)
char* cmd;
char* mode;
{
	char fbuf[MAXPATH];
	char cmd2[MAXCBUF];

	if (cmd == NULL || !*cmd || mode == NULL || !*mode)
		fatal("bad argument to fast_popen()", (char*)0, (char*)0);

	/* dump into a temporary file */

	sprintf(fbuf, "%s/popen_buf", tdir);
	sprintf(cmd2, "%s 1>%s", cmd, fbuf);
	if (fast_system(cmd2))
		return NULL;

	/* return pointer to that file */

	return fopen(fbuf, "r");
}

/* fast pclose() */
int fast_pclose(fp)
FILE* fp;
{
	if (fp == NULL)
		fatal("bad argument to fast_pclose()", (char*)0, (char*)0);
	return fclose(fp);
}
#endif

/************************* STRING LISTS *************************/

/* add to a string list */
void slp_add(slp, name)
Slist* slp;
char* name;
{
	char** np;
	int i;

	if (slp == NULL || name == NULL || !*name)
		fatal("bad argument to slp_add()", (char*)0, (char*)0);

	/* first time */

	if (slp->n == 0) {
		slp->max = LISTSTART;
		slp->ptrs = (char**)gs(sizeof(char*) * slp->max);
	}

	/* grow list if have to */

	else if (slp->n >= slp->max) {
		np = (char**)gs(sizeof(char*) * slp->max * 2);
		for (i = 0; i < slp->n; i++)
			np[i] = slp->ptrs[i];
		slp->max *= 2;
		fs((char*)slp->ptrs);
		slp->ptrs = np;
	}
	slp->ptrs[slp->n++] = copy(name);
}

/* add to a string list if not already there */
int slp_adduniq(slp, name)
Slist* slp;
char* name;
{
	int i;
	char** p;

	if (slp == NULL || name == NULL || !*name)
		fatal("bad argument to slp_adduniq()", (char*)0, (char*)0);

	if (slp->n) {
		for (i = slp->n - 1, p = slp->ptrs + i; i >= 0; i--, p--) {
			if (!STRCMP(*p, name))
				return 0;
		}
	}

	slp_add(slp, name);

	return 1;
}

/* delete a string list */
void slp_del(slp)
Slist* slp;
{
	int i;

	if (slp == NULL)
		fatal("bad argument to slp_del()", (char*)0, (char*)0);
	if (slp->n < 0 || (slp->n > 0 && slp->ptrs == NULL))
		fatal("string list corrupted", (char*)0, (char*)0);

	for (i = 0; i < slp->n; i++)
		fs((char*)slp->ptrs[i]);
	if (slp->n > 0)
		fs((char*)slp->ptrs);
	slp->n = 0;
}

/* sort a list of strings */
void slp_sort(slp)
Slist* slp;
{
	int h;
	int i;
	int j;
	char* m;
	char** ms;

	if (slp == NULL)
		fatal("bad argument to slp_sort()", (char*)0, (char*)0);

	if (!slp->n)
		return;

	/* sort using a Shell sort */

	h = 1;
	ms = slp->ptrs - 1;
	do
		h = h * 3 + 1;
	while (h <= slp->n);
	do {
		h /= 3;
		for (i = h + 1; i <= slp->n; i++) {
			m = ms[i];
			j = i;
			while (strcmp(ms[j - h], m) > 0) {
				ms[j] = ms[j - h];
				j -= h;
				if (j <= h)
					break;
			}
			ms[j] = m;
		}
	} while (h != 1);
}

/* see if any members of p1 are found in p2 --> p3 */
int slp_eq(p1, p2, p3)
Slist* p1;
Slist* p2;
Slist* p3;
{
	int i;
	int j;

	if (p1 == NULL || p2 == NULL || p3 == NULL)
		fatal("bad argument to slp_eq()", (char*)0, (char*)0);

	for (i = 0; i < p1->n; i++) {
		for (j = 0; j < p2->n; j++) {
			if (!STRCMP(p1->ptrs[i], p2->ptrs[j])) {
				slp_add(p3, p1->ptrs[i]);
				break;
			}
		}
	}

	return p3->n > 0;
}

/* see if p1 is a subset of p2 with members that are not --> p3 */
int slp_subset(p1, p2, p3)
Slist* p1;
Slist* p2;
Slist* p3;
{
	int i;
	int j;

	if (p1 == NULL || p2 == NULL || p3 == NULL)
		fatal("bad argument to slp_subset()", (char*)0, (char*)0);

	for (i = 0; i < p1->n; i++) {
		for (j = 0; j < p2->n; j++) {
			if (!STRCMP(p1->ptrs[i], p2->ptrs[j]))
				break;
		}
		if (j == p2->n)
			slp_add(p3, p1->ptrs[i]);
	}

	return p3->n == 0;
}
