/*ident	"@(#)cls4:tools/pt/ptcomp.c	1.9" */
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

char* tdir;
int verbose = 0;
long start_time;
char* PROGNAME = "CC[ptcomp]";

/************************* PROCESS A MAP FILE *************************/

typedef struct Map Map;
struct Map {
	char* name;
	char* file;
	Map* next;
	char rtype;
};

static Map* maph = NULL;
static Map* mapc = NULL;

/* strip quotes off a name */
char* strip(s, buf)
char* s;
char* buf;
{
	if (s == NULL || !*s || buf == NULL)
		fatal("bad argument to strip()", (char*)0, (char*)0);

	if (*s == '"' || *s == '<') {
		strcpy(buf, s + 1);
		buf[strlen(buf) - 1] = 0;
	}
	else {
		strcpy(buf, s);
	}

	return buf;
}

/* get the basename of a header file */
char* getbase(p, ilist)
char* p;
Slist* ilist;
{
	char* s;
	int len;
	int i;
	char buf[MAXPATH];
	char* orig;
	int maxlen;

	if (p == NULL || !*p || ilist == NULL)
		fatal("bad argument to getbase()", (char*)0, (char*)0);

	/* clip any prefix from -I list */

	orig = p;
	maxlen = 0;
	for (i = 0; i < ilist->n; i++) {
		s = ilist->ptrs[i];
		if (s[0] != '-' || s[1] != 'I')
			continue;
		s += 2;
		if (!strncmp(s, p, len = strlen(s)) &&
		    p[len] == '/' && len > maxlen) {
			maxlen = len;
		}
	}
	if (maxlen)
		p += maxlen + 1;

	/* clip off any ./ */

	if (!maxlen && p[0] == '.' && p[1] == '/')
		p += 2;

	/* see if file refers to local directory */

	if ((!maxlen && orig[0] != '/') || icmp(basename(orig, 0), orig))
		sprintf(buf, "\"%s\"", p);
	else
		sprintf(buf, "<%s>", p);

	return copy(buf);
}

/* read raw cfront output */
void readcfout(f, ilist)
char* f;
Slist* ilist;
{
	FILE* fp;
	char* flds[3];
	char* s;
	char* start;
	int i;
	Map* p;
	char fbuf[MAXLINE];
	char t;
	char buf[MAXLINE];

	if (f == NULL || !*f || ilist == NULL)
		fatal("bad argument to readcfout()", (char*)0, (char*)0);
	if ((fp = fopen(f, "r")) == NULL)
		fatal("could not open map file %s for reading", f, (char*)0);

	/* iterate over type records and pick apart fields */

	while (fgets(fbuf, MAXLINE, fp) != NULL) {
		s = fbuf;
		for (i = 0; i <= 2; i++) {
			while (*s && *s <= ' ')
				s++;
			start = s;
			while (*s && *s > ' ')
				s++;
			if (s == start)
				break;
			*s++ = 0;
			flds[i] = start;
		}
		if (i != 3) {
			fprintf(stderr, "%s warning: ##### illegally formatted record in map file %s, ignoring #####\n", PROGNAME, f);
			continue;
		}
		p = (Map*)gs(sizeof(Map));
		t = flds[0][0];
		if (t != 'c' && t != 's' && t != 'u' &&
		    t != 'f' && t != 't' && t != 'e') {
			sprintf(buf, "bad record type \"%c\" in map file %s", t, f);
			fatal(buf, (char*)0, (char*)0);
		}

		/* add record to list */

		p->rtype = t;
		p->name = copy(flds[1]);
		p->file = getbase(flds[2], ilist);
		p->next = NULL;
		if (maph == NULL) {
			maph = p;
			mapc = maph;
		}
		else {
			mapc->next = p;
			mapc = p;
		}
	}

	fclose(fp);
}

/************************* TYPE LISTS *************************/

typedef struct Tfile Tfile;
struct Tfile {
	char* name;
	Slist files;
	Slist bns;
	Tfile* next;
	char type;
	char deleted;
};
static Tfile* tftab[TFTABSIZE2];
static Slist tab;

/* read a map file */
void readtf(f, failok)
char* f;
int failok;
{
	FILE* fp;
	char buf[MAXMAPL];
	char lbuf[MAXLINE];
	int ln;
	char* s;
	Slist* slp;
	Tfile* p;
	int h;
	char* t;
	Slist* slp2;
	int err;
	char* start;
	int len;
	int flag;
	int indx;

	if (f == NULL || !*f)
		fatal("bad argument to readtf()", (char*)0, (char*)0);
	if ((fp = fopen(f, "r")) == NULL && !failok)
		fatal("cannot open map file %s for reading", f, (char*)0);
	if (fp == NULL)
		return;

	ln = 0;
	slp = NULL;

	/* iterate over lines of file */

	while (fgets(buf, MAXMAPL, fp) != NULL) {
		ln++;
		len = strlen(buf) - 1;
		if (buf[len] != '\n') {
			sprintf(lbuf, "%d", ln);
			fatal("missing newline at line %s of map file %s", lbuf, f);
		}
		buf[len] = 0;
		s = buf;
		while (*s && *s <= ' ')
			s++;
		if (!*s)
			continue;

		/* might be string table */

		if (!strcmp(s, "@tab")) {
			flag = 0;
			while (fgets(buf, MAXMAPL, fp) != NULL) {
				ln++;
				buf[strlen(buf) - 1] = 0;
				if (!strcmp(buf, "@etab")) {
					flag = 1;
					break;
				}
				slp_add(&tab, buf);
			}
			if (!flag || !tab.n) {
				sprintf(lbuf, "%d", ln);
				fatal("incorrectly formatted map file %s at line %s", f, lbuf);
			}
			continue;
		}

		/* check formatting */

		err = 0;
		if (s[0] == '@') {
			if (strncmp(s, "@dec", 4) && strncmp(s, "@def", 4))
				err = 1;
			if (!err && s[4] != ' ')
				err = 1;
			t = s + 5;
			while (*t && *t <= ' ')
				t++;
			if (!err && !isalpha(*t) && *t != '_')
				err = 1;
			if (err) {
				sprintf(lbuf, "%d", ln);
				fatal("incorrectly formatted map file %s at line %s", f, lbuf);
			}
		}

		/* start of a new type */

		if (s[0] == '@') {
			if (slp != NULL && slp->n < 1) {
				sprintf(lbuf, "%d", ln);
				fatal("no files found for type in map file %s at line %s", f, lbuf);
			}
			p = (Tfile*)gs(sizeof(Tfile));
			p->type = (s[3] == 'f' ? 'F' : 'C');;

			/* get type name */

			t = s + 5;
			while (*t && *t <= ' ')
				t++;
			while (isalnum(*t) || *t == '_')
				t++;
			if (*t)
				*t++ = 0;
			else
				*t = 0;
			p->name = copy(s + 5);
			slp = &p->files;
			slp->n = 0;

			/* get list of basenames */

			slp2 = &p->bns;
			slp2->n = 0;
			for (;;) {
				while (*t && *t <= ' ')
					t++;
				if (!*t)
					break;
				start = t;
				while (*t && *t > ' ')
					t++;
				if (*t)
					*t++ = 0;
				else
					*t = 0;
				if (start[0] == '\\' && !start[1]) {
					if (fgets(buf, MAXMAPL, fp) == NULL)
						fatal("premature EOF for map file %s", f, (char*)0);
					ln++;
					len = strlen(buf) - 1;
					if (buf[len] != '\n') {
						sprintf(lbuf, "%d", ln);
						fatal("missing newline at line %s of map file %s", lbuf, f);
					}
					buf[len] = 0;
					t = buf;
				}
				else {
					if (start[0] == '@' && isdigit(start[1])) {
						indx = atoi(start + 1);
						if (indx < 0 || indx >= tab.n) {
							sprintf(lbuf, "%d", ln);
							fatal("index for string table out of range at line %s of map file %s", lbuf, f);
						}
						slp_add(slp2, tab.ptrs[indx]);
					}
					else {
						slp_add(slp2, start);
						slp_adduniq(&tab, start);
					}
				}
			}
			p->deleted = 0;
			h = (int)hash(p->name, (long)TFTABSIZE2);
			p->next = tftab[h];
			tftab[h] = p;			
		}

		/* another file for preceding type */

		else {
			if (slp == NULL) {
				sprintf(lbuf, "%d", ln);
				fatal("incorrectly formatted map file %s at line %s", f, lbuf);
			}
			slp_add(slp, s);
		}
	}
	if (slp != NULL && slp->n < 1)
		fatal("premature EOF for map file %s", f, (char*)0);

	fclose(fp);
}

/************************* TABLE OF FILENAMES SEEN *************************/

typedef struct Fname Fname;
struct Fname {
	char* name;
	Fname* next;
};
static Fname* ftab[FTSIZE2];

/* look up a filename */
Fname* lookup(s)
char* s;
{
	Fname* p;

	if (s == NULL || !*s)
		fatal("bad argument to lookup()", (char*)0, (char*)0);

	p = ftab[(int)hash(s, (long)FTSIZE2)];
	while (p != NULL) {
		if (!STRCMP(s, p->name))
			return p;
		p = p->next;
	}

	return NULL;
}

/* add a filename */
void add(s)
char* s;
{
	Fname* p;
	int h;

	if (s == NULL || !*s)
		fatal("bad argument to add()", (char*)0, (char*)0);

	p = ftab[h = (int)hash(s, (long)FTSIZE2)];
	while (p != NULL) {
		if (!strcmp(s, p->name))
			return;
		p = p->next;
	}
	if (p == NULL) {
		p = (Fname*)gs(sizeof(Fname));
		p->next = ftab[h];
		ftab[h] = p;
		p->name = copy(s);
	}
}

/************************* TABLE OF BASENAMES *************************/

typedef struct Bnlist Bnlist;
struct Bnlist {
	char* name;
	char* indx;
	Bnlist* next;
};
static Bnlist* bntab[BNSIZE];

/* look up a basename in string table list */
char* bn_lookup(s)
char* s;
{
	int i;
	static int first_flag = 0;
	Bnlist* p;
	int h;
	char buf[MAXLINE];

	if (s == NULL || !*s)
		fatal("bad argument to bn_lookup()", (char*)0, (char*)0);

	/* set up table */

	if (!first_flag++) {
		for (i = 0; i < tab.n; i++) {
			h = (int)hash(tab.ptrs[i], (long)BNSIZE);
			p = (Bnlist*)gs(sizeof(Bnlist));
			p->name = copy(tab.ptrs[i]);
			sprintf(buf, "@%d", i);
			p->indx = copy(buf);
			p->next = bntab[h];
			bntab[h] = p;
		}
	}

	/* look up basename */

	h = (int)hash(s, (long)BNSIZE);
	p = bntab[h];
	while (p != NULL && strcmp(s, p->name))
		p = p->next;
	if (p != NULL)
		return p->indx;
	else
		fatal("basename %s not found in basename list", s, (char*)0);
	return NULL;
}

/*****************************************************************************/

main(argc, argv)
int argc;
char* argv[];
{
	char sb1[MAXPATH];
	char sb2[MAXPATH];
	char* bn;
	char fbuf[MAXPATH];
	char fbuf2[MAXPATH];
	Map* p;
	int i;
	FILE* fp;
	char* rep;
	char* tout;
	char* nmap;
	char* progname;
	int j;
	Tfile* tfp;
	int h;
	char* n;
	int lock_flag;
	int len;
	int len2;
	int dflag;
	char* fincl;
	int flag;
	Slist ilist;
	char* s;
	char buf3[MAXPATH];

	start_time = get_time();

	/* check out arguments */

	lock_flag = 1;
	dflag = 0;
	while (argc >= 2 && !strncmp(argv[1], "-pt", 3)) {
		switch (argv[1][3]) {
			case 'l':
				lock_flag = 0;
				break;
			case 'v':
				verbose = 1;
				break;
			case 'd':
				dflag = 1;
				break;
			default:
				break;		/* ignore bad options */
		}
		argc--;
		argv++;
	}
	if (argc != 6)
		fatal("usage is: ptcomp [-ptv] repository toutfile nmap progname incl", (char*)0, (char*)0);
	if (!lock_flag)
		verbose = 0;
	progress2();
	rep = argv[1];
	tout = argv[2];
	nmap = argv[3];
	progname = basename(copy(argv[4]), 1);
	fincl = argv[5];
	if (!*progname)
		fatal("progname passed to ptcomp is empty string", (char*)0, (char*)0);

	/* maybe change permissions on repository */

	if (dflag)
		set_rep_perms(rep);

	/* set umask */

	set_umask(rep);

	/* lock repository */

	if (lock_flag) {
		lock_rep(rep);
		progress("locked repository", (char*)0, (char*)0);
	}

	/* read in include list */

	if ((fp = fopen(fincl, "r")) == NULL)
		fatal("cannot open include list %s for reading", fincl, (char*)0);
	ilist.n = 0;
	while (fgets(buf3, MAXPATH, fp) != NULL) {
		s = buf3;
		s[strlen(s) - 1] = 0;
		slp_add(&ilist, s);
	}
	fclose(fp);

	/* read new raw cfront information */

	readcfout(tout, &ilist);
	progress("read raw cfront information", (char*)0, (char*)0);

	/* read old map file */

	sprintf(fbuf, "%s/%s", rep, nmap);
	readtf(fbuf, 1);
	progress("read old map file", (char*)0, (char*)0);

	/* make list of unique filenames in new map file */

	p = maph;
	while (p != NULL) {
		add(strip(p->file, sb1));
		p = p->next;
	}
	progress("made list of unique filenames in new map file", (char*)0, (char*)0);

	/* delete all entries from old map file that have a */
	/* basename matching the current program name being compiled */

	for (i = 0; i < TFTABSIZE2; i++) {
		tfp = tftab[i];
		while (tfp != NULL) {
			for (j = 0; j < tfp->bns.n; j++) {
				if (!STRCMP(progname, tfp->bns.ptrs[j]))
					break;
			}
			if (j < tfp->bns.n) {
				if (tfp->files.n == 1 &&
				    lookup(strip(tfp->files.ptrs[0], sb1)) != NULL) {
					if (tfp->bns.n == 1) {
						tfp->deleted = 1;
						slp_del(&tfp->files);
						slp_del(&tfp->bns);
					}
					else {
						fs((char*)tfp->bns.ptrs[j]);
						tfp->bns.ptrs[j] = tfp->bns.ptrs[tfp->bns.n - 1];
						tfp->bns.n--;
					}
				}
			}
			tfp = tfp->next;
		}
	}
	progress("deleted old map file entries", (char*)0, (char*)0);

	/* add in new entries */

	p = maph;
	slp_adduniq(&tab, progname);
	while (p != NULL) {
		tfp = tftab[h = (int)hash(p->name, (long)TFTABSIZE2)];
		flag = 0;
		for (; tfp != NULL; tfp = tfp->next) {

			/* skip entries with wrong names and @def entries */

			if (strcmp(tfp->name, p->name) || tfp->type == 'F')
				continue;

			/* if a function template or > 1 header or */
			/* header matches, then merge in */

			if (p->rtype == 'f' || tfp->files.n != 1 ||
			    !strcmp(strip(tfp->files.ptrs[0], sb1),
				strip(p->file, sb2))) {
				tfp->deleted = 0;
				slp_adduniq(&tfp->bns, progname);
				strip(p->file, sb1);
				for (i = 0; i < tfp->files.n; i++) {
					strip(tfp->files.ptrs[i], sb2);
					if (!strcmp(sb1, sb2)) {
						fs((char*)tfp->files.ptrs[i]);
						tfp->files.ptrs[i] = copy(p->file);
						break;
					}
				}
				if (i == tfp->files.n)
					slp_adduniq(&tfp->files, p->file);
				flag = 1;
			}
			if (flag)
				break;
		}

		/* not previously seen;  create new entry */

		if (!flag) {
			tfp = (Tfile*)(gs(sizeof(Tfile)));
			tfp->name = copy(p->name);
			tfp->files.n = 0;
			tfp->bns.n = 0;
			slp_add(&tfp->files, p->file);
			slp_add(&tfp->bns, progname);
			tfp->deleted = 0;
			tfp->type = 'C';
			tfp->next = tftab[h];
			tftab[h] = tfp;
		}
		p = p->next;
	}
	progress("added in new map entries", (char*)0, (char*)0);

	/* back up the defmap file */

	signal_block(SIG_IGN);
	sprintf(fbuf, "%s/%s", rep, nmap);
	sprintf(fbuf2, "%s/%s.old", rep, nmap);
	if (facc(fbuf) && mv(fbuf, fbuf2, rep, rep) < 0)
		fatal("could not move file %s --> %s", fbuf, fbuf2);

	/* write the new formatted file */

	if ((fp = fopen(fbuf, "w")) == NULL)
		fatal("cannot open map file %s for writing", fbuf, (char*)0);

	/* string table */

	fprintf(fp, "@tab\n");
	for (i = 0; i < tab.n; i++)
		fprintf(fp, "%s\n", tab.ptrs[i]);
	fprintf(fp, "@etab\n");

	/* individual type entries */

	for (i = 0; i < TFTABSIZE2; i++) {
		for (tfp = tftab[i]; tfp != NULL; tfp = tfp->next) {
			if (tfp->deleted)
				continue;
			n = (tfp->type == 'F' ? "ef" : "ec");
			fprintf(fp, "@d%s %s", n, tfp->name);
			len = 5 + strlen(tfp->name);
			for (j = 0; j < tfp->bns.n; j++) {
				bn = bn_lookup(tfp->bns.ptrs[j]);
				len2 = strlen(bn);
				if (len + 1 + len2 > 75) {
					len = strlen(bn) + 4;
					fputs(" \\\n    ", fp);
				}
				else {
					putc(' ', fp);
					len += 1 + len2;
				}
				fputs(bn, fp);
			}
			putc('\n', fp);
			for (j = 0; j < tfp->files.n; j++) {
				fputs(tfp->files.ptrs[j], fp);
				putc('\n', fp);
			}
		}
	}
	fclose(fp);
	progress("wrote new map file", (char*)0, (char*)0);

	/* unlock repository */

	if (lock_flag) {
		unlock_rep(rep);
		progress("unlocked repository", (char*)0, (char*)0);
	}

	progress2();

	exit(0);
}
