/*ident	"@(#)cls4:tools/demangler/dem.c	1.13" */
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
#include "dem.h"

/************************* CUSTOMIZATION SECTION *************************/

#if vax || tahoe || (sun && !(solaris || svr4 || i386)) || pyr
#define CLIP_UNDER			/* ignore first "_" on names */
#endif

#define SP_ALIGN 0x4			/* alignment of dynamic space blocks */

/*#define DEM_MAIN*/			/* set if want standalone program */

/************************************************************************/

#define MAXLINE 1024			/* general buffer use */

#define MAXARG 100			/* max arguments in a function */

#define STRCMP(s, t) ((s)[0] != (t)[0] || strcmp((s), (t)) != 0)

static char* spbase;
static char cc;
static char* base;
static int baselen;
#define gc() {cc = baselen >= 1 ? *base++ : 0, baselen--;}

static int waserror = 0;

#define MAXSTACK 100
static char* stackp[MAXSTACK];
static int stackl[MAXSTACK];
static char stackc[MAXSTACK];
static int sp = -1;

#define ERROR() {waserror = 1; return NULL;}

#define SKIP_WS 1
#define SKIP_ALL 2

/************************* UTILITIES *************************/

/* fatal errors */
static void fatal(msg, arg1, arg2)
char* msg;
char* arg1;
char* arg2;
{
	char buf[MAXLINE];

	sprintf(buf, msg, arg1, arg2);
	fprintf(stderr, "demangle fatal error: %s\n", buf);

	exit(1);
}

/* get space */
static char* gs(s)
int s;
{
	char* p;

	if (s < 1)
		fatal("bad argument to gs()", (char*)0, (char*)0);

	/* align space on SP_ALIGN boundary */

	while ((unsigned long)spbase & (SP_ALIGN - 1))
		spbase++;

	p = spbase;
	spbase += s;

	return p;
}

/* copy a string */
static char* copy(s)
char* s;
{
	char* p;

	if (s == NULL || !*s)
		fatal("bad argument to copy()", (char*)0, (char*)0);

	p = gs(strlen(s) + 1);
	strcpy(p, s);
	return p;
}

/************************* DEMANGLE UTILITIES *************************/

/* push a string to scan */
static void push(s, n)
char* s;
int n;
{
	if (s == NULL || !*s || n < 1)
		fatal("bad argument to push()", (char*)0, (char*)0);
	if (sp + 1 >= MAXSTACK)
		fatal("overflow of stack in push()", (char*)0, (char*)0);

	sp++;
	stackp[sp] = base;
	stackl[sp] = baselen;
	stackc[sp] = cc;
	base = s;
	baselen = n;
	gc();
}

static void pop()
{
	if (sp < 0)
		fatal("bad argument to pop()", (char*)0, (char*)0);

	base = stackp[sp];
	baselen = stackl[sp];
	cc = stackc[sp];
	sp--;
}

/************************* DEMANGLER *************************/

/* get a class name */
static DEMARG* getarglist();
static DEMCL* getclass()
{
	int n;
	char nbuf[MAXLINE];
	int i;
	int j;
	int iter;
	DEMCL* p;
	DEMCL* clhead;
	DEMCL* curr;
	DEMARG* ap;

	iter = 1;
	clhead = NULL;
	curr = NULL;

	/* fix for ambiguity in encoding */

	i = 0;
	if (isdigit(base[0])) {
		i = 1;
		if (isdigit(base[1]))
			i = 2;
	}
	if (isdigit(cc) && base[i] == 'Q' && isdigit(base[i + 1]) &&
	    base[i + 2] == '_') {
		gc();
		if (i)
			gc();
		if (i == 2)
			gc();
	}

	/* might be nested class */

	if (cc == 'Q') {
		gc();
		if (!isdigit(cc))
			ERROR();
		iter = cc - '0';
		if (iter < 1)
			ERROR();
		gc();
		if (cc != '_')
			ERROR();
		gc();
	}

	/* grab number of classes expected */

	while (iter-- > 0) {

		/* get a class */

		if (!isdigit(cc))
			ERROR();
		n = cc - '0';
		gc();
		if (isdigit(cc)) {
			n = n * 10 + cc - '0';
			gc();
		}
		if (isdigit(cc)) {
			n = n * 10 + cc - '0';
			gc();
		}
		if (n < 1)
			ERROR();
		for (i = 0; i < n; i++) {
			if (!isalnum(cc) && cc != '_')
				ERROR();
			nbuf[i] = cc;
			gc();
		}
		nbuf[i] = 0;
		p = (DEMCL*)gs(sizeof(DEMCL));
		p->rname = copy(nbuf);
		p->clargs = NULL;

		/* might be a template class */

		for (j = 0; j < i; j++) {
			if (nbuf[j] == '_' && nbuf[j + 1] == '_' &&
			    nbuf[j + 2] == 'p' && nbuf[j + 3] == 't')
				break;
		}
		if (j == 0)
			ERROR();
		if (j == i) {
			p->name = copy(nbuf);
		}
		else {
			if (nbuf[j + 4] != '_' || nbuf[j + 5] != '_')
				ERROR();
			nbuf[j] = 0;
			p->name = copy(nbuf);
			j += 6;
			if (!isdigit(nbuf[j]))
				ERROR();
			n = nbuf[j] - '0';
			j++;
			if (isdigit(nbuf[j])) {
				n = n * 10 + nbuf[j] - '0';
				j++;
			}
			if (isdigit(nbuf[j])) {
				n = n * 10 + nbuf[j] - '0';
				j++;
			}
			if (n < 2)
				ERROR();
			if (nbuf[j] != '_')
				ERROR();
			j++;
			n--;
			if (!nbuf[j])
				ERROR();

			/* get arguments for template class */

			push(nbuf + j, n);
			if ((ap = getarglist()) == NULL || cc)
				ERROR();
			pop();
			p->clargs = ap;
		}
		p->next = NULL;

		/* link in to list */

		if (clhead != NULL) {
			curr->next = p;
			curr = p;
		}
		else {
			clhead = p;
			curr = clhead;
		}
	}

	return clhead;
}

/* copy an argument */
static DEMARG* arg_copy(p)
DEMARG* p;
{
	DEMARG* p2;

	if (p == NULL)
		fatal("bad argument to arg_copy()", (char*)0, (char*)0);

	p2 = (DEMARG*)gs(sizeof(DEMARG));
	p2->mods = p->mods;
	p2->base = p->base;
	p2->arr = p->arr;
	p2->func = p->func;
	p2->clname = p->clname;
	p2->mname = p->mname;
	p2->lit = p->lit;
	p2->ret = p->ret;
	p2->next = NULL;

	return p2;
}

/* get an argument */
static DEMARG* getarg(acmax, arg_cache, ncount)
int acmax;
DEMARG* arg_cache[];
int* ncount;
{
	char mods[100];
	int mc;
	int type;
	static DEMARG* p;
	DEMCL* clp;
	long n;
	DEMARG* farg;
	DEMARG* fret;
	DEMARG* getarglist();
	char litbuf[MAXLINE];
	int lp;
	int foundx;
	long arrdim[100];
	int arrp;
	int i;
	int wasm;
	int waslm;
	char buf[MAXLINE];
	char buf2[MAXLINE];
	void dem_printcl();
	DEMCL* clist[100];
	int clc;
	int ic;

	/* might be stuff remaining from Nnn */

	if (ncount != NULL && *ncount > 0) {
		(*ncount)--;
		return arg_copy(p);
	}

	mc = 0;
	type = 0;
	clp = NULL;
	farg = NULL;
	fret = NULL;
	lp = 0;
	foundx = 0;
	arrp = 0;
	wasm = 0;
	clc = 0;

	/* get type */

	while (!type) {
		switch (cc) {

			/* modifiers and declarators */

			case 'X':
				gc();
				foundx = 1;
				break;
			case 'U':
			case 'C':
//			case 'V':	// volatile
//			case 'S':	// signed
			case 'P':
			case 'R':
				mods[mc++] = cc;
				gc();
				break;

			/* fundamental types */

			case 'v':
			case 'c':
			case 's':
			case 'i':
			case 'l':
			case 'f':
			case 'd':
			case 'V':
			case 'r':
			case 'e':
				type = cc;
				gc();
				break;

			/* arrays */

			case 'A':
				mods[mc++] = cc;
				gc();
				if (!isdigit(cc))
					ERROR();
				n = cc - '0';
				gc();
				while (isdigit(cc)) {
					n = n * 10 + cc - '0';
					gc();
				}
				if (cc != '_')
					ERROR();
				gc();
				arrdim[arrp++] = n;
				break;

			/* functions */

			case 'F':
				type = cc;
				gc();
				if ((farg = getarglist()) == NULL)
					ERROR();
				if (cc != '_')
					ERROR();
				gc();
				if ((fret = getarg(-1, (DEMARG**)0, (int*)0)) == NULL)
					ERROR();
				break;

			/* pointers to member */

			case 'M':
				mods[mc++] = cc;
				wasm = 1;
				gc();
				if ((clist[clc++] = getclass()) == NULL)
					ERROR();
				break;

			/* repeat previous argument */

			case 'T':
				gc();
tcase:
				if (!isdigit(cc))
					ERROR();
				n = cc - '0';
				gc();
#if 0
				if (isdigit(cc)) {
					n = n * 10 + cc - '0';
					gc();
				}
#endif
				if (n < 1)
					ERROR();
				if (arg_cache == NULL || n - 1 > acmax)
					ERROR();
				p = arg_copy(arg_cache[n - 1]);
				return p;

			/* repeat previous argument N times */

			case 'N':
				gc();
				if (!isdigit(cc))
					ERROR();
				if (ncount == NULL)
					ERROR();
				*ncount = cc - '0' - 1;
				if (*ncount < 0)
					ERROR();
				gc();
				goto tcase;

			/* class, struct, union, enum */

			case '1': case '2': case '3': case '4': case '5':
			case '6': case '7': case '8': case '9': case 'Q':
				if ((clp = getclass()) == NULL)
					ERROR();
				type = 'C';
				break;

			default:
				return NULL;
		}
	}

	/* template literals */

	if (type && foundx) {
		n = 0;
		waslm = 0;
		if (cc == 'L' && base[0] == 'M') {
			gc();
			gc();
			while (cc != '_' && cc)
				gc();
			if (!cc)
				ERROR();
			gc();
			while (cc != '_' && cc)
				gc();
			if (!cc)
				ERROR();
			gc();
			n = cc - '0';
			gc();
			if (isdigit(cc)) {
				n = n * 10 + cc - '0';
				gc();
			}
			if (isdigit(cc)) {
				n = n * 10 + cc - '0';
				gc();
			}
			waslm = 1;
		}
		else if (cc == 'L') {
			gc();
			if (!isdigit(cc))
				ERROR();
			n = cc - '0';
			gc();
			if (isdigit(cc) && base[0] == '_') {
				n = n * 10 + cc - '0';
				gc();
				gc();
			}
			if (cc == 'n') {
				gc();
				n--;
				litbuf[lp++] = '-';
			}
		}
		else if (cc == '0') {
			n = 1;
		}
		else if (isdigit(cc)) {
			n = cc - '0';
			gc();
			if (isdigit(cc)) {
				n = n * 10 + cc - '0';
				gc();
			}
		}
		else {
			ERROR();
		}
		if (!n && waslm) {
			strcpy(litbuf, "0");
			lp = 1;
		}
		else {
			ic = -1;
			while (n-- > 0) {
				if (!isalnum(cc) && cc != '_')
					ERROR();
				litbuf[lp++] = cc;
				gc();
				if (n > 0 && lp >= 2 &&
				    litbuf[lp - 1] == '_' && litbuf[lp - 2] == '_') {
					if ((clist[ic = clc++] = getclass()) == NULL)
						ERROR();
					litbuf[lp - 1] = 0;
					litbuf[lp - 2] = 0;
					lp -= 2;
					break;
				}	
			}
			litbuf[lp] = 0;
			if ((wasm && waslm) || ic >= 0) {
				dem_printcl(clist[ic >= 0 ? ic : 0], buf2);
				sprintf(buf, "%s::%s", buf2, litbuf);
				strcpy(litbuf, buf);
				lp = strlen(litbuf);
			}
		}
	}

	mods[mc] = 0;
	litbuf[lp] = 0;
	p = (DEMARG*)gs(sizeof(DEMARG));
	p->mods = mc ? copy(mods) : NULL;
	p->lit = lp ? copy(litbuf) : NULL;
	if (arrp > 0) {
		p->arr = (long*)gs(sizeof(long) * arrp);
		for (i = 0; i < arrp; i++)
			p->arr[i] = arrdim[i];
	}
	else {
		p->arr = NULL;
	}
	p->base = type;
	p->func = farg;
	p->ret = fret;
	p->clname = clp;
	if (clc > 0) {
		p->mname = (DEMCL**)gs(sizeof(DEMCL*) * (clc + 1));
		for (i = 0; i < clc; i++)
			p->mname[i] = clist[i];
		p->mname[clc] = NULL;
	}
	else {
		p->mname = NULL;
	}
	p->next = NULL;

	return p;
}

/* get list of arguments */
static DEMARG* getarglist()
{
	DEMARG* p;
	DEMARG* head;
	DEMARG* curr;
	DEMARG* arg_cache[MAXARG];
	int acmax;
	int ncount;

	head = NULL;
	curr = NULL;

	acmax = -1;
	ncount = 0;

	for (;;) {

		/* get the argument */

		p = getarg(acmax, arg_cache, &ncount);
		if (p == NULL) {
			if (waserror)
				return NULL;
			return head;
		}

		/* cache it for Tn and Nnn */

		arg_cache[++acmax] = p;
		if (curr == NULL) {
			head = p;
			curr = head;
		}
		else {
			curr->next = p;
			curr = p;
		}
	}
}

/* entry point for demangling */
int dem(s, p, buf)
char* s;
DEM* p;
char* buf;
{
	char nbuf[MAXLINE];
	int nc;
	long n;
	char* t;
	char* t2;
	char* t3;
	char* ob;
	int flag;
	int cuflag;
	char buf2[MAXLINE];
	enum DEM_TYPE dt;

	if (s == NULL || p == NULL || buf == NULL)
		return -1;

	cuflag = 0;
#ifdef CLIP_UNDER
	if (*s == '_')
		s++, cuflag = 1;
#endif
	if (!*s)
		return -1;

	/* set up space and input buffer management */

	spbase = buf;
	sp = -1;
	waserror = 0;

	p->fargs = NULL;
	p->cl = NULL;
	p->sc = 0;
	p->args = NULL;
	p->f = NULL;
	p->vtname = NULL;
	p->slev = -1;
	p->type = DEM_NONE;

	/* special case local variables */

	if (cuflag)
		s--;
	if (s[0] == '_' && s[1] == '_' && isdigit(s[2])) {
		t = s + 2;
		n = 0;
		while (isdigit(*t)) {
			n = n * 10 + *t - '0';
			t++;
		}
		if (*t) {
			p->f = copy(t);
			p->slev = n;
			goto done2;
		}
	}
	if (cuflag)
		s++;

	/* special case sti/sti/ptbl */

	if (s[0] == '_' && s[1] == '_' &&
	    (!strncmp(s, "__sti__", 7) || !strncmp(s, "__std__", 7) ||
	    !strncmp(s, "__ptbl_vec__", 12))) {
		p->sc = s[4];
		t = (s[2] == 's' ? s + 7 : s + 12);
		while (*t == '_')
			t++;
		t2 = t;
		while (t2[0] && (t2[0] != '_' || t2[1] != 'c' || t2[2] != '_'))
			t2++;
		*t2 = 0;
		p->f = copy(t);
		cc = 0;
		goto done2;
	}

	/* special case type names */

	if (cuflag)
		s--;
	t = s;
	flag = 0;
	while (t[0] && (t[0] != '_' || t == s || t[-1] != '_'))
		t++;
	if (t[0] == '_' && t[1] == 'p' && t[2] == 't' &&
	    t[3] == '_' && t[4] == '_')
		flag = 1;
	if (t[0] == '_' && t[1] == '_' && t[2] == 'p' && t[3] == 't' &&
	    t[4] == '_' && t[5] == '_')
		flag = 1;
	if (!flag) {
		t = s;
		if ((t[0] == '_' && t[1] == '_' && t[2] == 'Q' &&
		    isdigit(t[3]) && t[4] == '_'))
			flag = 2;
	}
	if (flag) {
		sp = -1;
		waserror = 0;
		if (flag == 1) {
			sprintf(buf2, "%d%s", strlen(s), s);
			push(buf2, 9999);
		}
		else {
			push(s + 2, 9999);
		}
		if ((p->cl = getclass()) == NULL)
			return -1;
		cc = 0;
		goto done2;
	}
	if (cuflag)
		s++;

	sp = -1;
	push(s, 9999);
	waserror = 0;

	/* get function name */

	nc = 0;
	nbuf[0] = 0;
	while (isalnum(cc) || cc == '_') {
		nbuf[nc++] = cc;
		nbuf[nc] = 0;
		if (!base[0] ||
		    (base[0] == '_' && base[1] == '_' && base[2] != '_')) {
			gc();
			break;
		}
		gc();

		/* conversion operators */

		if (!STRCMP(nbuf, "__op")) {
			ob = base - 1;
			if ((p->fargs = getarg(-1, (DEMARG**)0, (int*)0)) == NULL)
				return -1;
			while (ob < base - 1)
				nbuf[nc++] = *ob++;
			nbuf[nc] = 0;
			break;
		}
	}
	if (!isalpha(nbuf[0]) && nbuf[0] != '_')
		return -1;

	/* pick off delimiter */

	if (cc == '_' && base[0] == '_') {
		gc();
		gc();
		if (!cc)
			return -1;
	}

	/* get class name */

	if (isdigit(cc) || cc == 'Q') {
		if ((p->cl = getclass()) == NULL)
			return -1;
	}

	/* a function template */

	else if (cc == 'p' && !strncmp(base, "t__F", 4)) {
		gc();
		gc();
		gc();
		gc();
		gc();
		if (!isdigit(cc))
			return -1;
		n = cc - '0';
		gc();
		if (isdigit(cc)) {
			n = n * 10 + cc - '0';
			gc();
		}
		if (isdigit(cc)) {
			n = n * 10 + cc - '0';
			gc();
		}
		if (n < 1)
			return -1;
		while (n-- > 0) {
			if (!isalnum(cc) && cc != '_')
				return -1;
			gc();
		}
		if (cc != '_' || base[0] != '_')
			return -1;
		gc();
		gc();
	}

	if (!STRCMP(nbuf, "__vtbl")) {
		if (cc == '_' && base[0] == '_' && base[1])
			p->vtname = copy(base + 1);
		goto done;
	}

	/* const/static member functions */

	if ((cc == 'C' || cc == 'S') && base[0] == 'F') {
		p->sc = cc;
		gc();
	}

	/* get arg list for function */

	if (cc == 'F') {
		gc();
		if ((p->args = getarglist()) == NULL)
			return -1;
	}

done:
	if ((cc && STRCMP(nbuf, "__vtbl")) || waserror)
		return -1;

	p->f = copy(nbuf);

done2:

	/* figure out type we got */

	dt = DEM_NONE;
	if (p->sc) {
		switch (p->sc) {
			case 'i':
				dt = DEM_STI;
				break;
			case 'd':
				dt = DEM_STD;
				break;
			case 'b':
				dt = DEM_PTBL;
				break;
			case 'C':
				dt = DEM_CMFUNC;
				break;
			case 'S':
				dt = DEM_SMFUNC;
				break;
			default:
				fatal("bad type set for p->sc", (char*)0, (char*)0);
				break;
		}
	}
	else if (p->slev != -1) {
		dt = DEM_LOCAL;
	}
	else if (p->args != NULL) {
		if (p->fargs != NULL) {
			dt = DEM_OMFUNC;
		}
		else if (p->cl != NULL) {
			t3 = p->f;
			if (t3[0] == '_' && t3[1] == '_') {
				if (t3[2] == 'c' && t3[3] == 't' && !t3[4])
					dt = DEM_CTOR;
				else if (t3[2] == 'd' && t3[3] == 't' &&
				    !t3[4])
					dt = DEM_DTOR;
				else
					dt = DEM_MFUNC;
			}
			else {
				dt = DEM_MFUNC;
			}
		}
		else {
			dt = DEM_FUNC;
		}
	}
	else if (p->f == NULL && p->cl != NULL) {
		if (p->cl->clargs != NULL)
			dt = DEM_TTYPE;
		else
			dt = DEM_CTYPE;
	}
	else if (p->f != NULL) {
		if (p->cl != NULL) {
			t3 = p->f;
			if (t3[0] == '_' && t3[1] == '_' && t3[2] == 'v' &&
			    t3[3] == 't' && t3[4] == 'b' && t3[5] == 'l' &&
			    !t3[6])
				dt = DEM_VTBL;
			else
				dt = DEM_MDATA;
		}
		else {
			dt = DEM_DATA;
		}
	}

	if (dt == DEM_NONE)
		fatal("cannot characterize type of input", (char*)0, (char*)0);

	p->type = dt;

	return 0;
}

/************************* PRINT AN UNMANGLED NAME *************************/

/* format a class name */
void dem_printcl(p, buf)
DEMCL* p;
char* buf;
{
	int i;
	char buf2[MAXLINE];
	void dem_printarglist();

	if (p == NULL || buf == NULL)
		fatal("bad argument to dem_printcl()", (char*)0, (char*)0);

	buf[0] = 0;
	i = 0;
	while (p != NULL) {
		i++;

		/* handle nested */

		if (i > 1)
			strcat(buf, "::");
		strcat(buf, p->name);

		/* template class */

		if (p->clargs != NULL) {
			if (buf[strlen(buf) - 1] == '<')
				strcat(buf, " ");
			strcat(buf, "<");
			dem_printarglist(p->clargs, buf2, 0);
			strcat(buf, buf2);
			if (buf[strlen(buf) - 1] == '>')
				strcat(buf, " ");
			strcat(buf, ">");
		}
		p = p->next;
	}
}

/* format an argument list */
void dem_printarglist(p, buf, sv)
DEMARG* p;
char* buf;
int sv;
{
	int i;
	char buf2[MAXLINE];
	void dem_printarg();

	if (p == NULL || buf == NULL || sv < 0 || sv > 1)
		fatal("bad argument to dem_printarglist()", (char*)0, (char*)0);

	/* special case single "..." argument */

	if (p->base == 'v' && p->mods == NULL && p->next != NULL &&
	    p->next->base == 'e' && p->next->next == NULL) {
		strcpy(buf, "...");
		return;
	}

	/* special case single "void" argument */

	if (p->base == 'v' && p->mods == NULL) {
		strcpy(buf, "void");
		return;
	}

	buf[0] = 0;
	i = 0;
	while (p != NULL) {
		i++;
		if (i > 1)
			strcat(buf, p->base == 'e' ? " " : ",");
		dem_printarg(p, buf2, sv);
		strcat(buf, buf2);
		p = p->next;
	}
}

/* format a single argument */
void dem_printarg(p, buf, f)
DEMARG* p;
char* buf;
int f;
{
	char* t;
	char bufc[MAXLINE];
	char bufc2[MAXLINE];
	char farg[MAXLINE];
	char fret[MAXLINE];
	char* m;
	char* mm;
	char pref[MAXLINE];
	int arrindx;
	long dim;
	char scr[MAXLINE];
	char ptrs[MAXLINE];
	int i;
	int sv;
	char* s;
	char* trail;
	int clc;

	if (p == NULL || buf == NULL || f < 0 || f > 1)
		fatal("bad argument to dem_printarg()", (char*)0, (char*)0);

	/* format the underlying type */

	sv = !f;

	switch (p->base) {

		/* fundamental types */

		case 'v':
			t = "void";
			break;
		case 'c':
			t = "char";
			break;
		case 's':
			t = "short";
			break;
		case 'i':
			t = "int";
			break;
		case 'l':
			t = "long";
			break;
		case 'V':
			t = "long long";
			break;
		case 'f':
			t = "float";
			break;
		case 'd':
			t = "double";
			break;
		case 'r':
			t = "long double";
			break;
		case 'e':
			t = "...";
			sv = 1;
			break;

		/* functions */

		case 'F':
			dem_printarg(p->ret, fret, 0);
			dem_printarglist(p->func, farg, 0);
			break;

		/* class, struct, union, enum */

		case 'C':
			dem_printcl(p->clname, bufc);
			t = bufc;
			break;

		default:
			fatal("bad base type in dem_printarg()", (char*)0, (char*)0);
			break;
	}

	/* handle modifiers and declarators */

	pref[0] = 0;
	m = p->mods;
	if (m == NULL)
		m = "";

	/* const and unsigned */

	mm = m;
	while (*mm) {
		if (mm[0] == 'C' && (mm[1] != 'P' && mm[1] != 'R' && mm[1] != 'M') && (mm[1] || p->base != 'F')) {
			strcat(pref, "const ");
			break;
		}
		mm++;
	}
	mm = m;
	while (*mm) {
		if (*mm == 'U') {
			strcat(pref, "unsigned ");
			break;
		}
		mm++;
	}

	/* go through modifier list */

	mm = m;
	ptrs[0] = 0;
	arrindx = 0;
	clc = 0;
	while (*mm) {
		if (mm[0] == 'P') {
			sprintf(scr, "*%s", ptrs);
			strcpy(ptrs, scr);
		}
		else if (mm[0] == 'R') {
			sprintf(scr, "&%s", ptrs);
			strcpy(ptrs, scr);
		}
		else if (mm[0] == 'M') {
			dem_printcl(p->mname[clc++], bufc2);
			sprintf(scr, "%s::*%s", bufc2, ptrs);
			strcpy(ptrs, scr);
		}
		else if (mm[0] == 'C' && mm[1] == 'P') {
			sprintf(scr, " *const%s%s", isalnum(ptrs[0]) || ptrs[0] == '_' ? " " : "", ptrs);
			strcpy(ptrs, scr);
			mm++;
		}
		else if (mm[0] == 'C' && mm[1] == 'R') {
			sprintf(scr, " &const%s%s", isalnum(ptrs[0]) || ptrs[0] == '_' ? " " : "", ptrs);
			strcpy(ptrs, scr);
			mm++;
		}
		else if (mm[0] == 'C' && mm[1] == 'M') {
			dem_printcl(p->mname[clc++], bufc2);
			sprintf(scr, "%s::*const%s%s", bufc2, isalnum(ptrs[0]) || ptrs[0] == '_' ? " " : "", ptrs);
			strcpy(ptrs, scr);
			mm++;
		}
		else if (mm[0] == 'A') {
			dim = p->arr[arrindx++];
			s = sv ? "" : "@";
			if (!ptrs[0]) {
				sprintf(scr, "%s[%ld]", s, dim);
				sv = 1;
			}
			else if (ptrs[0] == '(' || ptrs[0] == '[') {
				sprintf(scr, "%s[%ld]", ptrs, dim);
			}
			else {
				sprintf(scr, "(%s%s)[%ld]", ptrs, s, dim);
				sv = 1;
			}
			strcpy(ptrs, scr);
		}
		else if (mm[0] == 'U' || mm[0] == 'C' || mm[0] == 'S') {
			/* ignore */
		}
		else {
			fatal("bad value in modifier list", (char*)0, (char*)0);
		}
		mm++;
	}

	/* put it together */

	s = sv ? "" : "@";
	if (p->base == 'F') {
		i = 0;
		if (ptrs[0] == ' ')
			i = 1;
		trail = "";
		if (p->mods != NULL && p->mods[strlen(p->mods) - 1] == 'C')
			trail = " const";
		if (ptrs[i])
			sprintf(buf, "%s%s (%s%s)(%s)%s", pref, fret, ptrs + i,
			    s, farg, trail);
		else
			sprintf(buf, "%s%s %s(%s)%s", pref, fret, s, farg, trail);
	}
	else {
		sprintf(buf, "%s%s%s%s%s", pref, t, ptrs[0] == '(' || isalnum(ptrs[0]) || ptrs[0] == '_' ? " " : "", ptrs, s);
	}
	if (p->lit != NULL) {
		if (isdigit(p->lit[0]) || p->lit[0] == '-')
			sprintf(scr, "(%s)%s", buf, p->lit);
		else
			sprintf(scr, "&%s", p->lit);
		strcpy(buf, scr);
	}
}

struct Ops {
	char* encode;
	char* name;
};

static struct Ops ops[] = {
	"__pp",		"operator++",
	"__as",		"operator=",
	"__vc",		"operator[]",
	"__nw",		"operator new",
	"__dl",		"operator delete",
	"__rf",		"operator->",
	"__ml",		"operator*",
	"__mm",		"operator--",
	"__oo",		"operator||",
	"__md",		"operator%",
	"__mi",		"operator-",
	"__rs",		"operator>>",
	"__ne",		"operator!=",
	"__gt",		"operator>",
	"__ge",		"operator>=",
	"__or",		"operator|",
	"__aa",		"operator&&",
	"__nt",		"operator!",
	"__apl",	"operator+=",
	"__amu",	"operator*=",
	"__amd",	"operator%=",
	"__ars",	"operator>>=",
	"__aor",	"operator|=",
	"__cm",		"operator,",
	"__dv",		"operator/",
	"__pl",		"operator+",
	"__ls",		"operator<<",
	"__eq",		"operator==",
	"__lt",		"operator<",
	"__le",		"operator<=",
	"__ad",		"operator&",
	"__er",		"operator^",
	"__co",		"operator~",
	"__ami",	"operator-=",
	"__adv",	"operator/=",
	"__als",	"operator<<=",
	"__aad",	"operator&=",
	"__aer",	"operator^=",
	"__rm",		"operator->*",
	"__cl",		"operator()",
	NULL,		NULL
};

/* format a function name */
void dem_printfunc(dp, buf)
DEM* dp;
char* buf;
{
	int i;
	char buf2[MAXLINE];

	if (dp == NULL || buf == NULL)
		fatal("bad argument to dem_printfunc()", (char*)0, (char*)0);

	if (dp->f[0] == '_' && dp->f[1] == '_') {

		/* conversion operators */

		if (!strncmp(dp->f, "__op", 4) && dp->fargs != NULL) {
			dem_printarg(dp->fargs, buf2, 0);
			sprintf(buf, "operator %s", buf2);		
		}

		/* might be overloaded operator */

		else {
			i = 0;
			while (ops[i].encode != NULL && strcmp(ops[i].encode, dp->f))
				i++;
			if (ops[i].encode != NULL)
				strcpy(buf, ops[i].name);
			else
				strcpy(buf, dp->f);
		}
	}
	else {
		strcpy(buf, dp->f);
	}
}

/* entry point to formatting functions */
int dem_print(p, buf)
DEM* p;
char* buf;
{
	char buf2[MAXLINE];
	char* s;
	int t;

	if (p == NULL || buf == NULL)
		return -1;

	buf[0] = 0;

	/* type names */

	if (p->f == NULL && p->cl != NULL) {
		dem_printcl(p->cl, buf);
		return 0;
	}

	/* sti/std */

	if (p->sc == 'i' || p->sc == 'd') {
		sprintf(buf, "%s:__st%c", p->f, p->sc);
		return 0;
	}
	if (p->sc == 'b') {
		sprintf(buf, "%s:__ptbl_vec", p->f);
		return 0;
	}

	/* format class name */

	buf2[0] = 0;
	if (p->cl != NULL) {
		dem_printcl(p->cl, buf2);
		strcat(buf, buf2);
		strcat(buf, "::");
	}

	/* special case constructors and destructors */

	s = buf2 + strlen(buf2) - 1;
	t = 0;
	while (s >= buf2) {
		if (*s == '>')
			t++;
		else if (*s == '<')
			t--;
		else if (*s == ':' && !t)
			break;
		s--;
	}
	if (!STRCMP(p->f, "__ct")) {
		strcat(buf, s + 1);
	}
	else if (!STRCMP(p->f, "__dt")) {
		strcat(buf, "~");
		strcat(buf, s + 1);
	}
	else {
		dem_printfunc(p, buf2);
		strcat(buf, buf2);
	}

	/* format argument list */

	if (p->args != NULL) {
		strcat(buf, "(");
		dem_printarglist(p->args, buf2, 0);
		strcat(buf, buf2);
		strcat(buf, ")");
	}

	/* const member functions */

	if (p->sc == 'C')
		strcat(buf, " const");

	return 0;
}

/* explain a type */
char* dem_explain(t)
enum DEM_TYPE t;
{
	switch (t) {
		case DEM_STI:
			return "static construction function";
		case DEM_STD:
			return "static destruction function";
		case DEM_VTBL:
			return "virtual table";
		case DEM_PTBL:
			return "ptbl vector pointing to vtbls";
		case DEM_FUNC:
			return "function";
		case DEM_MFUNC:
			return "member function";
		case DEM_SMFUNC:
			return "static member function";
		case DEM_CMFUNC:
			return "constant member function";
		case DEM_OMFUNC:
			return "conversion operator member function";
		case DEM_CTOR:
			return "constructor";
		case DEM_DTOR:
			return "destructor";
		case DEM_DATA:
			return "data";
		case DEM_MDATA:
			return "member data";
		case DEM_LOCAL:
			return "local variable";
		case DEM_CTYPE:
			return "class type";
		case DEM_TTYPE:
			return "template type";
		default:
			fatal("bad type passed to dem_explain()", (char*)0, (char*)0);
			return "";
	}
}

/* demangle in --> out */
int demangle(in, out)
char* in;
char* out;
{
	char sbuf[MAXDBUF];
	DEM d;

	if (in == NULL || !*in || out == NULL)
		return -1;

	if (dem(in, &d, sbuf) < 0) {
		strcpy(out, in);
		return -1;
	}

	dem_print(&d, out);

#ifdef EXPLAIN
	strcat(out, " [");
	strcat(out, dem_explain(d.type));
	strcat(out, "]");
#endif
	return 0;
}

#ifdef DEM_MAIN
/* process one file */
static void dofile(fp, del)
FILE* fp;
int del;
{
	char buf[MAXLINE];
	char* s;
	char* start;
	char buf2[MAXLINE];
	char c;

	while (fgets(buf, MAXLINE, fp) != NULL) {
		s = buf;
		for (;;) {

			/* skip whitespace */

			if (del == SKIP_WS) {
				while (*s && *s <= ' ') {
					putchar(*s);
					s++;
				}
			}
			else {
				while (*s && !isalnum(*s) && *s != '_') {
					putchar(*s);
					s++;
				}
			}
			if (!*s)
				break;
			start = s;

			/* unmangle one name in place */

			if (del == SKIP_WS) {
				while (*s && *s > ' ')
					s++;
			}
			else {
				while (*s && (isalnum(*s) || *s == '_'))
					s++;
			}
			c = *s;
			*s = 0;
			demangle(start, buf2);
			printf("%s", buf2);
			*s = c;
		}
	}
}

main(argc, argv)
int argc;
char** argv;
{
	int i;
	int nerr;
	FILE* fp;
	int del;

	del = SKIP_ALL;
	nerr = 0;

	while (argc >= 2 && argv[1][0] == '-') {
		switch (argv[1][1]) {
			case 'w':
				del = SKIP_WS;
				break;
		}
		argc--;
		argv++;
	}

	/* standard input */

	if (argc < 2) {
		dofile(stdin, del);
	}

	/* else iterate over all files */

	else {
		for (i = 1; i < argc; i++) {
			if ((fp = fopen(argv[i], "r")) == NULL) {
				fprintf(stderr, "cannot open %s for reading\n",
				    argv[i]);
				nerr++;
				continue;
			}
			dofile(fp, del);
			fclose(fp);
		}
	}

	exit(nerr);
}
#endif
