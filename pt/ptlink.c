/*ident	"@(#)cls4:tools/pt/ptlink.c	1.33" */
/*******************************************************************************
C++ source for the C++ Language System, Release 3.0.  This product
is a new release of the original cfront developed in the computer
science research center of AT&T Bell Laboratories.

Copyright (c) 1991, 1992 AT&T and UNIX System Laboratories, Inc.
Copyright (c) 1984, 1989, 1990 AT&T.  All Rights Reserved.

THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE of AT&T and UNIX System
Laboratories, Inc.  The copyright notice above does not evidence
any actual or intended publication of such source code.

*******************************************************************************/
/*
 *  ptlink - parameterized type instantiation program
 *
 *  Usage:  ptlink ptrepository /usr/tmp/CC.12345 /usr/bin/CC
 *		/usr/tmp/CC.12345/ptincl /usr/tmp/CC.12345/ptin
 *		/usr/tmp/CC.12345/ptout defmap /usr/tmp/CC.12345.ptccopt
 *		/usr/tmp/CC.12345/ptrlistx /lib/cpp
 *
 *  where ptrepository is the repository directory,
 *  /usr/tmp/CC.12345 is a temporary directory,
 *  /usr/bin/CC is the name of the CC command,
 *  /usr/tmp/CC.12345/ptincl is a file containing all of the -I options,
 *  /usr/tmp/CC.12345/ptin is a file that contains all of the .o and .a files
 *    that were found on the original link line,
 *  /usr/tmp/CC.12345/ptout is a file that will contain a list of all of the
 *    extra .o files in the repository directory that must be linked in,
 *  defmap is the name
 *  /usr/tmp/CC.12345/ptccopt is a file that contains all of the extra CC
 *    options that were on the original CC line,
 *  /usr/tmp/CC.12345/ptrlistx is a file that contains all of the auxiliary
 *    repository directories, and
 *  /lib/cpp is the name of the C preprocessor.
 *
 *  You can also use the following options:
 *     -ptH".h, .H, .hxx, .HXX, .hh, .HH, .hpp" -- define the suffixes that
 *	  ptlink will consider as a header file
 *     -ptS".c, .C, .cxx, .CXX, .cc, .CC, .cpp" -- define the suffixes that
 *	  ptlink will consider as a source file
 *     -ptv -- be verbose (print lots of information about the various
 *	  steps of instantiation)
 *     -pti -- compile the generated C files with the +i option, leaving the
 *	  ..c files in the repository
 *     -pta -- change the instantiation rule to instantiate an entire template
 *	  class at once rather that just the needed functions from a template
 *	  class
 *     -ptk -- quit early if there are instantiation errors
 *
 *
 *  Functions:
 *	void pushstring(s) - push "char*" onto local stack "sstack"
 *	void popstring() - pop local stack "sstack"
 *	void handlesig() - print error message on SIGINT or SIGQUIT
 *
 *	Fname* flookup(s, ftab) - look up name in hash table of file names
 *	void fadd(s, ftab) - add a new file name to hash table
 *	void ftab_del(ftab) - delete all entries in file name table
 *
 *	int nm_split(buf, flds) - split the nm output line into fields
 *	int is_archive(f) - returns 1 if the file named by f is an archive file
 *	Obj* readnm(f) - read and process the nm output for a single object
 *	   file
 *	void obj_del2(r) - delete a name list
 *	void addfile(objp, f, flag) - add a file to the known objects
 *	void obj_del() - delete a list of object files
 *	
 *	Symbol* lookup(s) - look up a Symbol
 *	void add(s, t, f, rep, on) - add a Symbol to the symbol table
 *	void symtab_del() - delete all entries in the symbol table
 *	
 *	int okobj(nl) - check if there are any conflicts if a ".o" file is
 *	   linked in -- okobj() is always called before addobj()
 *	void addobj(nl, f, rep, on, fp) - add an object
 *	void fakelink(fp) - fake a link on the FILE pointed to by fp
 *		-- if the file is a .o file, all of the symbols are
 *		   added to the symbol table
 *		-- if the file is a .a file (library archive), only the
 *		   symbols from the needed .o files are added to the
 *		   symbol table
 *
 *	int ispt(s, f) - uses dem() to determine if the name pointed to by
 *	   s is a template function or a template class member function
 *
 *	char* getname(s, pbuf, dbuf) - gets the base part of a template class
 *	void makefnam(ptr, s, bbuf, hflag) - makes a filename from a template
 *	   class name (possibly using a hash function on systems with 14-letter
 *	   limitations on filenames)
 *	
 *	Tfile** tf_get() - create a file table (list of Tfile objects)
 *	void tf_del(tftab) - clean up an entire file table
 *	void tf_read(f, tftab) - reads a map file and stores information
 *	   in a set of Tfile structures
 *
 *	void getcl(clp, slp) - get the type names used in a parameter list
 *	   (used for building instantiation files)
 *	void getfcl(p, slp) - get the type names used in a function template
	   parameter list
 *	void find_incl(incl, f, ext, quot, extout) - find a file based on a -I
 *	   list
 *	void addincl(t, slp, tfp, ext, incl) - add to a -I list
 *
 *	int valid_bn(r) - make sure that a basename is found by flookup() in
 *	   some file table
 *	void read_mname(dir, list, defmap) - read and sort a list of mapfile
 *	   names from a directory
 *	int tlookup_help(t, r, pdp, pip, minfile, maxfile, bnflag) - helper
 *	   function for tlookup
 *	int tlookup(t, tlp, fflag, tftab, incl) - look up a type in a set of
 *	   map files
 *	int tlookup2(name, tlist, t, tfp, dlp, defmap, incl) - look up a type
 *	   in a map file
 *
 *	void getund(slp, tfp, dlp, defmap, rawn) - get a list of undefined
 *	   template symbols (this function is run after fakelink())
 *	void doincl(fp, name, tfp, dlp, defmap, incl, tlist) - write the
 *	   includes into an instantiation file
 *
 *	void do_lit(fp, clp) - literals for an instantiation
 *	void doforce(fp, raw, fmt) - force an instantiation (finishes the
 *	   writing of an instantiation file, putting in the dummy typedef
 *	   or function call)
 *	void fcomp(ccpath, incl, ccopts, type, buf, dip) - build the CC
 *	   command for an instantiation
 *	unsigned long docs(ptr, bbuf, cpp, incl, ccopts) - do the checksum
 *	int check_time(r, bbuf, incl, tlist, wbuf2, main_rep) - check timestamps
 *	int dodep(ptr, bbuf, ccpath, incl, ccopts, obuf, dlp, dip, cs,
 *	   tflag, wbuf, cpp, tlist, ptoflag) - check dependencies
 *	void build_cache(cfn, incl, cpp, id, ptr, bbuf) - rebuild cache
 *	int cmp_in(p1, p2) - compare two Inode_cmp entries
 *	int comp_hdr(old, new) - compare two lists of headers from the header
 *	   cache
 *	void comp_id(old, new, ich, dch) - compare old and new -I/-D flags
 *	void read_cache(cfn, slp) - read headers from a cache
 *	int docomp(ptr, bbuf, ccpath, incl, ccopts, dip, prevcs, cpp) - compile
 *	   an instantiation file
 *	Slist* di_lookup(s) - look up a list
 *	void di_del() - delete the lists
 *	void get_defdata(rn, out, obuf) - find all of the symbols in the
 *	   symbol table that are associated with a given class
 *	void add_lnobj(olp, obuf, comp) - add to the list of object files from
 *	   the repository directory that are to be added to the link
 *	void parse_ext(t, p, list) - parse extension list
 *
 * The code in this file makes extensive use of the Slist datatype
 * that is declared in the file pt.h, with functions defined in
 * ptutil.c.  Slist is a simple "list of strings" abstraction with
 * the following functions:
 *	void slp_add(slp, name) - add a string at the end of the list
 *	int slp_adduniq(slp, name) - add a string at the end of the list if
 *	   it isn't in the list already
 *	void slp_del(slp) - delete the entire list
 *	void slp_sort(slp) - sort the list
 *	int slp_eq(p1, p2, p3) - see if any members of the list p1 are found
 *	   in the list p2 -- add any matches you find to the list p3
 *	int slp_subset(p1, p2, p3) - see if the list p1 is a subset of list p2,
 *	   add to list p3 any members of list p1 that are not found in list p2
 */
#include <stdio.h>
#include <ctype.h>
#include "dem.h"
#include "pt.h"
#define ATTLC

char* tdir;	/* the directory where "instfile" will be written */
int verbose = 0;	/* this flag is set if the -ptv option is used */
long start_time;
char* PROGNAME = "CC[ptlink]";
static int plusi = 0;	/* this flag is set if the -pti option is used */
#define MAXEXT 25 
static char* hdr_ext[MAXEXT];
static char* src_ext[MAXEXT];

/******************** STRINGS LISTS FOR ERROR CLEANUP ********************/

static char* sstack[25];
static int sptr = -1;
void pushstring(s)
char* s;
{
	if (s == NULL || !*s)
		fatal("bad argument to pushstring()", (char*)0, (char*)0);
	sstack[++sptr] = copy(s);
}
void popstring()
{
	if (sptr < 0)
		fatal("popstring() called without pushstring()", (char*)0, (char*)0);
	del_file(sstack[sptr]);
	fs((char*)sstack[sptr--]);
}
void handlesig()
{
	static int entry_count = 0;

	if (entry_count++)
		return;
	while (sptr >= 0)
		popstring();
	fatal("interrupted by INT or QUIT signal", (char*)0, (char*)0);
}

/************************* TABLE OF FILENAMES SEEN *************************/

typedef struct Fname Fname;
struct Fname {
	char* name;
	Fname* next;
};
static Fname* ftable[FTSIZE];
static Fname* otable[FTSIZE];
#if defined(hpux)
static Fname* stable[FTSIZE];
#endif

/* look up a filename */
Fname* flookup(s, ftab)
char* s;
Fname* ftab[];
{
	Fname* p;

	if (s == NULL || !*s || ftab == NULL)
		fatal("bad argument to flookup()", (char*)0, (char*)0);

	p = ftab[(int)hash(s, (long)FTSIZE)];
	while (p != NULL) {
		if (!strcmp(s, p->name))
			return p;
		p = p->next;
	}

	return NULL;
}

/* add a filename */
void fadd(s, ftab)
char* s;
Fname* ftab[];
{
	Fname* p;
	int h;

	if (s == NULL || !*s || ftab == NULL)
		fatal("bad argument to fadd()", (char*)0, (char*)0);

	p = ftab[h = (int)hash(s, (long)FTSIZE)];
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

/* delete the table of file names */
void ftab_del(ftab)
Fname* ftab[];
{
	int i;
	Fname* p;
	Fname* r;

	if (ftab == NULL)
		fatal("bad argument to ftab_del()", (char*)0, (char*)0);

	for (i = 0; i < FTSIZE; i++) {
		p = ftab[i];
		while (p != NULL) {
			fs((char*)p->name);
			r = p;
			p = p->next;
			fs((char*)r);
		}
		ftab[i] = NULL;
	}
}

/************************* NM / SYMBOL TABLE *************************/

typedef struct Ename Ename;
typedef struct Obj Obj;
typedef struct Flist Flist;

struct Ename {
	char* name;
	Ename* next;
	char type;
};

struct Obj {
	Ename* nlist;
	Obj* next;
	char* file;
	char pi;
	char aflag;
};

struct Flist {
	Obj* obj;
	char* file;
	Flist* next;
	char rep;
};

static Flist* fhead = NULL;
static Flist* fcurr = NULL;

/* split the output of nm into fields */
/* The nm_split(buf, flds) function parses the buf argument (which is
   assumed to be a line from the output of the "nm" command) and it modifies
   the pointers in the array flds.  The array flds should be a char* array
   of at least two elements.  flds[0] will point to a one-letter code
   which indicates if the symbol is defined or undefined and whether it
   is text, data, or common block.  flds[1] will point to the symbol name.
  
   The nm_split() function understands both System V and BSD nm formats.
   (Most System V nm commands have a "Berkeley" mode (either -B or -p)
   which is easier to parse and doesn't truncate very long symbols.)
   There is a kludge in this function to skip over the initial "."
   in a symbol name that is found in the IBM RS/6000 nm format.
  
   The return values of nm_split() are as follows:
     0 - this line defines a non-global symbol (usually ignored by the
         function calling nm_split())
     1 - this line defines a file (flds[0] is set to the file name)
         If nm_split() is used on the nm output from an archive file, it
         will find more that one file name within that output.
     2 - this line defined a global symbol (flds[0] is set to the one-letter
         typename for the symbol (U,D,T, or C), flds[1] is set to the symbol
         name)
  
   If nm_split() determines that the "nm" output is from a System V
   nm command (heuristic: it's System V nm output if the line contains
   at least one  "|" character), then there will be some translation
   required to get the one-letter codes for the type of each symbol.
  
   SVR4:
      We are only concerned with global symbols (field 4 == "GLOB").
      If field 6 == "UNDEF", then the one-letter code is "U".
      If field 6 == "COMMON", then the one-letter code is "C".
      Otherwise, we look at field 3 -- if it's "FUNC", the one-letter
      code is "T", otherwise it is "D".
  
   Other System V:
      We are only concerned with global symbols (field 2 == "extern").
      If field 6 == ".text", then the one-letter code is "T".
      If field 6 == ".data", then the one-letter code is "D".
      Otherwise, we look at the address field (field 0).  If it's "0",
      the one-letter code is "C", otherwise it's "U".
  
   BSD format:
      We are not interested in the address field, only the type and
      symbol fields.  We are only concerned with global symbols
      (one-letter code is a capital letter).  There is a mapping of
      some of the one-letter codes to simplify the rest of the software:
  
  		B, G, S --> D
  		E, c --> C
  		V --> U
  
      Other mappings may be required for some operating systems that
      have other single-letter types for external symbols in the nm
      output -- this is one place where ptlink might have to be
      modified in the future.
 */
int nm_split(buf, flds)
char* buf;
char* flds[];
{
	char c;
	int nf;
	char* s;
	int i;
	char* start;
	int flag;
	char* f[100];
	char* t;

	if (buf == NULL || flds == NULL)
		fatal("bad argument to nm_split()", (char*)0, (char*)0);

	nf = 0;
	buf[strlen(buf) - 1] = 0;

	/* verbose format from nm (SysV.3) */

	s = buf;
	if (s[0] == 'S' && !strncmp(s, "Symbols from ", 13)) {

		/* filename - might have to separate out object file name */

		t = s + 13;
		while (*t && *t != '[')
			t++;
		if (!*t) {
			if (t[-1] == ':')
				t[-1] = 0;
			flds[0] = s + 13;
			return 1;
		}
		else {
			s = ++t;
			while (*t && *t != ']')
				t++;
			if (*t)
				*t = 0;
			flds[0] = s;
			return 1;
		}
	}
	flag = 0;
	while (*s) {
		if (*s == '|') {
			flag = 1;
			break;
		}
		s++;
	}
	s = buf;
	if (flag) {

		/* grab separate fields */

#if defined(svr4) || defined(solaris)
		for (i = 0; i <= 7; i++) {
#else
		for (i = 0; i <= 6; i++) {
#endif
			start = s;
			while (*s && *s != '|')
				s++;
			if (*s)
				*s++ = 0;
			else
				*s = 0;
			while (*start && *start <= ' ')
				start++;
#ifdef _IBMR2
			if (i == 0 && *start == '.')
				start++;
#endif
			t = start + strlen(start) - 1;
			while (t >= start && *t <= ' ')
				*t-- = 0;
			f[i] = start;
		}
#if defined(svr4) || defined(solaris)
		if (strcmp(f[4], "GLOB")) {
#if defined(hpux)
			flds[1] = f[7];
#endif
			return 0;
		}
		if (!strcmp(f[6], "UNDEF"))
			flds[0] = "U";
		else if (!strcmp(f[6], "COMMON"))
			flds[0] = "C";
		else if (!strcmp(f[3], "FUNC"))
			flds[0] = "T";
		else
			flds[0] = "D";
		flds[1] = f[7];
		return 2;
#else
		if (strcmp(f[2], "extern")) {
#if defined(hpux)
			flds[1] = f[0];
#endif
			return 0;
		}
		if (!strcmp(f[6], ".text"))
			flds[0] = "T";
		else if (!strcmp(f[6], ".data"))
			flds[0] = "D";
		else if (strcmp(f[1], "0"))
			flds[0] = "C";
		else
			flds[0] = "U";
		flds[1] = f[0];
		return 2;
#endif
	}

	/* terse format */

	else {
		for (i = 0; i <= 99; i++) {
			while (*s && *s <= ' ')
				s++;
			start = s;
			while (*s && *s > ' ')
				s++;
			if (s == start)
				break;
			if (*s)
				*s++ = 0;
			f[nf++] = start;
		}
		if (nf < 1 || nf > 3) {
#if defined(hpux)
			flds[1] = 0;
#endif
			return 0;
		}
		if (nf == 3) {
			c = f[1][0];
			if (c == 'T')
				flds[0] = "T";
			else if (c == 'B' || c == 'D' || c == 'G' || c == 'S')
				flds[0] = "D";
			else if (c == 'C' || c == 'E' || c == 'c')
				flds[0] = "C";
			else if (c == 'U' || c == 'V')
				flds[0] = "U";
			else {
#if defined(hpux)
				flds[1] = f[2];
#endif
				return 0;
			}
			flds[1] = f[2];
			return 2;
		}
		if (nf == 2) {
			c = f[0][0];
			if (c == 'T')
				flds[0] = "T";
			else if (c == 'B' || c == 'D' || c == 'G' || c == 'S')
				flds[0] = "D";
			else if (c == 'C' || c == 'E' || c == 'c')
				flds[0] = "C";
			else if (c == 'U' || c == 'V')
				flds[0] = "U";
			else {
#if defined(hpux)
				flds[1] = f[1];
#endif
				return 0;
			}
			flds[1] = f[1];
			return 2;
		}

		/* grab filename and separate out if necessary */

		t = f[0];
		while (*t && *t != '[')
			t++;
		if (!*t) {
			if (t[-1] == ':')
				t[-1] = 0;
			flds[0] = f[0];
			return 1;
		}
		s = ++t;
		while (*t && *t != ']')
			t++;
		if (*t)
			*t = 0;
		flds[0] = s;
		return 1;
	}
}

/* return 1 if file is an archive library */
int is_archive(f)
char* f;
{
	if (f == NULL || !*f)
		fatal("bad argument to is_archive()", (char*)0, (char*)0);

	if (strlen(f) > strlen(ARC_EXT) &&
	    !strcmp(f + strlen(f) - strlen(ARC_EXT), ARC_EXT))
		return 1;
	else
		return 0;
}

/* read one object file or archive */
/* readnm(f) - run "nm" on a file, build an Obj record for the file
   f is the name of the file (either an object file or a library archive)

   The Obj record returned by this function will contains a linked list
   of all of the symbols (both defined and undefined) in the file.
   These symbols will be added to the symbol table later by fakelink().
 */
Obj* readnm(f)
char* f;
{
	char buf[MAXPATH];
	char* flds[2];
	int nf;
	FILE* fp;
	char c;
	Ename* enp;
	Ename* enplist;
	Obj* olist;
	Obj* olistp;
	Obj* olisth;
	char oldf[MAXPATH];

	if (f == NULL || !*f)
		fatal("bad argument to readnm()", (char*)0, (char*)0);

	/* establish pipe to file */

	sprintf(buf, "%s %s", NMPATH, f);
	if ((fp = fast_popen(buf, "r")) == NULL) {
		fprintf(stderr, "%s warning: ##### nm failed on %s, ignoring ... #####\n", PROGNAME, f);
		return NULL;
	}

	/* split each line into fields */

	enplist = NULL;
	olist = NULL;
	olisth = NULL;
	strcpy(oldf, f);
	while (fgets(buf, MAXPATH, fp) != NULL) {
		nf = nm_split(buf, flds);
		if (nf == 0) {
#if defined(hpux)
			if (flds[1]) {
				fadd(copy(flds[1]), stable);
			}
#endif
			continue;
		}

		/* start of a file in an archive */

		if (nf == 1) {
#if defined(hpux)
			ftab_del(stable);
#endif
			fadd(basename(flds[0], 1), ftable);
			if (enplist != NULL) {
				olistp = (Obj*)gs(sizeof(Obj));
				olistp->file = copy(oldf);
				olistp->nlist = enplist;
				enplist = NULL;
				olistp->next = NULL;
				if (olist != NULL)
					olist->next = olistp;
				else
					olisth = olistp;
				olist = olistp;
			}
			strcpy(oldf, flds[0]);
		}

		/* regular record in a file */

		else {
#if defined(hpux)
			if (flookup(flds[1], stable) != NULL) {
				continue;
			}
#endif
			c = flds[0][0];
			if (c != 'B' && c != 'C' && c != 'D' &&
			    c != 'T' && c != 'U')
				fatal("bad type returned by nm_split()", (char*)0, (char*)0);
			enp = (Ename*)gs(sizeof(Ename));
			enp->name = copy(flds[1]);
			enp->type = c;
			enp->next = enplist;
			enplist = enp;
		}
	}

	/* finish up */

	if (enplist != NULL) {
		olistp = (Obj*)gs(sizeof(Obj));
		olistp->file = copy(oldf);
		olistp->nlist = enplist;
		enplist = NULL;
		olistp->next = NULL;
		if (olist != NULL)
			olist->next = olistp;
		else
			olisth = olistp;
		olist = olistp;
	}
	if (olisth != NULL)
		olisth->aflag = (is_archive(f) || olisth->next != NULL);

	fast_pclose(fp);
#if defined(hpux)
	ftab_del(stable);
#endif

	return olisth;
}

/* delete a name list */
/* obj_del2(r) - clean up a single Obj record
   r is a pointer to an Obj record
 */
void obj_del2(r)
Obj* r;
{
	Obj* r2;
	Ename* s;
	Ename* s2;

	if (r == NULL)
		fatal("bad argument to obj_del2()", (char*)0, (char*)0);

	while (r != NULL) {
		fs((char*)r->file);
		s = r->nlist;
		while (s != NULL) {
			fs((char*)s->name);
			s2 = s;
			s = s->next;
			fs((char*)s2);
		}
		r2 = r;
		r = r->next;
		fs((char*)r2);
	}
}

/* add a file to the set of objects that are known */
/* addfile(objp, f, flag)

   objp is a pointer to filled-in Obj structure (not modified by this function)
   f is the name of the file
   flag is a boolean flag that indicates that this filename might already
      be in the global list of files

   addfile() will modify the list of Flist objects pointed to by the
   global variable fhead.  This function normally adds a new record to the
   list.  It makes the Obj pointer in the new record point to objp.
   If the file is already in the list (and flag is nonzero), addfile() will
   find the Flist record for that file, call obj_del2() to clean up the
   old Obj record, and then assign the Obj pointer in that record to objp.
 */
void addfile(objp, f, flag)
Obj* objp;
char* f;
int flag;
{
	Flist* p;

	if (f == NULL || !*f || flag < 0 || flag > 1)
		fatal("bad argument to addfile()", (char*)0, (char*)0);
	if (objp == NULL)
		return;

	/* might already be on list if instantiation object */

	if (flag) {
		p = fhead;
		while (p != NULL) {
			if (!strcmp(f, p->file))
				break;
			p = p->next;
		}
		if (p != NULL) {
			obj_del2(p->obj);
			p->obj = objp;
			return;
		}
	}

	/* otherwise add to list */

	p = (Flist*)gs(sizeof(Flist));
	p->obj = objp;
	p->file = copy(f);
	p->next = NULL;
	p->rep = flag;
	if (!flag) {
		if (fhead == NULL) {
			fhead = p;
			fcurr = fhead;
		}
		else {
			fcurr->next = p;
			fcurr = p;
		}
	}
	else {
		if (fhead == NULL) {
			fhead = p;
			fcurr = p;
		}
		else {
			p->next = fhead;
			fhead = p;
		}
	}
}

/* delete a list of object files */
/* obj_del() - cleaning up at the end of execution
   This function cleans up all of the Obj records in the
   global Flist pointed to by fhead.
 */
void obj_del()
{
	Flist* p;
	Flist* p2;

	p = fhead;
	while (p != NULL) {
		fs((char*)p->file);
		obj_del2(p->obj);
		p2 = p;
		p = p->next;
		fs((char*)p2);
	}

	fhead = NULL;
	fcurr = NULL;
}

typedef struct Symbol Symbol;
struct Symbol {
	char* name;
	char* file;
	char* on;
	Symbol* next;
	char* rn;
	char* f;
	char type;
	char rep;
};

static Symbol* symtab[SYMTSIZE];

/* The following functions (lookup(), add(), and symtab_del())
   manipulate the global symbol table stored in the array "symtab"
 */
/* look up a symbol */
Symbol* lookup(s)
char* s;
{
	Symbol* p;

	if (s == NULL || !*s)
		fatal("bad argument to lookup()", (char*)0, (char*)0);

	p = symtab[(int)hash(s, (long)SYMTSIZE)];
		/* find which hash bucket to search */
	
	while (p != NULL) {	/* look at each Symbol in the bucket */
		if (!strcmp(s, p->name))
			return p;
		p = p->next;
	}

	return NULL;
}

/* add a symbol */
/* add(s, t, f, rep, on) - add a symbol to the global symbol table
   s is the name of the symbol
   t is the "type" of the symbol (the one-letter code from the "nm" output)
   f is file that the symbol comes from
   rep is 0 for all symbols added to the symbol table from the link line's .o
     and .a files, 1 for all symbols from instantiation files
   on is non-null if the symbol came from a library archive.
     Its value is the name of the .o file within the library.
     This allows fakelink() to produce an error messages for unresolved
     external references that contain both the library and .o filenames.
 */
void add(s, t, f, rep, on)
char* s;
char t;
char* f;
char rep;
char* on;
{
	Symbol* p;
	int h;

	if (s == NULL || !*s || f == NULL || !*f || rep < 0 || rep > 1)
		fatal("bad argument to add()", (char*)0, (char*)0);

	p = symtab[h = (int)hash(s, (long)SYMTSIZE)];
		/* find which hash bucket to search */
	
	/* may be there already */

	while (p != NULL) {
		if (!strcmp(s, p->name))
			break;
		p = p->next;
	}

	/* add if not */

	if (p == NULL) {
		p = (Symbol*)gs(sizeof(Symbol));
		p->next = symtab[h];
		symtab[h] = p;	/* add the new node to the hash table bucket */
		p->name = copy(s);
		p->file = copy(f);
		p->on = (on != NULL ? copy(on) : NULL);
		p->type = t;
		p->rep = rep;
		p->rn = NULL;
		p->f = NULL;
	}
	else {
		p->type = t;
		fs((char*)p->file);
		p->file = copy(f);
		p->rep = rep;
	}
}

/* delete the symbol table */
void symtab_del()
{
	int i;
	Symbol* p;
	Symbol* r;

	for (i = 0; i < SYMTSIZE; i++) {
		p = symtab[i];
		while (p != NULL) {
			fs((char*)p->name);
			fs((char*)p->file);
			if (p->rn != NULL)
				fs((char*)p->rn);
			if (p->f != NULL)
				fs((char*)p->f);
			if (p->on != NULL)
				fs((char*)p->on);
			r = p;
			p = p->next;
			fs((char*)r);
		}
		symtab[i] = NULL;
	}
}

/************************* SIMULATE LINKING *************************/

/* see if adding object with create conflicts */
/* okobj(nl,f,a) - look for possible multiple definitions
   nl is a pointer to the name list section of an Obj structure -- it's
      a list of the defined and undefined global symbols for a single .o file.

   okobj() returns 1 if this object file's symbols can be added to the
   global symbol table without causing multiple definitions.  The global
   symbol table will not be changed.

   If there will be a conflict, then okobj() will kludge the global
   symbol table:  any symbol defined in the file that is undefined ("U"
   or "C") in the global symbol table will have its type changed in
   the global symbol table to "u".  This will help keep the error
   messages consistent.
 */
int okobj(nl, f, a)
Ename* nl;
char* f;
char* a;
{
        Symbol* sp;
        char t;
        char t2;
        int ok;
        Ename* nl2;
        char* sy;
        char arc[MAXPATH];
 
        if (nl == NULL || !f || !*f)
                fatal("bad argument to okobj()", (char*)0, (char*)0);
 
        /* check for conflicts */
 
        ok = 1;
        nl2 = nl;
        while (nl != NULL) {
                t = nl->type;
                if (t == 'T' || t == 'D' || t == 'B') {
                        if ((sp = lookup(nl->name)) != NULL) {
                                t2 = sp->type;
                                if (t2 == 'T' || t2 == 'D' || t2 == 'B') {
                                        ok = 0;
                                        sy = nl->name;
                                        break;
                                }
                        }
                }
                nl = nl->next;
        }
        if (ok)
                return 1;
        if (a)
                sprintf(arc, "[%s]", a);
        else
                arc[0] = 0;
        fprintf(stderr, "%s warning: ##### duplicate symbol %s, %s%s%s not added #####\n", PROGNAME, sy, f, a ? OBJ_EXT : "", arc);

        /* found conflict, make all symbols in the object undefinable */

        while (nl2 != NULL) {
                t = nl2->type;
                if (t == 'T' || t == 'D' || t == 'B') {
                        if ((sp = lookup(nl2->name)) != NULL) {
                                t2 = sp->type;
                                if (t2 == 'U' || t2 == 'C')
                                        sp->type = 'u';
                        }
                }
                nl2 = nl2->next;
        }

        return 0;
}

/* add one object */
void addobj(nl, f, rep, on, fp)
Ename* nl;
char* f;
char rep;
char* on;
FILE* fp;
{
	char t;
	Symbol* sp;

	if (f == NULL || !*f || rep < 0 || rep > 1)
		fatal("bad argument to addobj()", (char*)0, (char*)0);

	while (nl != NULL) {
		if (fp != NULL)
			fprintf(fp, "\t%c %s\n", nl->type, nl->name);
		t = nl->type;

		/* always add symbols that are already defined */

		if (t == 'B' || t == 'D' || t == 'T') {
			add(nl->name, t, f, rep, on);
		}

		/* add undefined if not already seen */

		else if (t == 'U') {
			if (lookup(nl->name) == NULL)
				add(nl->name, t, f, rep, on);
		}

		/* add global common if not already seen or if
		   previously undefined */

		else if (t == 'C') {
			sp = lookup(nl->name);
			if (sp == NULL || sp->type == 'U')
				add(nl->name, t, f, rep, on);
		}
		else {
			fatal("bad symbol table type in addobj()", (char*)0, (char*)0);
		}
		nl = nl->next;
	}
}

/* fake a link */
void fakelink(fp)
FILE* fp;
{
	Flist* flp;
	Obj* op;
	char t;
	int pulled_in;
	int addflag;
	Symbol* sp;
	Ename* nl;
	char* f;
	char rep;
	int oko;
	static int num = 1;

	if (fp != NULL)
		fprintf(fp, "\n*************** LINK SIMULATION #%d ***************\n\n", num++);

	flp = fhead;
	while (flp != NULL) {
		op = flp->obj;
		f = flp->file;
		rep = flp->rep;

		/* an object, unconditionally link its symbols */

		if (!op->aflag) {
			oko = okobj(op->nlist, op->file, 0);
			if (fp != NULL && oko)
				fprintf(fp, "%s (unconditional):\n", f);
			if (oko)
				addobj(op->nlist, f, rep, (char*)0, fp);
		}

		/* an archive;  conditionally link */

		else {

			/* clear pulled-in flags */

			while (op != NULL) {
				op->pi = 0;
				op = op->next;
			}

/* multiple passes through an archive might be necessary, so
   execution returns to this label after any .o file's symbols
   are added to the symbol table */
loop:
			pulled_in = 0;
			op = flp->obj;
			while (op != NULL) {

				/* ignore if already pulled in */

				if (op->pi) {
					op = op->next;
					continue;
				}
				nl = op->nlist;
				addflag = 0;

				/* add object if its symbols are needed */

				while (nl != NULL) {
					t = nl->type;
					if (t == 'B' || t == 'D' || t == 'T') {
						sp = lookup(nl->name);
						if (sp != NULL && (sp->type == 'U' || sp->type == 'C'))
							addflag = 1;
					}
					else if (t == 'C') {
						sp = lookup(nl->name);
						if (sp != NULL && sp->type == 'U')
							addflag = 1;
					}
					if (addflag) {
						oko = okobj(op->nlist, op->file, f);
						if (oko) {
							pulled_in++;
							op->pi = 1;
							if (fp != NULL) {
								fprintf(fp, "%s.o[%s] (symbol %s needed by ", op->file, f, nl->name);
								if (sp->on != NULL)
									fprintf(fp, "%s.o[%s]):\n", sp->on, sp->file);
								else
									fprintf(fp, "%s):\n", sp->file);
							}
							addobj(op->nlist, f, rep, op->file, fp);
						}
						break;
					}
					nl = nl->next;
				}
				op = op->next;
			}
			if (pulled_in > 0)
				goto loop;
		}
		flp = flp->next;
	}
}

/************************* NAME DEMANGLING *************************/

/* see if a a name is PTish */
int ispt(s, f)
char* s;
char* f;
{
	DEM d;
	char buf[MAXDBUF];

	if (s == NULL || !*s || f == NULL)
		fatal("bad argument to ispt()", (char*)0, (char*)0);

	*f = 0;
	if (dem(s, &d, buf) < 0)
		return 0;

	/* member of a template class */

	if (d.cl != NULL && d.cl->clargs != NULL && d.vtname == NULL)
		return 1;

	/* function template or regular function */

	if (d.cl == NULL && d.args != NULL) {
		strcpy(f, d.f);
		return 1;
	}

	return 0;
}

/* get the distinguishing part of a PT name */
char* getname(s, pbuf, dbuf)
char* s;
char* pbuf;
char* dbuf;
{
	DEM d;

	if (s == NULL || !*s || pbuf == NULL || dbuf == NULL)
		fatal("bad argument to getname()", (char*)0, (char*)0);

	/* use the raw PT template class name */

	if (dem(s, &d, dbuf) < 0)
		fatal("call to demangler failed for %s", s, (char*)0);
	if (d.cl != NULL) {
		dem_printcl(d.cl, pbuf);
		return d.cl->rname;
	}

	/* or the function template name */

	else {
		dem_print(&d, pbuf);
		return s;
	}
}

/* make a filename from a template class name */
void makefnam(ptr, s, bbuf, hflag)
char* ptr;
char* s;
char* bbuf;
int hflag;
{
	char buf1[MAXPATH];
	char buf[MAXLINE];
	unsigned long h;
	unsigned long hs;
	FILE* fp;

	if (ptr == NULL || !*ptr || s == NULL || !*s || bbuf == NULL ||
	    hflag < 0 || hflag > 1)
		fatal("bad argument to makefnam()", (char*)0, (char*)0);

	if (!hflag) {
		strcpy(bbuf, s);
	}
	else {
		h = hash(s, 90000049L);		/* allow room for overflows */
		hs = h;

		/* find a hash code that is unused */

		for (;;) {
			sprintf(bbuf, "pt%08lu", h);
			sprintf(buf1, "%s/%s%s", ptr, bbuf, HC_EXT);
			if ((fp = fopen(buf1, "r")) == NULL) {
				if (facc(buf1))
					fatal("cannot open hash conflict file %s for reading", buf1, (char*)0);
				break;
			}
			fgets(buf, MAXLINE, fp);
			fclose(fp);
			buf[strlen(buf) - 1] = 0;
			if (!strcmp(buf, s))
				break;
			h++;
		}
		del_file(buf1);
		if ((fp = fopen(buf1, "w")) == NULL)
			fatal("could not create hash collision file %s", buf1, (char*)0);
		fprintf(fp, "%s\n", s);
		fclose(fp);

		if (h != hs)
			fprintf(stderr, "%s warning: ##### filename hash conflict for %s, using pt%08lu instead of pt%08lu #####\n", PROGNAME, s, h, hs);
	}
}

/************************* NAME MAPPING STUFF *************************/

typedef struct Tfile Tfile;
struct Tfile {
	char* name;
	Slist files;
	Slist bns;
	Tfile* next;
	int fnum;
	char type;
};

/* get a new file table */
Tfile** tf_get()
{
	Tfile** p;
	int i;

	p = (Tfile**)gs(sizeof(Tfile*) * TFTABSIZE);
	for (i = 0; i < TFTABSIZE; i++)
		p[i] = NULL;
	return p;
}

static int fcnt = 0;

/* delete a file/type table */
void tf_del(tftab)
Tfile** tftab;
{
	Tfile* p;
	Tfile* r;
	int i;

	if (tftab == NULL)
		fatal("bad argument to tf_del()", (char*)0, (char*)0);

	for (i = 0; i < TFTABSIZE; i++) {
		p = tftab[i];
		tftab[i] = NULL;
		while (p != NULL) {
			fs((char*)p->name);
			slp_del(&p->files);
			slp_del(&p->bns);
			r = p;
			p = p->next;
			fs((char*)r);
		}
	}
}

/* read a map file */
void tf_read(f, tftab)
char* f;
Tfile** tftab;
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
	Slist tab;
	int flag;
	int indx;
	int len2;
	int nbl;
	char* t2;

	if (f == NULL || !*f || tftab == NULL)
		fatal("bad argument to tf_read()", (char*)0, (char*)0);
	if ((fp = fopen(f, "r")) == NULL) {

		/* may be defmap file, where failure to */
		/* read should only be a warning */

		if (!fcnt) {
			fcnt++;
			fprintf(stderr, "%s warning: ##### cannot open map file %s for reading #####\n", PROGNAME, f);
			return;
		}
		fatal("cannot open map file %s for reading", f, (char*)0);
	}

	ln = 0;
	slp = NULL;
	tab.n = 0;
	nbl = 0;

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
		nbl++;

		/* might be string table */

		if (!strcmp(s, "@tab") && nbl == 1) {
			flag = 0;
			while (fgets(buf, MAXMAPL, fp) != NULL) {
				ln++;
				len2 = strlen(buf) - 1;
				if (buf[len2] != '\n') {
					sprintf(lbuf, "%d", ln);
					fatal("missing newline at line %s of map file %s", lbuf, f);
				}
				buf[len2] = 0;
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
			if (!err) {
				t = s + 5;
				while (*t && *t <= ' ')
					t++;
			}
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
			p->type = s[3];

			/* get type name */

			t = s + 5;
			while (*t && *t <= ' ')
				t++;
			t2 = t;
			while (isalnum(*t) || *t == '_')
				t++;
			if (*t)
				*t++ = 0;
			p->name = copy(t2);
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
					}
				}
			}
			p->fnum = fcnt;
			h = (int)hash(p->name, (long)TFTABSIZE);
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
	fcnt++;
	slp_del(&tab);
}

/* get the type names used in a parameter list */
void getcl(clp, slp)
DEMCL* clp;
Slist* slp;
{
	void getfcl();

	if (clp == NULL || slp == NULL)
		fatal("bad argument to getcl()", (char*)0, (char*)0);

	/* add the class name */

	slp_adduniq(slp, clp->name);

	/* might be a template class;  recurse if so */

	if (clp->clargs != NULL)
		getfcl(clp->clargs, slp);
}

/* get the type names used in a function template parameter list */
void getfcl(p, slp)
DEMARG* p;
Slist* slp;
{
	DEMCL** r;

	if (p == NULL || slp == NULL)
		fatal("bad argument to getfcl()", (char*)0, (char*)0);

	while (p != NULL) {
		if (p->clname != NULL)
			getcl(p->clname, slp);
		if (p->mname != NULL) {
			r = p->mname;
			while (*r != NULL) {
				getcl(*r, slp);
				r++;
			}
		}
		if (p->func != NULL)
			getfcl(p->func, slp);
		if (p->ret != NULL)
			getfcl(p->ret, slp);
		p = p->next;
	}
	
}

#if 0
/* check whether an include exists */
int check_incl(incl, f, quot)
Slist* incl;
char* f;
char quot;
{
	char fbuf[MAXPATH];
	int i;
	char* s;

	if (f == NULL || !*f || incl == NULL || (quot != '"' && quot != '<'))
		fatal("bad argument to check_incl()", (char*)0, (char*)0);

	/* hard path or current directory */

	if ((quot == '"' || f[0] == '/') && facc(f))
		return 0;

	/* go through list */

	for (i = 0; i < incl->n; i++) {
		s = incl->ptrs[i];
		if (s[0] != '-' || s[1] != 'I')
			continue;
		s += 2;
		sprintf(fbuf, "%s/%s", s, f);
		if (facc(fbuf))
			return 0;
	}

	return -1;
}
#endif


/* find a file across all -I and all extensions */
/* find_incl(incl, f, ext, quot, extout)
   incl is an Slist that contains all of the -I options on the command line
   f is the basename of a file
   ext is an array strings containing potential file extensions
   quot is either '"' or '<'
   extout is a pointer to a buffer where the result will be stored

   The output in extout will be the basename f with one of the extensions
   in ext tacked onto the end, if such a file exists in one of the directories
   indicated by the -I options.  If no match is found, then extout will
   be unchanged.
 */
void find_incl(incl, f, ext, quot, extout)
Slist* incl;
char* f;
char* ext[];
char quot;
char* extout;
{
	int i;
	int j;
	char path[MAXPATH];
	char fbuf[MAXPATH];
	char* s;

	if (f == NULL || !*f || ext == NULL ||
	    (quot != '"' && quot != '<') || extout == NULL)
		fatal("bad argument to find_incl()", (char*)0, (char*)0);

	*extout = 0;

	/* current directory or hard path */

	if (quot == '"' || f[0] == '/') {
		i = 0;
		while (ext[i] != NULL) {
			sprintf(path, "%s%s", f, ext[i]);
			if (facc(path)) {
				strcpy(extout, path);
				return;
			}
			i++;
		}
	}

	/* iterate over -I */

	if (incl == NULL)
		return; 
	for (i = 0; i < incl->n; i++) {
		s = incl->ptrs[i];
		if (s[0] != '-' || s[1] != 'I')
			continue;
		s += 2;
		sprintf(fbuf, "%s/%s", s, f);
		j = 0;
		while (ext[j] != NULL) {
			sprintf(path, "%s%s", fbuf, ext[j]);
			if (facc(path)) {
				sprintf(extout, "%s%s", f, ext[j]);
				return;
			}
			j++;
		}
	}
}

/* add to a unique include file list */
void addincl(t, slp, tfp, ext, incl)
char* t;
Slist* slp;
Slist* tfp;
int ext;
Slist* incl;
{
	int i;
	char buf[MAXPATH];
	char buf2[MAXPATH];
	char c1;
	char c2;
	char extout[MAXPATH];
	char* s;

	if (t == NULL || !*t || slp == NULL || tfp == NULL ||
	    ext < -1 || ext > 1)
		fatal("bad argument to addincl()", (char*)0, (char*)0);

	/* maybe type not found in map files */

	if (!tfp->n) {
		if (ext == -1)
			fatal("ext == -1 in addincl()", (char*)0, (char*)0);

		/* find the file across -I and extensions */

		find_incl(incl, t, ext == 0 ? hdr_ext : src_ext, '"', extout);

		if (!extout[0]) {
			fprintf(stderr, "%s warning: ##### template %s file not found #####\n", PROGNAME, ext == 0 ? "declaration" : "definition");
			return;
		}
		sprintf(buf2, "#include \"%s\"", extout);
		slp_adduniq(slp, buf2);
		return;
	}

	/* iterate over all map file entries */

	for (i = 0; i < tfp->n; i++) {
		strcpy(buf2, tfp->ptrs[i]);
		c1 = '"';
		c2 = '"';
		if (buf2[0] == '"') {
			buf2[strlen(buf2) - 1] = 0;
			strcpy(buf, buf2 + 1);
		}
		else if (buf2[0] == '<') {
			buf2[strlen(buf2) - 1] = 0;
			strcpy(buf, buf2 + 1);
			c1 = '<';
			c2 = '>';
		}
		else {
			strcpy(buf, buf2);
		}

		if (ext != -1) {

			/* strip out extension */

			s = buf + strlen(buf) - 1;
			while (s >= buf && *s != '.')
				s--;
			if (s >= buf)
				*s = 0;

			/* look up file */

			find_incl(incl, buf, ext == 0 ? hdr_ext : src_ext, c1, extout);
			/* have to assume T.x? */

			if (!extout[0]) {
				find_incl(incl, t, ext == 0 ? hdr_ext : src_ext, c1, extout);
				if (!extout[0]) {
					fprintf(stderr, "%s warning: ##### template %s file not found #####\n", PROGNAME, ext == 0 ? "declaration" : "definition");
					continue;
				}
			}
			strcpy(buf, extout);
		}

		/* format and add to list */

		sprintf(buf2, "#include %c%s%c", c1, buf, c2);
		slp_adduniq(slp, buf2);
	}
}

/* validate a file basename */
int valid_bn(r)
Tfile* r;
{
	int i;
	Slist* slp;

	if (r == NULL)
		fatal("bad argument to valid_bn()", (char*)0, (char*)0);

	slp = &r->bns;
	for (i = 0; i < slp->n; i++) {
		if (flookup(slp->ptrs[i], ftable) != NULL)
			return 1;
	}

	return 0;
}

/* read and sort a list of map file names from a directory */
/* read_mname(dir, list, defmap)
   dir is the name of the repository directory
   list is an Slist that will be added to in this function
   defmap is the name of the standard map file (normally "defmap")

   read_mname() will add to the list any filenames in the repository
   directory beginning with "nmap".  Then it will add the value in
   defmap.
 */
void read_mname(dir, list, defmap)
char* dir;
Slist* list;
char* defmap;
{
	char cmd[MAXCBUF];
	FILE* fp;
	char fbuf[MAXPATH];
	int i;
	int j;
	char* t;
	int n;

	if (dir == NULL || !*dir || list == NULL ||
	    defmap == NULL || !*defmap)
		fatal("bad argument to read_mname()", (char*)0, (char*)0);

	sprintf(fbuf, "%s/%s", dir, defmap);
	slp_add(list, fbuf);

	/* do an "ls" on the repository */

	sprintf(cmd, "%s %s/nmap* 2>/dev/null", LSPATH, dir);
	if ((fp = popen(cmd, "r")) == NULL)
		fatal("popen() failed on %s while reading map files", cmd, (char*)0);
	while (fgets(fbuf, MAXPATH, fp) != NULL) {
		fbuf[strlen(fbuf) - 1] = 0;
		slp_add(list, fbuf);
	}
	pclose(fp);

	/* reverse the list */

	n = (list->n - 1) / 2;
	i = 1;
	j = list->n - 1;
	while (n-- > 0) {
		t = list->ptrs[i];
		list->ptrs[i] = list->ptrs[j];
		list->ptrs[j] = t;
		i++;
		j--;
	}
}

/* tlookup() helper function */
int tlookup_help(t, r, pdp, pip, minfile, maxfile, bnflag)
char* t;
Tfile* r;
Tfile** pdp;
Tfile** pip;
int minfile;
int maxfile;
int bnflag;
{
	Tfile* pd;
	Tfile* pi;
	int pdnum;
	int pinum;
	int err;

	if (t == NULL || !*t || pdp == NULL || pip == NULL ||
	    minfile < 0 || maxfile < 0 || minfile > maxfile ||
	    bnflag < 0 || bnflag > 1)
		fatal("bad argument to tlookup_help()", (char*)0, (char*)0);

	pd = NULL;
	pi = NULL;
	pdnum = -1;
	pinum = -1;
	err = 0;

	for (; r != NULL; r = r->next) {

		/* check file number */

		if (r->fnum < minfile || r->fnum > maxfile)
			continue;

		/* check name */

		if (strcmp(r->name, t))
			continue;

		/* check basenames flag */

		if ((bnflag && !valid_bn(r)) || (!bnflag && r->bns.n))
			continue;

		if (r->type == 'c') {
			pd = (pd == NULL ? r : pd);

			/* check for duplicates */

			if (pdnum == r->fnum) {
				err = 1;
				break;
			}
			pdnum = r->fnum;
		}
		else {
			pi = (pi == NULL ? r : pi);

			/* check for duplicates */

			if (pinum == r->fnum) {
				err = 1;
				break;
			}
			pinum = r->fnum;
		}
	}

	if (err)
		return -2;
	if (pd == NULL && pi == NULL)
		return -1;

	*pdp = pd;
	*pip = pi;
	return 0;
}

/* look up a type in one set of map files */
int tlookup(t, tlp, fflag, tftab, incl)
char* t;
Slist* tlp;
char fflag;
Tfile** tftab;
Slist* incl;
{
	Tfile* pd;
	Tfile* pi;
	int h;
	int err;

	if (t == NULL || !*t || tlp == NULL ||
	    (fflag != 'c' && fflag != 'f') || tftab == NULL)
		fatal("bad argument to tlookup()", (char*)0, (char*)0);

	/* find the type */

	h = (int)hash(t, (long)TFTABSIZE);

	/* check nmapXXX files with basenames */

	err = tlookup_help(t, tftab[h], &pd, &pi, 1, 9999, 1);

	/* check nmapXXX files without basenames */

	if (err == -1)
		err = tlookup_help(t, tftab[h], &pd, &pi, 1, 9999, 0);

	/* check default map file with basenames */

	if (err == -1)
		err = tlookup_help(t, tftab[h], &pd, &pi, 0, 0, 1);

	/* check default map file without basenames */

	if (err == -1)
		err = tlookup_help(t, tftab[h], &pd, &pi, 0, 0, 0);

	/* check out errors */

	if (err == -2)
		fatal("type %s defined twice in map files", t, (char*)0);
	if (err == -1)
		return -1;

	/* format to instantiation file */

	if (fflag == 'f') {
		addincl(t, tlp, pd == NULL ? &pi->files : &pd->files,
		    pd == NULL ? 0 : -1, incl);
		addincl(t, tlp, pi == NULL ? &pd->files : &pi->files,
		    pi == NULL ? 1 : -1, incl);
	}
	else {
		addincl(t, tlp, pd == NULL ? &pi->files : &pd->files,
		    pd == NULL ? 0 : -1, incl);
	}

	return 0;
}

static Tfile*** tf_cache = NULL;

/* look up a type in map files */
int tlookup2(name, tlist, t, tfp, dlp, defmap, incl)
char* name;
Slist* tlist;
char t;
Tfile** tfp;
Slist* dlp;
char* defmap;
Slist* incl;
{
	int j;
	int k;
	Slist* mlp;
	Slist ml;

	if (name == NULL || !*name || tlist == NULL ||
	    (t != 'c' && t != 'f') || tfp == NULL || dlp == NULL ||
	    defmap == NULL || !*defmap)
		fatal("bad argument to tlookup2()", (char*)0, (char*)0);

	mlp = &ml;

	if (tlookup(name, tlist, t, tfp, incl)) {

		/* allocate cache */

		if (tf_cache == NULL) {
			tf_cache = (Tfile***)gs(sizeof(Tfile**) * dlp->n);
			for (j = 0; j < dlp->n; j++)
				tf_cache[j] = NULL;
		}


		/* look in other repositories */

		for (j = 1; j < dlp->n; j++) {
			if (tf_cache[j] == NULL) {
				tf_cache[j] = tf_get();
				mlp->n = 0;
				read_mname(dlp->ptrs[j], mlp, defmap);
				fcnt = 0;
				for (k = 0; k < mlp->n; k++)
					tf_read(mlp->ptrs[k], tf_cache[j]);
				slp_del(mlp);
			}
			if (!tlookup(name, tlist, t, tf_cache[j], incl))
				break;
		}

		/* anything found? */

		if (j == dlp->n)
			return -1;
	}

	return 0;
}

/* get list of undefined symbols */
void getund(slp, tfp, dlp, defmap, rawn)
Slist* slp;
Tfile** tfp;
Slist* dlp;
char* defmap;
Slist* rawn;
{
	int i;
	Symbol* sp;
	Slist tlist;
	char f[MAXLINE];
	char dbuf[MAXDBUF];
	DEM d;

	if (slp == NULL || tfp == NULL || dlp == NULL ||
	    defmap == NULL || !*defmap || rawn == NULL)
		fatal("bad argument to getund()", (char*)0, (char*)0);

	tlist.n = 0;

	for (i = 0; i < SYMTSIZE; i++) {
		sp = symtab[i];
		while (sp != NULL) {

			/* must be undefined and PT-ish */

			if ((sp->type == 'U' || sp->type == 'u' || sp->type == 'C') &&
			    ispt(sp->name, f)) {

				/* check for possible function template */

				if (!*f || !tlookup2(f, &tlist, 'c', tfp, dlp, defmap, (Slist*)0))
					slp_add(slp, sp->name);
				slp_del(&tlist);
			}
			sp = sp->next;
		}
	}

	slp_sort(slp);

	/* create list of raw names for each symbol */

	for (i = 0; i < slp->n; i++) {
		if (dem(slp->ptrs[i], &d, dbuf) < 0)
			fatal("call to demangler failed for %s", slp->ptrs[i], (char*)0);
		if (d.cl != NULL && d.cl->rname != NULL)
			slp_add(rawn, d.cl->rname);
		else
			slp_add(rawn, " ");
	}
}

/****************************** INSTANTIATION ******************************/

/* write set of includes to instantiation file */
void doincl(fp, name, tfp, dlp, defmap, incl, tlist)
FILE* fp;
char* name;
Tfile** tfp;
Slist* dlp;
char* defmap;
Slist* incl;
Slist* tlist;
{
	DEM d;
	int i;
	Slist clist;
	char t;
	char buf[MAXLINE];
	char* n;
	Slist dlist;
	char dbuf[MAXDBUF];

	if (fp == NULL || name == NULL || !*name ||
	    tfp == NULL || dlp == NULL || defmap == NULL || !*defmap ||
	    incl == NULL || tlist == NULL)
		fatal("bad argument to doincl()", (char*)0, (char*)0);

	clist.n = 0;

	/* figure out needed types */

	if (dem(name, &d, dbuf) < 0)
		fatal("call to demangler failed for %s", name, (char*)0);
	if (d.cl != NULL) {
		getcl(d.cl, &clist);
#if 0
		if (d.args != NULL)
			getfcl(d.args, &clist);
		if (d.fargs != NULL)
			getfcl(d.fargs, &clist);
#endif
	}
	else {
		slp_add(&clist, d.f);
		getfcl(d.args, &clist);
		if (d.fargs != NULL)
			getfcl(d.fargs, &clist);
	}

	/* look them up */

	for (i = 0; i < clist.n; i++) {
		n = clist.ptrs[i];
		t = (i == 0 ? 'f' : 'c');
		if (tlookup2(n, tlist, t, tfp, dlp, defmap, incl)) {

			/* forwardize type */

			if (t == 'c') {
				sprintf(buf, "struct %s;", n);
				slp_adduniq(tlist, buf);
			}

			/* a template type, can't forwardize */

			else {
				dlist.n = 0;
				addincl(n, tlist, &dlist, 0, incl);
				addincl(n, tlist, &dlist, 1, incl);
				slp_del(&dlist);
			}
		}
	}

	/* add to instantiation file */

	for (i = 0; i < tlist->n; i++)
		fprintf(fp, "%s\n", tlist->ptrs[i]);

	slp_del(&clist);
}

/* handle literals for an instantiation */
void do_lit(fp, clp)
FILE* fp;
DEMCL* clp;
{
	DEMARG* p;
	DEMCL** r;
	char* s;
	char abuf[MAXLINE];
	char* t;

	if (fp == NULL || clp == NULL)
		fatal("bad argument to do_lit()", (char*)0, (char*)0);

	p = clp->clargs;

	/* step through arguments */

	while (p != NULL) {
		if (p->lit != NULL) {
			s = p->lit;

			/* might be pointer to member */

			while (*s) {
				if (*s == ':')
					break;
				s++;
			}

			/* skip pointers to members and integral literals */

			if (!*s && (isalpha(p->lit[0]) || p->lit[0] == '_')) {
				s = p->mods;

				/* remove one level of pointer */

				while (*s && *s != 'P')
					s++;
				if (*s == 'P') {
					if (s > p->mods && s[-1] == 'C')
						strcpy(s - 1, s + 1);
					else
						strcpy(s, s + 1);
				}
				if (!p->mods[0])
					p->mods = NULL;

				/* format the result as an extern */

				t = p->lit;
				p->lit = NULL;
				dem_printarg(p, abuf, 1);
				p->lit = t;
				fputs("extern ", fp);
				s = abuf;
				while (*s != '@') {
					fputc(*s, fp);
					s++;
				}
				if (s == abuf || !*s)
					fatal("missing @ in do_lit()", (char*)0, (char*)0);
				if (isalnum(s[-1]) || s[-1] == '_')
					fputc(' ', fp);
				fprintf(fp, "%s%s;\n", p->lit, s + 1);
			}
		}

		/* recurse if need be */

		if (p->clname != NULL)
			do_lit(fp, p->clname);
		if (p->mname != NULL) {
			r = p->mname;
			while (*r != NULL) {
				do_lit(fp, *r);
				r++;
			}
		}
		p = p->next;
	}
}

/* force an instantiation */
void doforce(fp, raw, fmt)
FILE* fp;
char* raw;
char* fmt;
{
	DEM d;
	char dbuf[MAXDBUF];
	char abuf[MAXLINE];
	int i;
	DEMARG* p;
	int n;
	char buf[MAXLINE];

	if (fp == NULL || raw == NULL || !*raw || fmt == NULL || !*fmt)
		fatal("bad argument to doforce()", (char*)0, (char*)0);

	if (dem(raw, &d, dbuf) < 0)
		fatal("call to demangler failed for %s", raw, (char*)0);

	/* for template classes, use typedef */

	if (d.cl != NULL) {
		do_lit(fp, d.cl);
		fprintf(fp, "typedef %s _dummy_;\n", fmt);
#ifdef ATTLC
fprintf(stderr, "Instantiating %s...\n", fmt);
#endif
	}

	/* for function templates, fake a call to the function */

	else {
#ifdef ATTLC
char localabuf[1024];
char *localaptr = localabuf;
#endif
		fprintf(fp, "static void _dummy_(");
		p = d.args;
		n = 0;

		/* print the arguments to the dummy function */

		while (p != NULL) {
			n++;
			dem_printarg(p, abuf, 1);
			i = 0;
			while (abuf[i] && abuf[i] != '@')
				i++;
			if (!i || !abuf[i])
				fatal("missing @ in doforce()", (char*)0, (char*)0);
			abuf[i] = 0;
			fprintf(fp, "%s%s%sa%d%s", n > 1 ? ", " : "", abuf, " ", n, abuf + i + 1);
#ifdef ATTLC
sprintf(localaptr, "%s%s%s", n > 1 ? ", " : "", abuf, abuf + i + 1);
while (*localaptr) localaptr++;
#endif
			p = p->next;
		}
		fprintf(fp, ")\n{\n");

		/* print the call to the function */

		dem_printfunc(&d, buf);
		fprintf(fp, "\t%s(", buf);
		for (i = 1; i <= n; i++)
			fprintf(fp, "%sa%d", i > 1 ? ", " : "", i);
		fprintf(fp, ");\n}\n");
#ifdef ATTLC
fprintf(stderr, "Instantiating %s(%s)...\n", buf, localabuf);
#endif
	}
}

/******************* COMPILE AN INSTANTIATION FILE ********************/

/* format a compile invocation */
/* fcomp(ccpath, incl, ccopts, type, buf, dip) - build up a compile
   command (but don't run it)

   ccpath is the name of the CC or cpp command
   incl is an Slist that contains all of the -I options from the command line
   ccopts is an Slist that contains all of the other command line options
   type is a string that contains extra compilation-specific arguments
     (like the -c option or the -ptr option to specify the repository)
   buf is a character buffer where fcomp builds the compilation command
   dip is an Slist that contains a list of functions that should be
     instantiated (using the +T "directed instantiation" option) --
     if dip is an empty list, there will be no +T option
 */
void fcomp(ccpath, incl, ccopts, type, buf, dip)
char* ccpath;
Slist* incl;
Slist* ccopts;
char* type;
char* buf;
Slist* dip;
{
	char fbuf[MAXPATH];
	FILE* fp;
	char dinst[MAXPATH];
	int i;

	if (ccpath == NULL || !*ccpath || incl == NULL ||
	    type == NULL || buf == NULL)
		fatal("bad argument to fcomp()", (char*)0, (char*)0);

	if (dip == NULL) {
		dinst[0] = 0;
	}

	/* directed instantiation */

	else {
		sprintf(fbuf, "%s/instfile", tdir);
		sprintf(dinst, "+T%s", fbuf);
		if ((fp = fopen(fbuf, "w")) == NULL)
			fatal("cannot open instantiation file %s for writing", fbuf, (char*)0);
		for (i = 0; i < dip->n; i++) {
			fprintf(fp, "%s\n", dip->ptrs[i]);
			progress("instantiating function: %s", dip->ptrs[i], (char*)0);
  		}
		fclose(fp);
	}

	sprintf(buf, "%s %s %s ", ccpath, type, dinst);

	/* includes */

	for (i = 0; i < incl->n; i++) {
		strcat(buf, incl->ptrs[i]);
		strcat(buf, " ");
	}

	/* other options */

	if (ccopts == NULL)
		return;
	for (i = 0; i < ccopts->n; i++) {
		strcat(buf, ccopts->ptrs[i]);
		strcat(buf, " ");
	}
}

#ifdef CHECKSUM
/*
 *  If CHECKSUM is defined, ptlink will use the old-fashioned scheme
 *  for building checksum files.  Each instantation file (.c file) in
 *  the repository will have a corresponding .cs file.  The first line
 *  of the .cs file will be a checksum that is generated from the
 *  preprocessor output of the .c file.
 *
 *  The dodep() function defined in this section will compute a new
 *  checksum for its instantiation file and compare it with the
 *  checksum stored in the .cs file to determine if recompilation is
 *  required.
 */
/* docs() - create the checksum based on the output of cpp
  
   docs() runs the preprocessor on the instantiation file, then
   feeds all of the lines that don't start with '#' into a checksum
   algorithm.  It also ignores leading blanks on each line.  docs()
   returns the checksum value.
  
  	ptr is the name of the repository directory
  	bbuf is the basename of the instantiation file
  	cpp is the C preprocessor command
  	incl is an Slist containing all the -I options from the command line
  	ccopts is an Slist containing all the other command line options

   build_cache() uses fcomp() to format the preprocessor line.
 */
unsigned long docs(ptr, bbuf, cpp, incl, ccopts)
char* ptr;
char* bbuf;
char* cpp;
Slist* incl;
Slist* ccopts;
{
	char cmd[MAXCBUF];
	char cmd2[MAXCBUF];
	FILE* fp;
	unsigned long cs;
	char buf1[MAXPATH];
	char buf2[MAXPATH];
	int flag;
	char buf[MAXCPP];
	char* s;

	if (ptr == NULL || !*ptr || bbuf == NULL || !*bbuf ||
	    cpp == NULL || !*cpp || incl == NULL || ccopts == NULL)
		fatal("bad argument to docs()", (char*)0, (char*)0);

	/* format */

	fcomp(cpp, incl, NULL, "", cmd, NULL);
	sprintf(buf1, "%s%s", bbuf, SRC_EXT);
	sprintf(buf2, "%s/%s", ptr, buf1);
	sprintf(cmd2, "%s %s", cmd, buf1);

	/* establish pipe */

	flag = (icmp(ptr, ".") == 0);
	if (flag)
		pushstring(buf1);
	signal_block(handlesig);
	if (flag && cp(buf2, buf1) < 0)
		fatal("could not copy %s --> %s", buf2, buf1);
	if ((fp = fast_popen(cmd2, "r")) == NULL) {
		if (flag)
			popstring();
		fatal("fast_popen() failed on %s while taking checksum", buf2, (char*)0);
	}

	/* read pipe, ignoring #line and leading whitespace */

	cs = 0;
	while (fgets(buf, MAXCPP, fp) != NULL) {
		s = buf;
		while (*s && *s <= ' ')
			s++;
		if (!*s || *s == '#')
			continue;
		while (*s)
			cs = cs * 271 + *s++;
	}
	fast_pclose(fp);
	signal_unblock();
	if (flag)
		popstring();

	/* return checksum, with 0 as special case */

	if (cs)
		return cs;
	else
		return 1L;
}

/* check timestamps */
/* check_time(r, bbuf, incl, tlist, wbuf2, main_rep) - check the timestamps of
   the header files that are directly included in an instantiation file

   In the CHECKSUM version of ptlink, the ".he" files (header file caches)
   are not created, so we can't easily find all of the header files that
   are indirectly included when an instantiation file is compiled.  This
   function is content with just checking the timestamps of the header files
   that are directly included in an instantiation file.

   r is the name of the repository directory
   bbuf is the "basename" of the instantiation file
   incl is an Slist that contains all the -I options on the command line
   tlist is an Slist that contains all the file names from the #include
      statements found in the instantiation file
   wbuf2 is a buffer to write error messages
   main_rep is the name of the main repository directory (which may be
      different from r)
 */
int check_time(r, bbuf, incl, tlist, wbuf2, main_rep)
char* r;
char* bbuf;
Slist* incl;
Slist* tlist;
char* wbuf2;
char* main_rep;
{
	char ibuf[MAXPATH];
	char ibuf2[MAXPATH];
	int i;
	int j;
	unsigned long tso;
	unsigned long tsi;
	char obj[MAXPATH];
	char c;
	int hard;

	if (r == NULL || !*r || bbuf == NULL || !*bbuf ||
	    incl == NULL || tlist == NULL || wbuf2 == NULL ||
	    main_rep == NULL || !*main_rep)
		fatal("bad argument to check_time()", (char*)0, (char*)0);

	wbuf2[0] = 0;

	/* get time stamp for object */

	sprintf(obj, "%s/%s%s", r, bbuf, OBJ_EXT);
	if ((tso = timestamp(obj)) == 0)
		return 0;

	/* go through list of includes and check against object */

	for (i = 0; i < tlist->n; i++) {
		if (strncmp(tlist->ptrs[i], "#include ", 9))
			continue;
		c = tlist->ptrs[i][9];
		strcpy(ibuf, tlist->ptrs[i] + 10);
		ibuf[strlen(ibuf) - 1] = 0;
		hard = 0;
		if (ibuf[0] == '/')
			hard = 1;

		/* local directories and hard paths */

		if ((c == '"' || hard) && (tsi = timestamp(ibuf))) {
			if (tsi < tso)
				continue;
			strcpy(wbuf2, ibuf);
			return 0;
		}

		/* go through -I paths */

		for (j = 0; j < incl->n; j++) {
			if (incl->ptrs[j][0] != '-' || incl->ptrs[j][1] != 'I')
				continue;
			if (hard)
				strcpy(ibuf2, ibuf);
			else
				sprintf(ibuf2, "%s/%s", incl->ptrs[j] + 2, ibuf);
			tsi = timestamp(ibuf2);
			if (tsi == 0)
				continue;
			if (tsi >= tso) {
				strcpy(wbuf2, ibuf);
				return 0;
			}
			else {
				break;
			}
		}
	}

	return 1;
}

/* check dependencies */
/* dodep(ptr, buf, ccpath, incl, ccopts, obuf, dlp, dip, cs,
	 tflag, wbuf, cpp, tlist, ptoflag)
	
   If there is already a .o file that is up to date in a
   repository, this function will copy the name of that .o file
   to obuf and return 0.  Otherwise, this function will return 1.

   ptr is the name of the main repository
   bbuf is the "basename" that should be looked for in the repository --
     if LONG_NAMES is defined, bbuf will be the name of the template class
     or template function being instantated, if LONG_NAMES is not defined,
     bbuf will be the hashed version of the name
   ccpath is name of the C++ compiler (not used in this function)
   incl is an Slist that contains the -I options specified on the original
     compilation line
   ccopts is an Slist that contains all the other options specified on the
     original compilation line (-D options, for example)
   obuf is a pointer to a buffer that can be filled with the name of a .o file
   dlp is an Slist that contains the names of all of the repositories
     to be searched
   dip is an Slist of the functions that should be instantiated
   cs is the address of the location where dodep() stores the checksum
   tflag is a boolean flag that indicates if -ptt was used on the command
     line -- don't check the checksums, compare the modification times
     of the header files instead (using the check_time() function)
   wbuf is a pointer to a character buffer that dodep() fills with any
     warning messages
   cpp is the name of the C preprocessor command
   tlist is a list of the #include files from the instantiation file, which
     will be passed to check_time() if tflag is set
   ptoflag is a boolean flag that indicates if -pto was used on the command
     line
 */

int dodep(ptr, bbuf, ccpath, incl, ccopts, obuf, dlp, dip, cs, tflag, wbuf, cpp, tlist, ptoflag)
char* ptr;
char* bbuf;
char* ccpath;
Slist* incl;
Slist* ccopts;
char* obuf;
Slist* dlp;
Slist* dip;
unsigned long* cs;
int tflag;
char* wbuf;
char* cpp;
Slist* tlist;
int ptoflag;
{
	char fbuf[MAXPATH];
	char fbuf2[MAXPATH];
	unsigned long csold;
	char csbuf[MAXLINE];
	char csbuf2[MAXLINE];
	FILE* fp;
	int i;
	char* r;
	char buf[MAXLINE];
	Slist cslist;
	int ret;
	Slist nlist;
	int j;
	char wbuf2[MAXPATH];
	Fname* fnp;

	if (ptr == NULL || !*ptr || bbuf == NULL || !*bbuf ||
	    ccpath == NULL || !*ccpath || incl == NULL ||
	    ccopts == NULL || obuf == NULL || !*obuf ||
	    dlp == NULL || dip == NULL || tflag < 0 || tflag > 1 ||
	    wbuf == NULL || cpp == NULL || !*cpp || tlist == NULL ||
	    ptoflag < 0 || ptoflag > 1)
		fatal("bad argument to dodep()", (char*)0, (char*)0);

	csbuf[0] = 0;
	cslist.n = 0;
	nlist.n = 0;
	*cs = 0;

	/* iterate over repositories */

	for (i = 0; i < dlp->n; i++) {
		r = dlp->ptrs[i];

		/* check for instantiation file */

		sprintf(fbuf2, "%s/%s%s", r, bbuf, SRC_EXT);
		if (!facc(fbuf2)) {
			strcpy(wbuf, "missing instantiation file");
			continue;
		}

		/* check for object */

		sprintf(fbuf, "%s/%s%s", r, bbuf, OBJ_EXT);
		if (!facc(fbuf)) {
			strcpy(wbuf, "missing object");
			continue;
		}

		/* maybe -pto; if on list then known to be out of date */

		sprintf(fbuf2, "%s%s", bbuf, OBJ_EXT);
		if (ptoflag && (fnp = flookup(fbuf2, otable)) != NULL) {
			strcpy(wbuf, "-pto indicates out of date");
			fs((char*)fnp->name);		/* can be out of */
			fnp->name = copy(" ");	/* date only once */
			continue;
		}

		/* see if needed members are a subset of */
		/* those stored with checksum */

		sprintf(fbuf, "%s/%s%s", r, bbuf, CS_EXT);
		if ((fp = fopen(fbuf, "r")) == NULL) {
			strcpy(wbuf, "checksum missing or unreadable");
			continue;
		}
		if (fgets(csbuf2, 25, fp) == NULL) {
			strcpy(wbuf, "checksum unreadable");
			fclose(fp);
			continue;
		}
		while (fgets(buf, MAXLINE, fp) != NULL) {
			buf[strlen(buf) - 1] = 0;
			slp_add(&cslist, buf);
		}
		fclose(fp);
		if (cslist.n == 1 && !strcmp(cslist.ptrs[0], "@alltc"))
			ret = 1;
		else
			ret = slp_subset(dip, &cslist, &nlist);

		/* might have to handle exclude list for @data */

		if (!ret &&
		    nlist.n == 1 && !strncmp(nlist.ptrs[0], "@data", 5) &&
		    !strncmp(dip->ptrs[0], "@data", 5) &&
		    !strncmp(cslist.ptrs[0], "@data", 5)) {
			char* ptr[2];
			Slist list[2];
			Slist scr;
			char* s;
			char* t;
			int k;
			ptr[0] = copy(dip->ptrs[0]);
			ptr[1] = copy(cslist.ptrs[0]);
			scr.n = 0;
			for (k = 0; k <= 1; k++) {
				s = ptr[k];
				list[k].n = 0;
				for (;;) {
					t = s;
					while (*s && *s > ' ')
						s++;
					if (*s)
						*s++ = 0;
					if (!*t)
						break;
					slp_add(&list[k], t);
					while (*s && *s <= ' ')
						s++;
				}
			}
			ret = slp_subset(&list[0], &list[1], &scr);
			slp_del(&list[0]);
			slp_del(&list[1]);
			slp_del(&scr);
			fs((char*)ptr[0]);
			fs((char*)ptr[1]);
		}
		slp_del(&cslist);
		if (!ret) {
			strcpy(wbuf, "members missing: ");
			for (j = 0; j < nlist.n; j++) {
				strcat(wbuf, j > 0 ? "," : "");
				strcat(wbuf, nlist.ptrs[j]);
			}
			slp_del(&nlist);
			continue;
		}
		slp_del(&nlist);

		/* might be -pto */

		if (ptoflag)
			break;

		/* might have specified to check timestamps only */

		if (tflag) {
			if (check_time(r, bbuf, incl, tlist, wbuf2, ptr)) {
				break;
			}
			else {
				sprintf(wbuf, "header changed: %s", wbuf2);
				continue;
			}
		}

		/* compare checksums */

		csold = docs(r, bbuf, cpp, incl, ccopts, dip);
		if (i == 0)
			*cs = csold;
		sprintf(csbuf, "%lx\n", csold);
		if (strcmp(csbuf, csbuf2)) {
			strcpy(wbuf, "checksum mismatch");
			continue;
		}

		break;
	}

	/* found a matching one */

	if (i < dlp->n) {
		sprintf(obuf, "%s/%s%s", r, bbuf, OBJ_EXT);
		strcpy(wbuf, "OK");
		return 0;
	}

	return 1;
}

#else
/*
 *  If CHECKSUM is not defined, ptlink will use the new header file
 *  cache scheme to determine if an instantiation file needs to be
 *  recompiled.  Each instantation file (.c file) in the repository
 *  will have a corresponding .cs and .he files.  The .cs file won't
 *  contain a checksum, just a list of the member functions that have
 *  been instantiated from the given class.
 *
 *  The .he file contains three things.  The first line is the set of
 *  -I and -D options that the instantiation file was compiled with.
 *  The following lines are the header files that are included directly
 *  or indirectly when the instantiation file was compiled.  The last
 *  line is a count of the bytes in the .he file (not including the last
 *  line and not including newline characters).
 *
 *  The dodep() in this section will call check_time(), which will
 *  march through the .he file to see if any of the header files have
 *  a more recent modification time than the .o file in the repository
 *  that was built from the instantiation file.  The build_cache()
 *  function will built a new .he file when it is called.
 */

/* rebuild cache */
/* build_cache(cfn, incl, cpp, id, ptr, bbuf) - build a ".he" file (a
   "header file cache") in the repository directory

   cfn is the name of the cache file that this function will write
   incl is an Slist that contains all of the -I options from the command line
   cpp is the name of the C preprocessor
   id is a string containing the compiler's command line options
   ptr is the name of the repository
   bbuf is the base name of the instantiation file

   build_cache() will run the C preprocessor on the instantiation file,
   then it will pick out the names of the header files that are included
   directly or indirectly (by looking at the #line directives in the cpp
   output).  These header file names will be written to a ".he" file in
   the repository directory.

   build_cache() uses fcomp() to format the preprocessor line.
 */
void build_cache(cfn, incl, cpp, id, ptr, bbuf)
char* cfn;
Slist* incl;
char* cpp;
char* id;
char* ptr;
char* bbuf;
{
	char cmd[MAXCBUF];
	char cmd2[MAXCBUF];
	int flag;
	char buf1[MAXPATH];
	char buf2[MAXPATH];
	char buf[MAXCPP];
	char prev[MAXPATH];
	Slist flist;
	FILE* fp;
	FILE* fp2;
	int i;
	char* s;
	char* t;
	long len;
	char buf3[MAXPATH];
	char cwd[MAXPATH];

	if (cfn == NULL || !*cfn || incl == NULL || cpp == NULL || !*cpp ||
	   id == NULL || ptr == NULL || !*ptr || bbuf == NULL || !*bbuf)
		fatal("bad argument to build_cache()", (char*)0, (char*)0);

	flist.n = 0;

	/* format cpp line */

	fcomp(cpp, incl, (Slist*)0, "", cmd, (Slist*)0);
	sprintf(buf1, "%s%s", bbuf, SRC_EXT);
	sprintf(buf2, "%s/%s", ptr, buf1);
	sprintf(cmd2, "%s %s", cmd, buf1);

	/* establish pipe */

	flag = (icmp(ptr, ".") == 0);
	if (flag)
		pushstring(buf1);
	signal_block(handlesig);
	if (flag && cp(buf2, buf1) < 0)
		fatal("could not copy %s --> %s", buf2, buf1);
	if ((fp = fast_popen(cmd2, "r")) == NULL) {
		if (flag)
			popstring();
		fatal("fast_popen() failed on %s while building cache", buf2, (char*)0);
	}

	/* read pipe and look for #lines */

	prev[0] = 0;
	get_cwd(cwd);
	while (fgets(buf, MAXCPP, fp) != NULL) {
		s = buf;

		/* skip # or #line */

		while (*s && *s <= ' ')
			s++;
		if (*s != '#')
			continue;
		t = s + 1;
		while (*s && *s > ' ')
			s++;
		if (*s)
			*s++ = 0;
		if (*t > ' ' && strcmp(t, "line"))
			continue;

		/* skip line number */

		while (*s && *s <= ' ')
			s++;
		if (!isdigit(*s))
			continue;
		while (*s && *s > ' ')
			s++;

		/* get filename */

		while (*s && *s <= ' ')
			s++;
		if (*s == '"')
			s++;
		if (!*s)
			continue;
		t = s;
		while (*s && *s > ' ' && *s != '"')
			s++;
		*s = 0;

		/* check if we've already seen this file */

		if (!strcmp(t, prev))
			continue;
		else
			strcpy(prev, t);

		/* screen out instantiation file */

		while (s > t && *s != '.')
			s--;
		if (s > t && !strcmp(s, SRC_EXT) && icmp(t, buf1))
			continue;

		/* add to unique list */

		if (*t != '/') {
			if (t[0] == '.' && t[1] == '/')
				t += 2;
			sprintf(buf3, "%s/%s", cwd, t);
			t = buf3;
		}

		slp_adduniq(&flist, t);
	}
	fast_pclose(fp);
	signal_unblock();
	if (flag)
		popstring();

	/* rewrite cache */

	del_file(cfn);
	if ((fp2 = fopen(cfn, "w")) == NULL)
		fatal("cannot open header cache %s for writing", cfn, (char*)0);
	fprintf(fp2, "%s\n", id);
	len = strlen(id);
	slp_sort(&flist);
	for (i = 0; i < flist.n; i++) {
		fprintf(fp2, "%s\n", flist.ptrs[i]);
		len += strlen(flist.ptrs[i]);
	}
	fprintf(fp2, "%ld\n", len);
	fclose(fp2);
	slp_del(&flist);

	progress("rebuilt header file cache", (char*)0, (char*)0);
}

struct Inode_cmp {
	unsigned long dev;
	unsigned long num;
};
typedef struct Inode_cmp Inode_cmp;

/* compare two Inode_cmp entries */
int cmp_in(p1, p2)
Inode_cmp* p1;
Inode_cmp* p2;
{
	if (p1->dev < p2->dev)
		return -1;
	if (p1->dev > p2->dev)
		return 1;
	if (p1->num < p2->num)
		return -1;
	if (p1->num > p2->num)
		return 1;
	return 0;
}

/* compare two lists of headers from header cache */
/* returns 1 if the lists are not equal, 0 if they are equal */
int comp_hdr(old, new)
Slist* old;
Slist* new;
{
	Inode_cmp* op;
	Inode_cmp* np;
	int i;

	if (old == NULL || new == NULL)
		fatal("bad argument to comp_hdr()", (char*)0, (char*)0);

	/* preliminary checks */

	if (!old->n)
		return 0;
	if (old->n != new->n)
		return 1;

	/* get devices and inodes for files and sort them */

	op = (Inode_cmp*)gs(old->n * sizeof(Inode_cmp));
	np = (Inode_cmp*)gs(new->n * sizeof(Inode_cmp));
	for (i = 0; i < old->n; i++) {
		get_devnum(old->ptrs[i], &op[i].dev, &op[i].num);
		get_devnum(new->ptrs[i], &np[i].dev, &np[i].num);
	}
	qsort((char*)op, old->n, sizeof(Inode_cmp), cmp_in);
	qsort((char*)np, new->n, sizeof(Inode_cmp), cmp_in);

	/* compare */

	for (i = 0; i < old->n; i++) {
		if (op[i].dev != np[i].dev || op[i].num != np[i].num)
			break;
	}

	fs((char*)op);
	fs((char*)np);

	if (i < old->n)
		return 1;

	return 0;
}

/* compare old and new -I/-D flags */
/* return 0 if they are equal, non-zero if they are not equal */
void comp_id(old, new, ich, dch)
char* old;
char* new;
int* ich;
int* dch;
{
	char oldi[MAXPATH*4];
	char newi[MAXPATH*4];
	char oldd[MAXPATH*4];
	char newd[MAXPATH*4];
	char buf[MAXPATH];
	char* s;
	char* t;

	if (old == NULL || new == NULL || ich == NULL || dch == NULL)
		fatal("bad argument to comp_id()", (char*)0, (char*)0);

	oldi[0] = 0;
	newi[0] = 0;
	oldd[0] = 0;
	newd[0] = 0;

	/* old lists */

	s = old;
	for (;;) {
		while (*s && *s <= ' ')
			s++;
		t = buf;
		while (*s && *s > ' ')
			*t++ = *s++;
		*t = 0;
		if (t == buf)
			break;
		if (buf[0] == '-' && buf[1] == 'I') {
			strcat(oldi, buf);
			strcat(oldi, " ");
		}
		else if (buf[0] == '-' && buf[1] == 'D') {
			strcat(oldd, buf);
			strcat(oldd, " ");
		}
	}

	/* new lists */

	s = new;
	for (;;) {
		while (*s && *s <= ' ')
			s++;
		t = buf;
		while (*s && *s > ' ')
			*t++ = *s++;
		*t = 0;
		if (t == buf)
			break;
		if (buf[0] == '-' && buf[1] == 'I') {
			strcat(newi, buf);
			strcat(newi, " ");
		}
		else if (buf[0] == '-' && buf[1] == 'D') {
			strcat(newd, buf);
			strcat(newd, " ");
		}
	}

	/* compare */

	if (strcmp(oldi, newi))
		*ich = 1;
	if (strcmp(oldd, newd))
		*dch = 1;
}

/* read the headers from a cache */
/* read_cache(cfn, slp)

   cfn is the name of the cache file (cache files are located in the
      repository directory and their names end in ".he")
   slp is a pointer to the Slist that read_cache() will fill in with
      the names of the header files found in the cache file
 */
void read_cache(cfn, slp)
char* cfn;
Slist* slp;
{
	FILE* fp;
	int first_flag;
	char fbuf[MAXCPP];
	char* s;

	if (cfn == NULL || !*cfn || slp == NULL)
		fatal("bad argument to read_cache()", (char*)0, (char*)0);

	if ((fp = fopen(cfn, "r")) == NULL)
		fatal("cannot open header cache %s for reading", cfn, (char*)0);
	first_flag = 1;
	while (fgets(fbuf, MAXCPP, fp) != NULL) {
		fbuf[strlen(fbuf) - 1] = 0;
		if (first_flag) {
			first_flag = 0;
			continue;
		}
		if (isdigit(fbuf[0])) {
			s = fbuf;
			while (isdigit(*s))
				s++;
			if (!*s)
				break;
		}
		slp_add(slp, fbuf);
	}
	fclose(fp);
}

/* check timestamps */
/*
   The non-CHECKSUM version of check_time() is more complicated.
   It has to read the ".he" file and check the modification time
   of each header file listed in this file against the modification
   of the .o file.

   check_time() returns 1 if the instantiation file doesn't need to
   be recompiled, 0 otherwise.

   check_time() will also rebuild the .he file if it is out of date.

   r is the name of the repository
   bbuf is the basename of the instantiation file
   incl is an Slist containing all of the -I options
   cpp is the name of the C preprocessor
   wbuf is a character buffer for error messages
   main_rep is the name of the main repository directory (which may be
      different from r)
 */
int check_time(r, bbuf, incl, cpp, wbuf, main_rep)
char* r;
char* bbuf;
Slist* incl;
char* cpp;
char* wbuf;
char* main_rep;
{
	int redo_flag;
	int comp_flag;
	char cfn_secondary[MAXPATH]; /* header file cache filename in
					secondary repository */
	char cfn_main[MAXPATH]; /* header file cache filename in
				   main repository */
	char *cfn = &cfn_secondary[0];
	char ifn[MAXPATH];
	char ofn[MAXPATH];
	unsigned long cts;
	unsigned long its;
	unsigned long hts;
	unsigned long ots;
	char fbuf[MAXCPP];
	int first_flag;
	FILE* fp;
	char id[MAXCBUF];
	int i;
	char* s;
	long len;
	int saw_check;
	int id_flag;
	int ich;
	int dch;
	Slist olist;
	Slist nlist;

	if (r == NULL || !*r || bbuf == NULL || !*bbuf || incl == NULL ||
	    cpp == NULL || !*cpp || wbuf == NULL ||
	    main_rep == NULL || !*main_rep)
		fatal("bad argument to check_time()", (char*)0, (char*)0);

	redo_flag = 0;
	comp_flag = 0;
	saw_check = 0;
	wbuf[0] = 0;
	id_flag = 0;
	ich = 0;
	dch = 0;
	olist.n = 0;
	nlist.n = 0;

	/* format I/D */

	id[0] = 0;
	for (i = 0; i < incl->n; i++) {
		strcat(id, incl->ptrs[i]);
		strcat(id, " ");
	}

	/* format cache and instantiation filenames */

	sprintf(cfn, "%s/%s%s", r, bbuf, HE_EXT);
	sprintf(ifn, "%s/%s%s", r, bbuf, SRC_EXT);
	sprintf(ofn, "%s/%s%s", r, bbuf, OBJ_EXT);

	/* check cache existence and date */

	cts = timestamp(cfn);
	its = timestamp(ifn);
	ots = timestamp(ofn);
	if (!its)
		fatal("instantiation file %s missing", ifn, (char*)0);
	if (!ots)
		fatal("object file %s missing", ofn, (char*)0);

	if (!cts || its > cts) {
		strcpy(wbuf, !cts ? ".he file missing" : ".he file out of date");
		redo_flag = 1;
		id_flag = 1;
	}

	/* go through cache */

	else {
		if ((fp = fopen(cfn, "r")) == NULL)
			fatal("cannot open header cache %s for reading", cfn, (char*)0);
		first_flag = 1;
		len = 0;
		while (fgets(fbuf, MAXCPP, fp) != NULL) {
			fbuf[strlen(fbuf) - 1] = 0;

			/* might be check field at end of cache */

			if (isdigit(fbuf[0])) {
				s = fbuf;
				while (isdigit(*s))
					s++;
				if (!*s) {
					saw_check = 1;
					if (len != atoi(fbuf)) {
						redo_flag = 1;
						strcpy(wbuf, ".he of different length");
					}
					break;
				}
			}

			len += strlen(fbuf);

			/* check out -I/-D line */

			if (first_flag) {
				first_flag = 0;
				comp_id(fbuf, id, &ich, &dch);
				if (dch) {
					redo_flag = 1;
					id_flag = 1;
					strcpy(wbuf, "-I/-D changed");
					break;
				}
				else if (ich) {
					redo_flag = 1;
					strcpy(wbuf, "-I/-D changed");
				}
				continue;
			}

			/* check times */

			hts = timestamp(fbuf);
			if (!hts) {
				redo_flag = 1;
				sprintf(wbuf, "header does not exist: %s", fbuf);
				break;
			}
			if (hts >= cts) {
				redo_flag = 1;
				sprintf(wbuf, "header newer than .he file: %s", fbuf);
				break;
			}
			if (hts >= ots) {
				comp_flag = 1;
				sprintf(wbuf, "header changed: %s", fbuf);
			}
		}
		fclose(fp);
	}

	/* if -I changed, grab old list of includes */

	if (ich && !dch)
		read_cache(cfn, &olist);

	/* see what we've got */

	if (redo_flag || !saw_check) {
		/* modify the header file cache in the main repository */
		cfn = &cfn_main[0];
		sprintf(cfn, "%s/%s%s", main_rep, bbuf, HE_EXT);
		
		build_cache(cfn, incl, cpp, id, main_rep, bbuf);
	}
	if (ich && !dch) {
		read_cache(cfn, &nlist);
		if (comp_hdr(&olist, &nlist)) {
			id_flag = 1;
			strcpy(wbuf, "-I/-D changed");
		}
	}
	slp_del(&olist);
	slp_del(&nlist);
	if (!redo_flag && saw_check && !comp_flag && !id_flag)
		return 1;
	if (!redo_flag && saw_check && comp_flag && !id_flag)
		return 0;
	if (id_flag)
		return 0;

	/* go through rebuilt cache */

	if ((fp = fopen(cfn, "r")) == NULL)
		fatal("cannot open header cache %s for reading", cfn, (char*)0);
	first_flag = 1;
	saw_check = 0;
	while (fgets(fbuf, MAXCPP, fp) != NULL) {
		fbuf[strlen(fbuf) - 1] = 0;

		/* might be check field at end of cache */

		if (isdigit(fbuf[0])) {
			s = fbuf;
			while (isdigit(*s))
				s++;
			if (!*s) {
				saw_check = 1;
				if (len != atoi(fbuf)) {
					strcpy(wbuf, ".he of different length");
					redo_flag = 1;
				}
				break;
			}
		}
		len += strlen(fbuf);

		/* check out -I/-D line */

		if (first_flag) {
			first_flag = 0;
			if (strcmp(fbuf, id))
				fatal("mismatched -I/-D in header cache %s", cfn, (char*)0);
			continue;
		}

		/* check times */

		hts = timestamp(fbuf);
		if (!hts)
			fatal("header file %s does not exist", fbuf, (char*)0);
		if (hts >= ots) {
			fclose(fp);
			sprintf(wbuf, "header changed: %s", fbuf);
			return 0;
		}
	}
	fclose(fp);
	if (!saw_check)
		fatal("header cache %s corrupted", cfn, (char*)0);

	return 1;
}

/* check dependencies */
/* dodep(ptr, buf, ccpath, incl, ccopts, obuf, dlp, dip, cs,
	 tflag, wbuf, cpp, tlist, ptoflag)
	
   If there is already a .o file that is up to date in a
   repository, this function will copy the name of that .o file
   to obuf and return 0.  Otherwise, this function will return 1.

   ptr is the name of the main repository
   bbuf is the "basename" that should be looked for in the repository --
     if LONG_NAMES is defined, bbuf will be the name of the template class
     or template function being instantated, if LONG_NAMES is not defined,
     bbuf will be the hashed version of the name
   ccpath is name of the C++ compiler (not used in this function)
   incl is an Slist that contains the -I options specified on the original
     compilation line
   ccopts is an Slist that contains all the other options specified on the
     original compilation line (-D options, for example)
   obuf is a pointer to a buffer that can be filled with the name of a .o file
   dlp is an Slist that contains the names of all of the repositories
     to be searched
   dip is an Slist of the functions that should be instantiated
   cs is the address of the location where dodep() stores the checksum (not
     used in this version of dodep())
   tflag is a boolean flag that indicates if -ptt was used on the command
     line (-ptt is the default for this version of ptlink)
   wbuf is a pointer to a character buffer that dodep() fills with any
     warning messages
   cpp is the name of the C preprocessor command
   tlist is not used
   ptoflag is a boolean flag that indicates if -pto was used on the command
     line
 */

int dodep(ptr, bbuf, ccpath, incl, ccopts, obuf, dlp, dip, cs, tflag, wbuf, cpp, tlist, ptoflag)
char* ptr;
char* bbuf;
char* ccpath;
Slist* incl;
Slist* ccopts;
char* obuf;
Slist* dlp;
Slist* dip;
unsigned long* cs;
int tflag;
char* wbuf;
char* cpp;
Slist* tlist;
int ptoflag;
{
	char fbuf[MAXPATH];
	char fbuf2[MAXPATH];
	char csbuf[MAXLINE];
	FILE* fp;
	int i;
	char* r;
	char buf[MAXLINE];
	Slist cslist;
	int ret;
	Slist nlist;
	int j;
	Fname* fnp;
	int k;
	char id[MAXCBUF];
	char cfn[MAXPATH];

	if (ptr == NULL || !*ptr || bbuf == NULL || !*bbuf ||
	    ccpath == NULL || !*ccpath || incl == NULL ||
	    ccopts == NULL || obuf == NULL || !*obuf ||
	    dlp == NULL || dip == NULL || tflag < 0 || tflag > 1 ||
	    wbuf == NULL || cpp == NULL || !*cpp || tlist == NULL ||
	    ptoflag < 0 || ptoflag > 1)
		fatal("bad argument to dodep()", (char*)0, (char*)0);

	csbuf[0] = 0;
	cslist.n = 0;
	nlist.n = 0;
	*cs = 0;

	/* iterate over repositories */

	for (i = 0; i < dlp->n; i++) {
		r = dlp->ptrs[i];

		/* check for instantiation file */

		sprintf(fbuf2, "%s/%s%s", r, bbuf, SRC_EXT);
		if (!facc(fbuf2)) {
			strcpy(wbuf, "missing instantiation file");
			continue;
		}

		/* check for object */

		sprintf(fbuf, "%s/%s%s", r, bbuf, OBJ_EXT);
		if (!facc(fbuf)) {
			strcpy(wbuf, "missing object");

			/* build header cache */

			id[0] = 0;
			for (k = 0; k < incl->n; k++) {
				strcat(id, incl->ptrs[k]);
				strcat(id, " ");
			}
			/* modify header file cache in the main repository (ptr) */
			sprintf(cfn, "%s/%s%s", ptr, bbuf, HE_EXT);
			build_cache(cfn, incl, cpp, id, ptr, bbuf);
			continue;
		}

		/* check timestamps */

		if (tflag && !check_time(r, bbuf, incl, cpp, wbuf, ptr))
			continue;

		/* maybe -pto; if on list then known to be out of date */

		sprintf(fbuf2, "%s%s", bbuf, OBJ_EXT);
		if (ptoflag && (fnp = flookup(fbuf2, otable)) != NULL) {
			strcpy(wbuf, "-pto indicates out of date");
			fs((char*)fnp->name);		/* can be out of */
			fnp->name = copy(" ");	/* date only once */
			continue;
		}

		/* see if needed members are a subset of */
		/* those stored with checksum */

		sprintf(fbuf, "%s/%s%s", r, bbuf, CS_EXT);
		if ((fp = fopen(fbuf, "r")) == NULL) {
			strcpy(wbuf, "checksum missing or unreadable");
			continue;
		}
		while (fgets(buf, MAXLINE, fp) != NULL) {
			buf[strlen(buf) - 1] = 0;
			slp_add(&cslist, buf);
		}
		fclose(fp);
		if (cslist.n == 1 && !strcmp(cslist.ptrs[0], "@alltc"))
			ret = 1;
		else
			ret = slp_subset(dip, &cslist, &nlist);

		/* might have to handle exclude list for @data */

		if (!ret &&
		    nlist.n == 1 && !strncmp(nlist.ptrs[0], "@data", 5) &&
		    !strncmp(dip->ptrs[0], "@data", 5) &&
		    !strncmp(cslist.ptrs[0], "@data", 5)) {
			char* ptr[2];
			Slist list[2];
			Slist scr;
			char* s;
			char* t;
			ptr[0] = copy(dip->ptrs[0]);
			ptr[1] = copy(cslist.ptrs[0]);
			scr.n = 0;
			for (k = 0; k <= 1; k++) {
				s = ptr[k];
				list[k].n = 0;
				for (;;) {
					t = s;
					while (*s && *s > ' ')
						s++;
					if (*s)
						*s++ = 0;
					if (!*t)
						break;
					slp_add(&list[k], t);
					while (*s && *s <= ' ')
						s++;
				}
			}
			ret = slp_subset(&list[0], &list[1], &scr);
			slp_del(&list[0]);
			slp_del(&list[1]);
			slp_del(&scr);
			fs((char*)ptr[0]);
			fs((char*)ptr[1]);
		}
		slp_del(&cslist);
		if (!ret) {
			strcpy(wbuf, "members missing: ");
			for (j = 0; j < nlist.n; j++) {
				strcat(wbuf, j > 0 ? "," : "");
				strcat(wbuf, nlist.ptrs[j]);
			}
			slp_del(&nlist);
			continue;
		}
		slp_del(&nlist);

		break;
	}

	/* found a matching one */

	if (i < dlp->n) {
		sprintf(obuf, "%s/%s%s", r, bbuf, OBJ_EXT);
		strcpy(wbuf, "OK");
		return 0;
	}

	return 1;
}
#endif

/* compile an instantiation file */
/* docomp(ptr, bbuf, ccpath, incl, ccopts, dip, prevcs, cpp)

   An "instantiation file" is a .c file in the ptrepository directory
   that is generated in the main loop of ptlink.  Compiling the
   instantiation file will generate an object file with either:
	- the definitions of selected member functions of a template class, or
	- the definition of a non-class template function
   Some of the arguments of this function are passed along to fcomp() to
   help build the correct command line.

   ptr is the name of the repository
   bbuf is the basename of the instantiation file
   ccpath is the name of the CC command
   incl is an Slist containing all of the -I options on the original command
   ccopts is an Slist containing the other command line options
   dip is an Slist containing the names of the member functions to instantiate
   prevcs is the previous value of the checksum (used to determine whether
      the compile is necessary, if you are using the CHECKSUM version of
      ptlink) -- this value is normally obtained from the beginning of the
      appropriate ".cs" file in the repository
   cpp is the name of the C preprocessor

   docomp() will write the new checksum in the appropriate ".cs" file (if
   this is the CHECKSUM version of ptfilt), and it will update the list
   of functions in the ".cs" file.
 */
int docomp(ptr, bbuf, ccpath, incl, ccopts, dip, prevcs, cpp)
char* ptr;
char* bbuf;
char* ccpath;
Slist* incl;
Slist* ccopts;
Slist* dip;
unsigned long prevcs;
char* cpp;
{
	char cmd[MAXCBUF];
	int ret;
	char obuf[MAXPATH];
	char ddcbuf[MAXPATH];
	char ptrbuf[MAXPATH];
	char cbuf[MAXCBUF];
	unsigned long cs;
	FILE* fp;
	int i;
	char type[MAXLINE];
	char buf1[MAXPATH];
	char buf2[MAXPATH];
	char buf[MAXPATH];
	int flag;

	if (ptr == NULL || !*ptr || bbuf == NULL || !*bbuf ||
	    ccpath == NULL || !*ccpath || incl == NULL ||
	    ccopts == NULL || dip == NULL ||
	    cpp == NULL || !*cpp)
		fatal("bad argument to docomp()", (char*)0, (char*)0);

	/* format for compiling */

	sprintf(buf, "%s/%s%s", ptr, bbuf, OBJ_EXT);
	del_file(buf);
	if (plusi)
		sprintf(type, "-c +i -ptr%s", ptr);
	else
		sprintf(type, "-c -ptr%s", ptr);
	fcomp(ccpath, incl, ccopts, type, cbuf, dip);
	sprintf(buf1, "%s%s", bbuf, SRC_EXT);
	sprintf(buf2, "%s/%s", ptr, buf1);
	sprintf(cmd, "%s %s", cbuf, buf1);
	progress("CC line is: %s", cmd, (char*)0);

	/* compile */

	sprintf(obuf, "%s%s", bbuf, OBJ_EXT);
	sprintf(ddcbuf, "%s%s", bbuf, DDC_EXT);
	flag = (icmp(ptr, ".") == 0);
	if (flag) {
		pushstring(buf1);
		pushstring(obuf);
		pushstring(ddcbuf);
	}
	signal_block(handlesig);
	if (flag && cp(buf2, buf1) < 0)
		fatal("could not copy file %s --> %s", buf2, buf1);
	ret = fast_system(cmd);
	if (ret) {
		if (flag) {
			signal_unblock();
			popstring();
			popstring();
			popstring();
		}
		return -1;
	}

	/* move the resulting object file to the repository */

	sprintf(ptrbuf, "%s/%s", ptr, obuf);
	if (flag && mv(obuf, ptrbuf, ".", ptr) < 0) {
		popstring();
		popstring();
		popstring();
		fatal("could not move %s --> %s", obuf, ptrbuf);
	}

	/* move the ..c file to the repository if +i */

	if (plusi) {
		sprintf(ptrbuf, "%s/%s", ptr, ddcbuf);
		if (flag && mv(ddcbuf, ptrbuf, ".", ptr) < 0) {
			popstring();
			popstring();
			popstring();
			fatal("could not move %s --> %s", ddcbuf, ptrbuf);
		}
	}
	signal_unblock();
	if (flag) {
		popstring();
		popstring();
		popstring();
	}

	/* write its checksum there */

	sprintf(buf, "%s/%s%s", ptr, bbuf, CS_EXT);
#ifdef CHECKSUM
	if (prevcs)
		cs = prevcs;
	else
		cs = docs(ptr, bbuf, cpp, incl, ccopts, dip);
#endif
	del_file(buf);
	if ((fp = fopen(buf, "w")) == NULL)
		fatal("cannot open checksum file %s for writing", buf, (char*)0);
#ifdef CHECKSUM
	fprintf(fp, "%lx\n", cs);
#endif
	for (i = 0; i < dip->n; i++)
		fprintf(fp, "%s\n", dip->ptrs[i]);
	fclose(fp);

	return 0;
}

/******************** DIRECTED INSTANTIATION LISTS ********************/

typedef struct Dilist Dilist;
struct Dilist {
	char* name;
	Slist list;
	Dilist* next;
};

static Dilist* ditab[DISIZE];

/* look up a list */
Slist* di_lookup(s)
char* s;
{
	int h;
	Dilist* p;

	if (s == NULL || !*s)
		fatal("bad argument to di_lookup()", (char*)0, (char*)0);

	/* look up in list */

	p = ditab[h = (int)hash(s, (long)DISIZE)];
	while (p != NULL && strcmp(s, p->name))
		p = p->next;

	/* create if not found */

	if (p == NULL) {
		p = (Dilist*)gs(sizeof(Dilist));
		p->name = copy(s);
		p->list.n = 0;
		p->next = ditab[h];
		ditab[h] = p;
	}

	return &p->list;
}

/* delete the lists */
void di_del()
{
	int i;
	Dilist* p;
	Dilist* r;

	for (i = 0; i < DISIZE; i++) {
		p = ditab[i];
		while (p != NULL) {
			fs((char*)p->name);
			slp_del(&p->list);
			r = p;
			p = p->next;
			fs((char*)r);
		}
		ditab[i] = NULL;
	}
}

/* get defined data members for a class */
/* get_defdata(rn, out, obuf) -- find all of the symbols in the symbol table
   that are associated with a given class

   rn is a class or template name
   out is a pointer to a character buffer that get_defdata() fills in with
     a list of symbol names -- the symbols are data members or member
     functions in the class
   obuf is the name of the instantiation file's .o file in the repository
     directory.  Symbols from this file will be specifically excluded from
     the list in the output buffer "out".

   get_defdata() is called from the main program, and the output is used
   to update a .cs file in the repository.
 */
void get_defdata(rn, out, obuf)
char* rn;
char* out;
char* obuf;
{
	int i;
	Symbol* sp;
	DEM d;
	char dbuf[2*MAXDBUF];
	Slist list;
	char t;

	if (rn == NULL || !*rn || out == NULL || obuf == NULL || !*obuf)
		fatal("bad argument to get_defdata()", (char*)0, (char*)0);

	list.n = 0;
	slp_add(&list, "@data");
	out[0] = 0;

	/* get the list */

	for (i = 0; i < SYMTSIZE; i++) {
		sp = symtab[i];
		while (sp != NULL) {

			/* find data/virtual members for template class */

			/* only retrieve those members that are defined, */
			/* not in the repository, and members of template */
			/* classes */

			t = sp->type;
			if (t == 'T' || t == 'D' || t == 'B') {
				if (sp->rn == NULL) {
				    if (!dem(sp->name, &d, dbuf) &&
					d.cl != NULL &&
					d.vtname == NULL &&
					d.cl->rname != NULL) {
						sp->rn = copy(d.cl->rname);
						sp->f = copy(d.f);
					}
					else {
						sp->rn = copy(" ");
					}
				}
				if (sp->rn != NULL &&
				    !STRCMP(sp->rn, rn) &&
				    STRCMP(sp->file, obuf) &&
				    (strcmp(basename(sp->file, 0),
					    basename(obuf, 0)) || !sp->rep))
					slp_adduniq(&list, sp->f);
			}
			sp = sp->next;
		}
	}

	/* sort and format */

	slp_sort(&list);
	for (i = 0; i < list.n; i++) {
		if (i > 0)
			strcat(out, " ");
		strcat(out, list.ptrs[i]);
	}
	slp_del(&list);
}

/**********************************************************************/

/* add to list of link objects */
/* add_lnobj(olp, obuf, comp) -- add a .o file to the list of object files in
   the repository directory to add to the link step

   olp is a pointer to an Slist that contains the current list of link objects
     from the repository directory
   obuf is the name of the new link object to add to the list
   comp is either 0 or 1

   If the basename of obuf matches the basename of another object file
   already in the list, then the new link object will replace the old
   one in the list.  Otherwise, the object file will be placed at the
   end of the list.
 */
void add_lnobj(olp, obuf, comp)
Slist* olp;
char* obuf;
int comp;
{
	char obase[MAXPATH];
	char buf[MAXPATH];
	int i;

	if (olp == NULL || obuf == NULL || !*obuf || comp < 0 || comp > 1)
		fatal("bad argument to add_lnobj()", (char*)0, (char*)0);

	strcpy(obase, basename(obuf, 0));
	sprintf(buf, "%s%s", comp ? " " : "", obuf);
	for (i = 0; i < olp->n; i++) {
		if (!strcmp(obase, basename(olp->ptrs[i], 0))) {
			fs((char*)olp->ptrs[i]);
			olp->ptrs[i] = copy(buf);
			return;
		}
	}

	slp_adduniq(olp, buf);
}

/* parse extension list */
/* parse_ext(t, p, list) -- parse a list of filename extensions

   t is a single character (either H for "header" or S for "source")
   p is a string containing a comma or space separated list of extensions --
      each extension should start with a '.' and should be 1 to 3 chars long
   list is a pointer to the start of an array of character pointers -- the
      parse_ext() function will fill in each pointer with a pointer to a
      copy of a suffix.  The array will be terminated with a zero pointer.

   Note:  these values come from the -ptH and -ptS options, which are
   found in the main program.
 */
void parse_ext(t, p, list)
char t;
char* p;
char* list[];
{
	int n;
	char buf[12];
	char* s;

	if ((t != 'H' && t != 'S') || list == NULL)
		fatal("bad argument to parse_ext()", (char*)0, (char*)0);

	n = 0;

	/* loop over string from environment variable */

	for (;;) {
		while (p != NULL && *p && (*p == ',' || *p <= ' '))
			p++;
		if (p == NULL || !*p)
			break;
		s = buf;
		while (*p > ' ' && *p != ',') {
			if (s - buf <= 10)
				*s++ = *p;
			p++;
		}
		*s = 0;

		/* add to list */

		if (buf[0] == '.' && buf[1] && strlen(buf) <= 4) {
			if (n + 1 >= MAXEXT)
				fatal("overflow of extension list", (char*)0, (char*)0);
			list[n++] = copy(buf);
		}
		else {
			fprintf(stderr, "%s warning: ##### illegal extension %s in %s, ignored #####\n", PROGNAME, buf, t == 'H' ? "PTHDR" : "PTSRC"); 
		}
	}

	/* maybe nothing */

	if (!n) {
		list[0] = (t == 'H' ? INC_EXT : SRC_EXT);
		fprintf(stderr, "%s warning: ##### no %s list specified, assuming %s #####\n", PROGNAME, t == 'H' ? "PTHDR" : "PTSRC", t == 'H' ? "INC_EXT" : "SRC_EXT");
	}
}

/* main -- the main program for ptlink has three major phases:
   1. initialization
   2. main loop
   3. cleanup

   In the initialization phase, main calls the following functions:
	+-
	| parse_ext() - to get the legal endings for header files and
	|    source files
	| lock_rep() - to lock the repository
	| tf_get() - to build a new "file table"
	| read_mname() - to get defmap and any auxiliary map files (files that
	|    begin with "nmap" in the repository)
	| - for each object file and library file on the link line, main calls:
        |   +-
	|   | readnm() - read all the symbols from an object file or a library
	|   |    archive into an Obj structure.  readnm() uses nm_split() to
	|   |    parse each line of "nm" command output.
	|   | addfile() - add the filename to the global filename list
	|   | fadd() - add the basename of the file to a list of basenames
	|   +-
	+-
	
   In the main loop, the following functions are called:
	+-
	| tf_del() - clean up the file table
	| tf_read() - read each of the map files
	| symtab_del() - clean up the global symbol table
	| fakelink() - build up the symbol table one file at a time
	|    fakelink() calls okobj(), addobj(), and lookup() as necessary
	| getund() - builds a list of undefined template symbols
	| 
	| - main() makes a list of template classes and functions based
	|    on the list generated by getund()
	| - main() checks this list against a list that was saved from the
	|    last iteration of the loop.  If they are the same, an "infinite
	|    loop" error message is printed and ptlink quits.
	| - main() builds a new instantiation file for one of the items on
	|    the list
	| - if the instantiation file is new, or it is different from
	|    the existing instantiation file for the given template class
	|    or function, main() overwrites the old instantiation file with
	|    the new instantiation file
	|
	| dodep() - check if the instantiation file must be recompiled.
	|    dodep() will call on docs() to compute checksums (in the
	|    CHECKSUM version) and build_cache() to build the header file
	|    cache files (in the non-CHECKSUM version) -- dodep() returns
	|    the name of a .o file to look for or build in a repository
	|    directory (in its "obuf" argument).
	| add_lnobj() - add the name of the.o file that will be generated
	|    for the instantiation file to the list of objects to be linked
	| docomp() [optional] - if dodep() determined that recompilation was
	|    required, docomp() will do it.  docomp() will call on fcomp() to
	|    format the compile command line
	| readnm(), addfile(), and fadd() - create a new Obj record for the
	|    new .o file and put the appropriate entry in the global Flist
	+-
	
   In the cleanup phase, main calls the following functions:
	+-
	| symtab_del() - clean up the global symbol table
	| unlock_rep() - unlock the main repository
	+-
 */
main(argc, argv)
int argc;
char** argv;
{
	Symbol* sp;
	int first;
	char* s;
	Slist nores;
	Slist* noresp;
	Slist tlist;
	char wbuf[MAXLINE*4];
	int tflag;	/* this flag is set if the -ptt option is used */
	char* memname;
	int k;
	int split_flag;	/* this flag is set if the -pts option is used */
	char buf3[MAXPATH];
	Slist dilist2;
	Slist* dip2;
	unsigned long cs;
	int df;
	Slist* dip;
	int all_flag;	/* this flag is set if the -pta option is used */
	int k_flag;	/* this flag is set if the -ptk option is used */
	int k_err;
	int ret;
	Obj* p;
	int i;
	int niter;	/* number of fakelink iterations */
	Slist sc;
	Slist so;
	Slist* scp;	/* scp points to a list of undefined template symbols
			   that are found during fakelink().  This list is
			   initialized by the function getund() */
	Slist* sop;	/* sop points to the list that was pointed to by
			   scp during the last fakelink iteration.  If sip
			   and sop point to identical lists, there is some
			   kind of problem, because no progress was made in
			   the most recent iteration. */
	Slist nl;
	Slist* nlp;	/* nlp points to a list of template class names
			   (mangled) and template function names (mangled)
			   that are about to be instantiated */
	Slist pl;
	Slist* plp;	/* plp points to a list that contains demangled
			   versions of the information in the nlp list */
	Slist rl;
	Slist* rlp;	/* rlp points to a list that contains the mangled
			   name of the object (member function, template
			   function, or static data member) that caused an
			   entry to be made in the nlp and plp lists */
	Slist olist;
	Slist* olp;	/* olp is a list of object files from the repository
			   that are to be included in the link step.
			   This list starts out empty, and a new entry is
			   added for each repository .o file that is found
			   or built.  At the end, this list is printed to
			   the file specified by argv[6]. */
	Slist mlist;
	Slist* mlp;	/* mlp is a list of user-defined map files.  This
			   list is built by doing "ls nmap*" on the repository
			   directory.  These map files override information
			   that is stored in the "defmap" file. */
	char pbuf[MAXLINE];
	char bbuf[MAXPATH];
	char sbuf[MAXPATH];
	char obuf[MAXPATH];
	char dbuf[MAXDBUF];
	char prbuf[MAXLINE];
	DEM d;
	FILE* fp;
	char* ccpath;	/* ccpath is the name of the CC command to run in the
			   instantiation process.  This value is set from
			   argv[3] */
	Slist incl2;
	Slist* incl;	/* incl points to a list of include directories that
			   need to be included with the -I option when
			   instantiating -- the data in this list is read
			   from the file specified by argv[4] */
	char* infile;	/* infile is the name of a file that contains a list
			   of all of the ".o" and ".a" filenames that were
			   on the original link line.  This value is set
			   from argv[5] */
	char* outfile;	/* outfile is the name of a file that is created by
			   this program to dump the "olp" list -- the list
			   of .o files in the repository directory that should
			   be added to the main link.  This value is set
			   from argv[6] */
	char fbuf[MAXPATH];
	FILE* fpin;
	FILE* fpout;
	char* ptr;	/* ptr points to the name of the main repository.
			   This value is set from argv[1] */
	char* defmap;	/* this is the name of the "defmap" file -- the
			   default name mappings and other auxiliary
			   information are contained in this file.  This
			   value is set from argv[7] */
	Slist ccoptslist;
	Slist* ccopts;	/* ccopts points to a list of all of the CC options
			   (mostly -D options) that need to be tacked on for
			   each instantation that is performed -- the data in
			   this list is read from the file specified by
			   argv[8] */
	char* ptrlist;
	Slist dl;
	Slist* dlp;	/* dlp points to a list of the repository directories
			   that have been read from the file specified by
			   argv[9] */
	Tfile** tfp;
	int j;
	Slist ndiplist;
	Slist* ndip;
	char databuf[MAXLINE*4];
	char* fincl;	/* fincl is the name of a file that contains the
			   include directories (-I options) that were on
			   the original CC command line.  This data is used
			   to initialize the list "incl".  This value is
			   set from argv[4] */
	char* cpp;	/* cpp is the name of the C preprocessor.
			   This value is set from argv[10] */
	Slist rawn;
	Slist* rawnp;	/* rawnp points to a list containing the class names
			   (mangled) of the undefined symbols that are in the
			   list pointed to by scp */
	int del_flag;
	int ptoflag;
	char* opts;	/* opts is the name of the file that CC uses to pass
			   the necessary command line arguments (like -D) that
			   will be needed at instantiation time.  The data in
			   this file is needed to initialize the list ccopts.
			   This value is set from argv[8] */
	FILE* fpm;
	char* dout;
	int num_comp;
	int hflag;	/* this flag is set if the -pth option is used */
	char* hdr_list;
	char* src_list;

	start_time = get_time();

	/* check out arguments */

	num_comp = 0;
	all_flag = 0;
	split_flag = 0;
	tflag = 0;
	ptoflag = 0;
	k_flag = 0;
	k_err = 0;
	fpm = NULL;
	dout = NULL;
	hdr_list = NULL;
	src_list = NULL;
#ifdef LONG_NAMES
	hflag = 0;
#else
	hflag = 1;
#endif
	while (argc >= 2 && !strncmp(argv[1], "-pt", 3)) {
		switch (argv[1][3]) {
			case 'H':
				hdr_list = &argv[1][4];
				break;
			case 'S':
				src_list = &argv[1][4];
				break;
			case 'h':
#ifndef LONG_NAMES
				fprintf(stderr, "%s warning: ##### -pth ignored, short filenames the default #####\n", PROGNAME);
#else
				hflag = 1;
#endif
				break;
			case 'd':
				dout = &argv[1][4];
				break;
			case 'm':
				s = &argv[1][4];
				if (!*s || (fpm = fopen(s, "w")) == NULL)
					fprintf(stderr, "%s warning: ##### cannot open -ptm file for writing #####\n", PROGNAME);
				break;
			case 't':
				tflag = 1;
				break;
			case 'v':
				verbose = 1;
				break;
			case 'i':
				plusi = 1;
				break;
			case 'a':
				all_flag = 1;
				break;
			case 'k':
				k_flag = 1;
				break;
			case 's':
				split_flag = 1;
				break;
			case 'o':
				if ((fp = fopen(&argv[1][4], "r")) == NULL)
					fatal("bad file argument to -pto", (char*)0, (char*)0);
				del_flag = 0;
				while (fgets(buf3, MAXPATH, fp) != NULL) {
					buf3[strlen(buf3) - 1] = 0;
					if (!strcmp(buf3, "@delete"))
						del_flag = 1;
					else
						fadd(buf3, otable);
				}
				fclose(fp);
				if (del_flag)
					del_file(&argv[1][4]);
				ptoflag = 1;
				break;
			default:
				break;		/* ignore bad options */
		}
		argc--;
		argv++;
	}
#ifndef CHECKSUM
	if (tflag)
		fprintf(stderr, "%s warning: ##### -ptt option the default #####\n", PROGNAME);
	tflag = 1;
#endif
	if (argc != 11)
		fatal("usage is: [-pta] [-ptdpathname] [-pth] [-ptk] [-ptmpathname] [-ptopathname] [-pts] [-ptv] repository temp_dir CC_path includes input_file output_file defmap CCopts ptrlist cpp_path", (char*)0, (char*)0);
	ptr = argv[1];
	tdir = argv[2];
	ccpath = argv[3];
	fincl = argv[4];
	infile = argv[5];
	outfile = argv[6];
	defmap= argv[7];
	opts = argv[8];
	ptrlist = argv[9];
	cpp = argv[10];
	if (all_flag && split_flag)
		fatal("-pta and -pts cannot both be specified", (char*)0, (char*)0);

	progress2();

	/* set umask */

	set_umask(ptr);

	/* parse list of extensions */

	parse_ext('H', hdr_list, hdr_ext);
	parse_ext('S', src_list, src_ext);

	/* lock repository */

	lock_rep(ptr);
	progress("locked repository", (char*)0, (char*)0);

	/* basic initialization */

	niter = 0;
	scp = &sc;
	rawnp = &rawn;
	sop = &so;
	nlp = &nl;
	plp = &pl;
	rlp = &rl;
	olp = &olist;
	mlp = &mlist;
	dlp = &dl;
	ccopts = &ccoptslist;
	ndip = &ndiplist;
	incl = &incl2;
	noresp = &nores;
	rawnp->n = 0;
	scp->n = 0;
	sop->n = 0;
	nlp->n = 0;
	plp->n = 0;
	rlp->n = 0;
	olp->n = 0;
	mlp->n = 0;
	dlp->n = 0;
	ndip->n = 0;
	incl->n = 0;
	tlist.n = 0;
	noresp->n = 0;
	ccopts->n = 0;
	dilist2.n = 0;

	tfp = tf_get();

	/* read in include list */

	if ((fp = fopen(fincl, "r")) == NULL)
		fatal("cannot open include list %s for reading", fincl, (char*)0);
	while (fgets(buf3, MAXPATH, fp) != NULL) {
		buf3[strlen(buf3) - 1] = 0;
		slp_add(incl, buf3);
	}
	fclose(fp);

	/* read in options */

	if ((fp = fopen(opts, "r")) == NULL)
		fatal("cannot open options list %s for reading", opts, (char*)0);
	while (fgets(buf3, MAXPATH, fp) != NULL) {
		buf3[strlen(buf3) - 1] = 0;
		slp_add(ccopts, buf3);
	}
	fclose(fp);

	/* read in map file names */

	read_mname(ptr, mlp, defmap);
	progress("read name map file list", (char*)0, (char*)0);

	/* read in list of repositories */

	slp_add(dlp, ptr);
	if ((fp = fopen(ptrlist, "r")) == NULL)
		fatal("cannot open repository list %s for reading", ptrlist, (char*)0);
	while (fgets(fbuf, MAXPATH, fp) != NULL) {
		fbuf[strlen(fbuf) - 1] = 0;
		progress("also using repository %s", fbuf, (char*)0);
		slp_add(dlp, fbuf);
	}
	fclose(fp);

	/* read in all objects and archives */

	if ((fpin = fopen(infile, "r")) == NULL)
		fatal("cannot open input file %s for reading", infile, (char*)0);
	while (fgets(fbuf, MAXPATH, fpin) != NULL) {
		fbuf[strlen(fbuf) - 1] = 0;
		progress("searching %s for template symbols", fbuf, (char*)0);
		p = readnm(fbuf);
		addfile(p, fbuf, 0);

		/* add to file list for later type lookup */

		fadd(basename(fbuf, 1), ftable);
	}
	fclose(fpin);
	progress("read in all objects and archives", (char*)0, (char*)0);

loop:
	/* top of main loop - abandon hope all ye who enter here! */

	niter++;

	/* read in map files */

	tf_del(tfp);
	fcnt = 0;
	for (i = 0; i < mlp->n; i++)
		tf_read(mlp->ptrs[i], tfp);

	/* simulate linking */

	symtab_del();
	fakelink(fpm);
	progress("finished link simulation to pick up undefineds", (char*)0, (char*)0);

	/* get undefined PT symbols and bail out if none */

	if (niter & 0x1) {
		scp = &sc;
		sop = &so;
	}
	else {
		scp = &so;
		sop = &sc;
	}
	slp_del(scp);
	slp_del(rawnp);
	slp_del(noresp);
	getund(scp, tfp, dlp, defmap, rawnp);
	if (scp->n == 0)
		goto done;	/* no undefined template symbols were
				   found, so ptlink is finished */
				
	/* maybe in an infinite loop */

	if (niter > 1 && slp_eq(scp, sop, noresp)) {
		if (k_err)
			goto done;
		fprintf(stderr, "%s: unresolvable template symbols are:\n", PROGNAME);
		for (i = 0; i < noresp->n; i++) {
			ret = dem(noresp->ptrs[i], &d, dbuf);
			if (ret < 0)
				strcpy(prbuf, noresp->ptrs[i]);
			else
				dem_print(&d, prbuf);
			fprintf(stderr, "\t%s\n", prbuf);
		}
		fatal("template symbols cannot be resolved", (char*)0, (char*)0);
	}

	/* make list of unique template class or function names */

	slp_del(nlp);
	slp_del(plp);
	slp_del(rlp);
	first = 1;
	for (i = 0; i < scp->n; i++) {
		if (fpm != NULL) {
			sp = lookup(scp->ptrs[i]);
			if (sp != NULL && (sp->type == 'U' || sp->type == 'C')) {
				ret = dem(scp->ptrs[i], &d, dbuf);
				if (ret < 0)
					strcpy(prbuf, scp->ptrs[i]);
				else
					dem_print(&d, prbuf);
				if (first) {
					first = 0;
					fprintf(fpm, "\nUnresolved PT symbols are:\n");
				}
				fprintf(fpm, "\t%s [needed by %s]\n", prbuf, sp->file);
			}
		}
		if (slp_adduniq(nlp, getname(scp->ptrs[i], pbuf, dbuf))) {
			slp_add(plp, pbuf);
			slp_add(rlp, scp->ptrs[i]);
		}
	}
	progress("made list of unique template class names", (char*)0, (char*)0);

	/* write instantiation files and do compile */

	for (i = 0; i < nlp->n; i++) {
		progress2();

		if (verbose) {
			int k2;
			fprintf(stderr, "==========left to do==========\n");
			for (k2 = i; k2 < nlp->n; k2++)
				fprintf(stderr, "\t%s\n", plp->ptrs[k2]);
		}

		progress("now looking at template %s", plp->ptrs[i], (char*)0);

		/* figure out the target list for directed instantiation */

		dip = di_lookup(nlp->ptrs[i]);
		if (dem(rlp->ptrs[i], &d, dbuf) < 0)
			fatal("call to demangler failed for %s", rlp->ptrs[i], (char*)0);
		if (d.cl)
			df = 1;
		else
			df = 2;
		if (df == 2) {

			/* function templates */

			slp_adduniq(dip, "@ft");
		}
		else if (all_flag) {

			/* whole template class */

			slp_adduniq(dip, "@alltc");
		}
		else {

			/* some members of template class */

			slp_del(ndip);
			if (!split_flag)
				strcpy(buf3, nlp->ptrs[i]);
			else
				sprintf(buf3, "%s____data", nlp->ptrs[i]);
			makefnam(ptr, buf3, bbuf, hflag);
			sprintf(obuf, "%s/%s%s", ptr, bbuf, OBJ_EXT);
			get_defdata(nlp->ptrs[i], databuf, obuf);

			/* find needed members */

			for (j = 0; j < scp->n; j++) {
				if (STRCMP(rawnp->ptrs[j], nlp->ptrs[i]))
					continue;
				if (dem(scp->ptrs[j], &d, dbuf) < 0)
					fatal("call to demangler failed for %s", scp->ptrs[j], (char*)0);
				if (d.args != NULL) {
					if (slp_adduniq(dip, d.f))
						slp_adduniq(ndip, d.f);
				}
			}

			/* handle data list - replace @data with new list */

			for (j = 0; j < dip->n; j++) {
				if (!strncmp(dip->ptrs[j], "@data", 5)) {
					fs((char*)dip->ptrs[j]);
					dip->ptrs[j] = copy(databuf);
					break;
				}
			}
			if (j == dip->n)
				slp_add(dip, databuf);
			slp_add(ndip, databuf);
		}
		slp_sort(dip);

		/* handle split case */

		k = 0;
		slp_del(&dilist2);
loop2:

		/* make the name of the instantiation file */

		if (df == 2 || !split_flag) {
			strcpy(buf3, nlp->ptrs[i]);
			memname = "";
		}
		else {
			if (!strncmp(ndip->ptrs[k], "@data", 5))
				memname = "__data";
			else
				memname = ndip->ptrs[k];
			sprintf(buf3, "%s__%s", nlp->ptrs[i], memname);
		}
		makefnam(ptr, buf3, bbuf, hflag);
		if (split_flag && df == 1) {
			dip2 = &dilist2;
			slp_del(dip2);
			slp_add(dip2, ndip->ptrs[k]);
		}
		else {
			slp_del(&dilist2);
			dip2 = dip;
		}

		/* make the file */

		sprintf(sbuf, "%s/%s%s", ptr, bbuf, SRC_EXT);
		sprintf(buf3, "%s/_instfile", ptr);
		del_file(buf3);
		if ((fp = fopen(buf3, "w")) == NULL)
			fatal("cannot open instantiation file %s for writing", sbuf, (char*)0);
		if (*memname)
			fprintf(fp, "/* %s::%s */\n\n", plp->ptrs[i], memname);
		else
			fprintf(fp, "/* %s */\n\n", plp->ptrs[i]);
		slp_del(&tlist);
		doincl(fp, rlp->ptrs[i], tfp, dlp, defmap, incl, &tlist);
		fprintf(fp, "\n");
		doforce(fp, rlp->ptrs[i], plp->ptrs[i]);
		fclose(fp);

		/* only update the file if it has changed */

		if (!facc(sbuf) || !fcmp(buf3, sbuf)) {
			if (mv(buf3, sbuf, ptr, ptr) < 0)
				fatal("could not move file %s --> %s", buf3, sbuf);
		}

		progress("wrote instantiation file %s%s", bbuf, SRC_EXT);

		/* do dependency check */

		sprintf(obuf, "%s/%s%s", ptr, bbuf, OBJ_EXT);
		ret = dodep(ptr, bbuf, ccpath, incl, ccopts, obuf, dlp, dip2, &cs, tflag, wbuf, cpp, &tlist, ptoflag);
		sprintf(buf3, "%s%s", bbuf, SRC_EXT);
		progress("did dependency check on %s (%s)", buf3, wbuf);

		/* add object file to global list */

		add_lnobj(olp, obuf, ret);

		/* recompile if necessary */

		if (ret) {
			num_comp++;
			if (docomp(ptr, bbuf, ccpath, incl, ccopts, dip2, cs, cpp) < 0) {

				if (k_flag) {
					k_err++;
					continue;
				}
				else {
					fatal("compile of instantiation file %s failed", sbuf, (char*)0);
				}
			}
			progress("compiled %s%s", bbuf, SRC_EXT);
		}

		/* add file to global symbol table list */

		p = readnm(obuf);
		addfile(p, obuf, 1);
		fadd(basename(obuf, 1), ftable);
		progress("added symbols from %s%s to symbol table", bbuf, OBJ_EXT);

		/* handle split case */

		if (split_flag && ++k < ndip->n)
			goto loop2;
	}
	progress2();
	goto loop;

done:

	/* cleanup - mainly to track memory leaks better */

	symtab_del();
	di_del();
	ftab_del(ftable);
	ftab_del(otable);
	obj_del();
	tf_del(tfp);
	fs((char*)tfp);
	if (tf_cache != NULL) {
		for (i = 1; i < dlp->n; i++) {
			if (tf_cache[i] != NULL)
				tf_del(tf_cache[i]);
		}
		fs((char*)tf_cache);
		tf_cache = NULL;
	}
	slp_del(&tlist);
	slp_del(&dilist2);
	slp_del(&sc);
	slp_del(&so);
	slp_del(&nl);
	slp_del(&pl);
	slp_del(&rl);
	slp_del(&mlist);
	slp_del(&incl2);
	slp_del(&dl);
	slp_del(&ndiplist);
	slp_del(&rawn);
	slp_del(&nores);
	slp_del(&ccoptslist);
	
	if (fpm != NULL)
		fclose(fpm);

	/* unlock repository */

	unlock_rep(ptr);
	progress("unlocked repository", (char*)0, (char*)0);

	/* all done, pass out results */

	if (k_err) {
		progress2();
		fprintf(stderr, "%s error: instantiation errors (-ptk specified)\n", PROGNAME);
		exit(1);
	}
	if ((fpout = fopen(outfile, "w")) == NULL)
		fatal("cannot open output file %s for writing", outfile, (char*)0);
	for (i = 0; i < olp->n; i++)
		fprintf(fpout, "%s\n", olp->ptrs[i] + (olp->ptrs[i][0] == ' '));
	fclose(fpout);
	if (dout != NULL && (num_comp > 0 || !facc(dout))) {
		if ((fpout = fopen(dout, "w")) == NULL)
			fatal("cannot open -ptd output file %s for writing", dout, (char*)0);
		for (i = 0; i < olp->n; i++)
			fprintf(fpout, "%s\n", olp->ptrs[i]);
		fclose(fpout);
	}
	slp_del(&olist);

	progress2();

	exit(0);	
}
