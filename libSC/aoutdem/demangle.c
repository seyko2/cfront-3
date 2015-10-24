/*ident	"@(#)aoutdem:demangle.c	3.1" */
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

#include "aoutdem.h"
#include <signal.h>
#include <assert.h>
#include <Map.h>

//Mapdeclare(String,String)
//Mapimplement(String,String)

/* globals
*/
int verbose = 0;
int veryVerbose = 0;
int verboseDbx = 0;
int doFunctions = 1;
int doOperators = 1;
int omitAllSigs = 0;
int omitFirstInstanceSigs = 0;
int overwriteAout = 1;
char *qualifierToken = "::";
int debug = 0;
int prepareDebug = 0;

 char *lparQual 	= "_lp_";	// for (
 char *rparQual 	= "_rp_";	// for )
 char *labQual 		= "_la_";	// for <
 char *rabQual 		= "_ra_";	// for >
 char *lsbQual 		= "_ls_";	// for [
 char *rsbQual 		= "_rs_";	// for ]
 char *starQual		= "_st_";	// for *
 char *ampQual 		= "_am_";	// for &
 char *comQual 		= "_cm_";	// for ,
 char *spaQual 		= "_sp_";	// for space
 char *tilda 		= "_dt_";	// for tilda 
 char *plus 		= "_pl_";	// for + 
 char *equal 		= "_eq_";	// for = 
 char *minus 		= "_mi_";	// for - 
 char *percent 		= "_pc_";	// for % 
 char *bn		= "_bn_";	// for ! 
 char *orr		= "_or_";	// for | 
 char *divide		= "_di_";	// for / 
 char *er		= "_er_";	// for ^ 

static FILE
	*sym,		// a.out being demangled
	*newSng,	// temporary file containing the new string table (minus its first four bytes)
	*newSym;	// temporary file containing the new symbol table

static long newSngLoc;  // ftell(newSng) + 4
static char *symb, *newSngb, *newSymb;  // internal buffers for the files
static int bufsize = 6*BUFSIZ;	// default buffer size

static char *obj_n;	// name of a.out being demangled
static char *newSng_n;
static char *newSym_n;

static char *stringTable;  // in-core, of a.out being demangled

static void postProcess(int isDataMember, const String &mangled, String &buildingResult, const char *d);

static inline int min(int i, long j)
{
	return ((long)i < j) ? i : (int)j;
}

FILE *xfopen(char *file, char *mode)
{
	FILE *f;
	if ((f=fopen(file, mode)) == NULL)
	{
		cerr << "demangle: fatal error: can't open temp file " << file << endl;
		perror("demangle");
		exit(-1);
	}
	return f;
}

int xfwrite(const void *buf, int size, int nitems, FILE *f)
{
	int i;
	if ((i=fwrite((char*)buf, size, nitems, f)) < nitems)
	{
		cerr << "demangle: fatal error: write failure" << endl;
		perror("demangle");
		exit(-1);
	}
	return i;
}

int xfread(void *buf, int size, int nitems, FILE *f)
{
	int i;
	if ((i=fread((char*)buf, size, nitems, f)) < nitems)
	{
		cerr << "demangle: fatal error: read failure" << endl;
		perror("demangle");
		exit(-1);
	}
	return i;
}


#if !defined(SYSV) && !defined(V9)
void xsetbuf(FILE *f, char *buf)
{
	setbuffer(f, buf, bufsize);
}
#else
void xsetbuf(FILE *, char *)
{
}
#endif


static void getopts(int argc, char *argv[])
{
	extern char *optarg;
	int errflg = 0;
	int c;
	while ((c=getopt(argc, argv, "b:dfnoq:stvwx")) != EOF)
	{
		switch (c)
		{
			int size;
			case 'b':
				size = atoi(optarg);
				if (size < 1 || ((size << 10) < 1024))
					cerr << "demangle: illegal buffer size (ignored)" << endl;
				else
					bufsize = size << 10;
				break;
			case 'd':
				prepareDebug = 1;
				qualifierToken = "_cc_"; //yes, fall through
			case 't':
				omitFirstInstanceSigs = 1;
				break;
			case 'f':
				doFunctions = 0;
				break;
			case 'n':
				overwriteAout = 0;
				break;
			case 'o':
				doOperators = 0;
				break;
			case 's':
				omitAllSigs = 1;
				break;
			case 'q':
				qualifierToken = optarg;
				break;
			case 'v':
				verbose = 1;
				break;
			case 'w':
				veryVerbose = verbose = 1;
				break;
			case 'x':	
				verboseDbx = 1;
				break;
			case '?':
				errflg++;
				break;
		}
	}
	if (errflg)
	{
		cerr << "usage: demangle [-ndfostvx] [-qqualstring] file ..." << endl;
		exit(2);
	}
}
	
/* overwrite file from onto to, starting at current location of to.
*/
void overwrite(FILE *from, FILE *to)
{
	fseek(from, 0, 0);
	char *c = new char[bufsize];
	int i;
	while ((i=fread(c, sizeof(char), bufsize, from)) > 0)
		xfwrite(c, sizeof(char), i, to);
	delete c;
}

static void overwriteOldTables(const HDR *x)
{
	// write the new symbol table to the original a.out,
	// beginning at the beginning of the original symbol table
	fseek(sym, N_SYMOFF(*x), 0);
	overwrite(newSym, sym);

	// write the new string table to the original a.out,
	// beginning right after the new symbol table
	long len = ftell(newSng) + 4;
	xfwrite(&len, 4, 1, sym);
	overwrite(newSng, sym);

	// rem: truncate, but probably doesn't need it.
}


static void cleanup(int)
{
//cerr << "cleaning up!" << endl;
	signal(SIGINT, (SIG_TYP)SIG_IGN);
	unlink(newSng_n);
	unlink(newSym_n);
	exit(1);
	_exit(1);
}


static void fatalError(char *message)
{
	cerr << "demangle: fatal error: " << obj_n << ": " << message << endl;
	exit(-1);
}


/* returns the new string for n
*/
static String buildNewString(SYM *n)
{
	String ss;
#ifdef SYSV
 	if (n->n_zeroes != 0L)	// it's in n->n_name
	{
		char s[SYMNMLEN+1];
		strncpy(s, n->n_name, SYMNMLEN);
		s[SYMNMLEN] = 0;
		ss = s;
	}
	else 
#endif
	if (N_STRINDEX(*n) > 0) // if it's in string table
	{
#if defined(SUNOS) || defined(BSD)
		ss = parseDbxString(&stringTable[N_STRINDEX(*n)], n->n_type);
			// ss is now demangled and simplified
#else
		ss = &stringTable[N_STRINDEX(*n)];
			// ss is not yet demangled and simplified
#endif		
	}
#ifdef SYSV
	ss = demangleAndSimplify(ss, (n->n_sclass == C_MOS || n->n_sclass == C_MOU), n->n_sclass);
#endif
#ifdef V9
	ss = demangleAndSimplify(ss, (n->n_type == N_SSYM || n->n_type == N_SFLD), n->n_type);
#endif
	return ss;
}


/* process a single symbol table entry.
*  (notice that all non-SYSV strings are in the string table.)
*/
static void transformSymEntry(SYM *n)
{
	String s = buildNewString(n);
#ifdef SYSV
	n->n_zeroes = 0;
#endif
	N_STRINDEX(*n) = 0;				
	if (s.length() > 0)
	{
		const char *t = (const char*)s;
#ifdef SYSV
		if (s.length() <= SYMNMLEN)
			strncpy(n->n_name, t, SYMNMLEN);
		else
#endif
		{
			N_STRINDEX(*n) = newSngLoc;  // fix n's index 
			newSngLoc += xfwrite(t, sizeof(char), s.length()+1, newSng);  // write the new string
		}
	}
}


ostream &operator<<(ostream &oo, const SYM &
#ifdef SYSV
						n
#endif
						)
{
#ifdef SYSV
	char s[SYMNMLEN+1];
	s[SYMNMLEN] = 0;
 	if (n.n_zeroes != 0L)	// it's in n->n_name
	{
		strncpy(s, n.n_name, SYMNMLEN);
		oo << "(" << s << ") ";
	}
	oo << (int)N_AUX(n);
#endif
	return oo;
}


/* build the new symbol and string tables
*/
static void buildNewTables(const HDR *x, long nsyms)
{
	long remaining = nsyms;  // total number of symbol table entries remaining to process
#if FAST_NONPORTABLE_STRUCT_ARRAY_READ
	const int bunch = 1024;  // number of symbol table entries to process at a time	
#else
	const int bunch = 1;
#endif
	char *syms = new char[bunch*SYMSZ];	// buffer of old symbol table entries
	fseek(sym, N_SYMOFF(*x), 0);		// go to old symbol table
						// fill the buffer
	int numInBuffer = xfread(syms, SYMSZ, min(bunch, remaining), sym);  
	int symsi = 0;				// index in buffer
	char *thisSym = syms;			// pointer to entry symsi in buffer
	int auxEntries = 0;			// number of aux entries still to skip over
	while (remaining)
	{
		if (auxEntries)  // skip over them
		{
			if (auxEntries < 0)
				fatalError("Bad symbol table");
			int skip = min(numInBuffer-symsi, auxEntries);
			if (remaining < skip)
				fatalError("Bad symbol table (missing auxiliary entries)");
			auxEntries -= skip;
			remaining -= skip;
			symsi += skip;
			thisSym += skip*SYMSZ;
		}
		else
		{
			transformSymEntry((SYM*)thisSym);
			auxEntries = N_AUX(*(SYM*)thisSym);
			remaining--;
			symsi++;
			thisSym += SYMSZ;
		}
		assert (symsi <= bunch);
		if (symsi == bunch)  // write and then refill the buffer
		{
			xfwrite(syms, SYMSZ, numInBuffer, newSym);
			numInBuffer = xfread(syms, SYMSZ, min(bunch, remaining), sym);
			symsi = 0;
			thisSym = syms;
		}	
	}
	xfwrite(syms, SYMSZ, numInBuffer, newSym);
	delete syms;  
}


static void readStringTable(unsigned long offset)
{
	// Read string table size
	unsigned long ssize;
	fseek(sym, offset, 0);
	xfread(&ssize, sizeof(ssize), 1, sym);
	if (ssize > MAXINT)
		fatalError("String table is too big -- sorry.");

	stringTable = new char[ssize+1];
	fseek(sym, offset, 0);
	xfread(stringTable, sizeof(char), (int)ssize, sym);  
	stringTable[ssize] = NULL;
}

static void demangleAout(const HDR *x, long nsyms)
{
	newSym=xfopen(newSym_n, "w+");
	xsetbuf(newSym, newSymb);
	newSng=xfopen(newSng_n, "w+");
	xsetbuf(newSng, newSngb);
	newSngLoc = 4;

//cerr << "created temps!" << endl;
//sleep(5);

	readStringTable(N_STROFF(*x));

	buildNewTables(x, nsyms);
	if (overwriteAout)
		overwriteOldTables(x);

	delete stringTable;

	fclose(newSym);
	fclose(newSng);
	unlink(newSym_n);
	unlink(newSng_n);
}


static void tryToDemangleAout(char *file)
{
	obj_n = file;
	if ((sym=fopen(obj_n, "r+")) == NULL)
		cerr << "demangle: can't open " << obj_n << endl;
	else 
	{
		xsetbuf(sym, symb);

		HDR x;
		fread((char*)&x, HDRSZ, 1, sym);
#if 1
		if (N_BADMAG(x))
#else  
			// on system v, used to get the magic number in the 
			// auxiliary unix header following the file header
		aouthdr y;
		fread((char*)&y, sizeof(aouthdr), 1, sym);
		if (N_BADMAG(y))
#endif
		{
			cerr << "demangle: " << obj_n << " not an object file (magic is 0" << oct << MAGIC(x) << dec << ")" << endl;
		}
		else 
		{
			long nsyms = N_SYMS(x);
			if (nsyms > 0)  // else it's a stripped object, and there's nothing to do.
				demangleAout(&x, nsyms);
		}
		fclose(sym);
	}
}


main(int argc, char *argv[])
{
	newSng_n = mktemp("sngXXXXXX");
	newSym_n = mktemp("symXXXXXX");

	signal(SIGINT, (SIG_TYP)cleanup);

	getopts(argc, argv);

	symb = new char[bufsize];
	newSymb = new char[bufsize];
	newSngb = new char[bufsize];

	if (optind == argc)
		tryToDemangleAout("a.out");
	else 
	{
		for (; optind < argc; optind++) 
			tryToDemangleAout(argv[optind]);
	}

	delete symb;
	delete newSymb;
	delete newSngb;

	return 0;
}




/* Returns true just if sig is the signature of the first encountered
* instance of the function with selector sel.  That is, future calls with the same 
* sel but a different sig will return false; future calls with the same sel and 
* the same sig will return true.
*/

static Map<String,String> signature;

static int firstInstance(String &sel, const String &sig)
{
//cerr << "firstInstance(" << sel << ", " << sig << ") --";
	if (signature.element(sel))  // if this selector's been seen
	{
		return (signature[sel] == sig);
	}
	else
	{
		signature[sel] = sig;
		return 1;
	}
}	

static void beVerbose(String mangled, String demangled, int entryType, int isDataMember)
{
 	if (verbose)
	{
		cerr << mangled << " => " << demangled;
		if (veryVerbose)
			cerr << " (" << (void*)entryType << ", " << isDataMember << ")";
		cerr << endl;
	}
}

//There are a few char(s) eligible of being part of function name in C++
// but not in C. Replace these char(s) with strings
void translate( String& s )
{
	String buildingResult;
	const char *p = s;
	int len = s.length();

 	for (int i = 0; i < len; i++)
 	{
 		switch(*p) {
 		case ':':
 			assert(p[1] == ':');
 			buildingResult += qualifierToken;
 			p += 2;
			i++;
 			break;
 		case '(':
 			buildingResult += lparQual;
 			p += 1;
 			break;
 		case ')':
 			buildingResult += rparQual;
 			p += 1;
 			break;
 		case '<':
 			buildingResult += labQual;
 			p += 1;
 			break;
 		case '>':
 			buildingResult += rabQual;
 			p += 1;
 			break;
 		case '[':
 			buildingResult += lsbQual;
 			p += 1;
 			break;
 		case ']':
 			buildingResult += rsbQual;
 			p += 1;
 			break;
 		case '*':
 			buildingResult += starQual;
 			p += 1;
 			break;
 		case '&':
 			buildingResult += ampQual;
 			p += 1;
 			break;
 		case ',':
 			buildingResult += comQual;
 			p += 1;
 			break;
 		case ' ':
 			buildingResult += spaQual;
 			p += 1;
 			break;
 		case '~':
 			buildingResult += tilda;
 			p += 1;
 			break;
 		case '+':
 			buildingResult += plus;
 			p += 1;
 			break;
 		case '=':
 			buildingResult += equal;
 			p += 1;
 			break;
 		case '-':
 			buildingResult += minus;
 			p += 1;
 			break;
 		case '%':
 			buildingResult += percent;
 			p += 1;
 			break;
 		case '!':
 			buildingResult += bn;
 			p += 1;
 			break;
 		case '|':
 			buildingResult += orr;
 			p += 1;
 			break;
 		case '/':
 			buildingResult += divide;
 			p += 1;
 			break;
 		case '^':
 			buildingResult += er;
 			p += 1;
 			break;
 		default:
 			buildingResult += *p++;
 			break;
 		}
 	}

	s = buildingResult;
	return;
}

String demangleAndSimplify(const String & mangled, int isDataMember, int entryType)
{
	String buildingResult;
	const char *m = mangled;
	//int mlen = mangled.length();
	String	saveMangled = mangled; // dem may modify the string

	//const char *d = demangle_withlen(m, mlen);
	char	buf[MAXDBUF];
	char	sbuf[MAXDBUF];
	DEM	dm;
	int	putbackUnder = 0;
	
        if (*m == '_' && hasextra_(entryType) && !isDataMember)
        {
		putbackUnder = 1;
        }

	if( dem((char*)m, &dm, sbuf) < 0 || //Error
		 dm.type == DEM_PTBL 	||
		 dm.type == DEM_STI 	||
		 dm.type == DEM_STD )
	{
		buildingResult = saveMangled;
		beVerbose(saveMangled, saveMangled, entryType, isDataMember);
		return buildingResult;
	}

	dem_print( &dm, buf );

	postProcess(isDataMember, saveMangled, buildingResult, buf);

	//postProcess may decide to use the mangled name even if the name is
	// demangleable, for instance, after-first instance of overloading 
	// function name using -d option
	if( ( buildingResult != saveMangled ) && ( putbackUnder == 1 ) )
		buildingResult = "_" + buildingResult;

	// convert the non C function name char in function name to a
	// C string
	if( prepareDebug )
		translate( buildingResult );

	beVerbose(saveMangled, buildingResult, entryType, isDataMember);
	return buildingResult;
}

static int isOperator(const char *d, const char *colon)
{
	if (strncmp(d, "operator", 8) == 0)
	{
		if (!isalnum(d[8]))
			return 1;
	}
	if (colon != 0 && strncmp(colon+1, "operator", 8) == 0)
	{
		if (!isalnum(colon[9]))
			return 1;
	}
	return 0;
}
	
// append the string p to buildingResult, changing qualifier characters along the way
static void appendReplacingQualifierTokens(String & buildingResult, const char *p)
{
	// as an optimization, first append everything up to the first colon
	const char *colon = strchr(p, ':');
	if (colon == 0)
	{
		buildingResult += p;
	}
	else
	{
		buildingResult.append(p, colon - p);
		for (p = colon; *p != '\0'; )
		{
			if (*p == ':') 
			{
				assert(p[1] == ':');
				buildingResult += qualifierToken;
				p += 2;
			}
			else
			{
				buildingResult += *p++;
			}
		}
	}
}

static void appendSelectorAndSignature(String & buildingResult, const char *selector)
{
	// have to replace qualifiers in signature types
	appendReplacingQualifierTokens(buildingResult, selector);
}

static void dropFirstInstanceSignature(
//	const String &mangled, 
	String &buildingResult, 
	const char *d, 
	const char *selector, 
	const char *lp)
{
	// have to do the lookup based on the qualified selector
	String qualifiedSelector(d, lp - d);
	String signature = lp;
	if (firstInstance(qualifiedSelector, signature))
		buildingResult.append(selector, lp - selector);
	//else if (successiveSigsMangled)
	//	buildingResult = mangled;
	else
		appendSelectorAndSignature(buildingResult, selector);
}

static void postProcessFunction(
	//const String &mangled, 
	String &buildingResult, 
	const char *d, 
	const char *selector, 
	const char *lp)
{
	if (omitAllSigs)
		buildingResult.append(selector, lp - selector);
	else if (omitFirstInstanceSigs)
		dropFirstInstanceSignature(buildingResult, d, selector, lp);
	else
		appendSelectorAndSignature(buildingResult, selector);
}

// [d..colon] delimits the *entire* qualifier
static void postProcessQualifier(int isDataMember, String & buildingResult, const char *d, const char *colon)
{
	assert(colon[-1] == ':');
	if (!isDataMember)  // drop qualifier on data members, it's redundant
	{	
		((char*)colon)[-1] = '\0';
		appendReplacingQualifierTokens(buildingResult, d);
		((char*)colon)[-1] = ':';
		buildingResult += qualifierToken;
	}
}

static const char *rightmost_colon_between(const char *d, const char *lp)
{
	if (lp == 0)
	{
		return strrchr(d, ':');
	}
	else
	{
		char rem = *lp;
		*(char*)lp = '\0';
		const char *ret = strrchr(d, ':');
		*(char*)lp = rem;
		return ret;
	}
}

static void postProcess(int isDataMember, const String &mangled, String &buildingResult, const char *d)
{
	const char *lp = strchr(d, '(');
	const char *colon = rightmost_colon_between(d, lp);
	if (lp != 0 && (!doFunctions || (!doOperators && isOperator(d, colon))))
	{
		buildingResult = mangled;
		return;
	}
	if (colon != 0)
	{
		assert(colon[-1] == ':' && colon[1] != '\0');
		postProcessQualifier(isDataMember, buildingResult, d, colon);
	}
	const char *afterQualifier = (colon == 0)? d : colon + 1;
	if (lp != 0)
	{
		postProcessFunction(buildingResult, d, afterQualifier, lp);
	}
	else
	{
		buildingResult += afterQualifier;
	}
}

