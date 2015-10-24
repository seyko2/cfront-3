/*  Demangle for SOLARIS SVR4/ELF
 *
 *  This program must be linked with the mangle.a library distributed
 *  with the C++ Translator.
 */

#include "sym.h"
#include <signal.h>
#include <assert.h>
#include <dem.h>

extern "C" {
	int dem(char*, DEM*, char*);
	int dem_print( DEM*, char*);
	char *demangle(const char *);
}

#if 0
int hasextra_(int);
#endif

// Options
static int verbose = 0;
static int veryVerbose = 0;
static int doFunctions = 1;
static int doOperators = 1;
static int omitAllSigs = 0;
static int omitFirstInstanceSigs = 0;
static char *qualifierToken = "::";
static int prepareDebug = 0;
int overwriteAout = 1;

static char *lparQual 	= "_lp_";  // (
static char *rparQual 	= "_rp_";  // )
static char *labQual 	= "_la_";  // <
static char *rabQual 	= "_ra_";  // >
static char *lsbQual 	= "_ls_";  // [
static char *rsbQual 	= "_rs_";  // ]
static char *starQual	= "_st_";  // *
static char *ampQual 	= "_am_";  // &
static char *comQual 	= "_cm_";  // ,
static char *spaQual 	= "_sp_";  // space
static char *tilda 	= "_dt_";  // ~
static char *plus 	= "_pl_";  // +
static char *equal 	= "_eq_";  // =
static char *minus 	= "_mi_";  // -
static char *percent 	= "_pc_";  // %
static char *bn		= "_bn_";  // !
static char *or		= "_or_";  // |
static char *divide	= "_di_";  // /
static char *er		= "_er_";  // ^

#ifndef TRUE
#define FALSE   0
#define TRUE    (!FALSE)
#endif

#if 0
int bufsize = 6*BUFSIZ;	// default buffer size
#endif

static void postProcess(int, const String&, String&, const char *);

char *tsymfilename = "/tmp/dxXXXXXX";
char *tstrfilename = "/tmp/dyXXXXXX";
DBdeclare(
int readOnly = 0; /* load all sections but don't demangle or overwrite */
)

static void getopts(int argc, char *argv[])
{
	extern char *optarg;
	int errflg = 0;
	int c;
	while ((c=getopt(argc, argv, "b:dD:fnoq:stvwx")) != EOF)
	{
		switch (c)
		{
#if 0
			int size;
			case 'b':
				size = atoi(optarg);
				if (size < 1 || ((size << 10) < 1024))
					cerr << "demangle: illegal buffer size (ignored)" << endl;
				else
					bufsize = size << 10;
				break;
#endif
			case 'D':
#ifdef IDEBUG
				++idebug;
				xx:switch ( *optarg ) {
				case 'F':
				  ++optarg;
				  if ( dbfilename==0 ) {
				    if ((dbfile = fopen(optarg,"w")) == NULL) {
					cerr << "warning: can't open " << optarg << "; using stderr\n" << flush;
					dbfile = stderr;
					dbfilename = 0;
				    } else {
					dbfilename = optarg;
				    }
				  }
				  break;
				case '1':
				  readOnly = 1;
				  overwriteAout = 0;
				  ++optarg;
				  goto xx;
				default:
				  break;
				}
				cerr << form("debug level %d, dbfilename %s%s\n",idebug,dbfilename?dbfilename:"",readOnly?" (readonly)":"");
#else
				cerr << "not compiled for internal debugging\n";
#endif
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
				cerr << "demangle: DBX symbol table request ignored" << endl;
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

// Demangle Routines

// Returns true just if sig is the signature of the first encountered
// instance of the function with selector sel.  That is, future calls
// with the same sel, but a different sig will return false; future calls
// with the same sel and the same sig will return true.

static Map<String,String> signature;
static Map<String,String> empty;

static int firstInstance(String &sel, const String &sig)
{
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

// There are a few char(s) eligible for being part of function name
// in C++, but not in C.  Replace these char(s) with strings.
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
 			buildingResult += or;
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

// demangleAndSimplify takes a mangled name and produces an unmangled name,
//	depending on user options and whether this section is permitted to grow.

String demangleAndSimplify(const String & mangled, int isDataMember, int entryType)
{
	String	buildingResult;
	char	m[MAXDBUF];
	char	buf[MAXDBUF];
	char	sbuf[MAXDBUF];
	DEM	dm;
#if 0
	int	putbackUnder = 0;
#endif
	
	mangled.dump(m);
#if 0
        if (*m == '_' && hasextra_(entryType) && !isDataMember)
        {
		putbackUnder = 1;
        }
#endif

	if( dem((char*)m, &dm, sbuf) < 0 || //Error
		 dm.type == DEM_STI 	||
		 dm.type == DEM_STD	||
		 dm.type == DEM_VTBL 	||
		 dm.type == DEM_PTBL )
	{
		buildingResult = mangled;
		beVerbose(mangled, mangled, entryType, isDataMember);
		return buildingResult;
	}

	dem_print( &dm, buf );

	postProcess(isDataMember, mangled, buildingResult, buf);

	//postProcess may decide to use the mangled name even if the name is
	// demangleable, for instance, after-first instance of overloading
	// function name using -d option
#if 0
	if( ( buildingResult != mangled ) && ( putbackUnder == 1 ) )
		buildingResult = "_" + buildingResult;
#endif

	// convert the non C function name char in function name to a
	// C string
	if( prepareDebug )
		translate( buildingResult );

	beVerbose(mangled, buildingResult, entryType, isDataMember);
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
	
// append string p to buildingResult; change qualifier characters along the way
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
	else
		appendSelectorAndSignature(buildingResult, selector);
}

static void postProcessFunction(
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

// OBJECT FILE MANAGEMENT
static void
tryToDemangleAout(char *file)
{
	signature = empty;
	cerr.flush();
	reformat(file);
	fflush(stderr);
}

static char*
signame(int i)
{
    static char* sigstring[] = {
	"", //0
	"HUP",  //1
	"INT",  //2
	"QUIT", //3
	"ILL",  //4
	"TRAP", //5
	"IOT",  //6
	"EMT",  //7
	"FPE",  //8
	"KILL", //9
	"BUS",  //10
	"SEGV", //11
	"SYS",  //12
	"PIPE", //13
	"ALRM", //14
	"TERM", //15
	"USR1", //16
	"USR2", //17
	"CHLD", //18
	"PWR",  //19
 	"WINCH",//20
	"URG",  //21
	"POLL", //22
	"STOP", //23
	"TSTP", //24
	"CONT", //25
	"TTIN", //26
	"TTOU", //27
	"VTALRM", //28
	"PROF", //29
	"XCPU", //30
	"XFSZ", //31
	"WAITING",//32
	"LWP",  //33
    };
    if ( i >= 0 && i <= 33 )
	return sigstring[i];
    else
	return "???";
}

void bombout(int sig)
{
	signal(SIGINT, (SIG_TYP)SIG_IGN);
	cerr.flush();
	fprintf(stderr,"can't recover: %s %d\n",signame(sig),sig);
	finish(1);
}

void
finish(int e)
{
	unlink(tsymfilename);
	unlink(tstrfilename);
	fflush(stderr);
	cerr.flush();
#ifdef IDEBUG
	if ( dbfilename ) { fflush(dbfile); fclose(dbfile); }
#endif
	exit(e);
}

main(int argc, char **argv)
{
	mktemp(tsymfilename);
	mktemp(tstrfilename);

	signal(SIGINT, (SIG_TYP)bombout);
	signal(SIGILL, (SIG_TYP)bombout);
	signal(SIGEMT, (SIG_TYP)bombout);
	signal(SIGFPE, (SIG_TYP)bombout);
	signal(SIGBUS, (SIG_TYP)bombout);
	signal(SIGSEGV, (SIG_TYP)bombout);

	getopts(argc, argv);

        if (optind == argc)
	{
                tryToDemangleAout("a.out");
	}
        else
        {
                for (; optind < argc; optind++)
                        tryToDemangleAout(argv[optind]);
        }
	DB(if(idebug>=1)fprintf(dbfile,"DONE.\n"););
	finish(0);
}
