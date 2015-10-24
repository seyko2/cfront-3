
/*  Demangle for SVR4/ELF
 *
 *  ELF a.out files are difficult to parse, so this program
 *  simply scans through the sections of the a.out file that
 *  contain debugging string information, looking for C++-style names.
 *  This program substitutes fully demangled names in the
 *  debug  section.
 *
 *  There is a somewhat complex transformation that must be made to
 *  the .debug section of the a.out file.  See the functions reformat()
 *  and fixrefs() below.
 *
 *  This program must be linked with the mangle.a library distributed
 *  with the C++ Translator.
 */

#include <osfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <stream.h>
#include <ctype.h>
#include <memory.h>
#include <string.h>
#include <String.h>
#include <libelf.h>
#undef const
#include <Map.h>
#include <signal.h>
#include <assert.h>

#include <dem.h>
extern "C" {
	int dem(char*, DEM*, char*);
	int dem_print( DEM*, char*);
}

int hasextra_(int);

/* globals for options
*/
int verbose = 0;
int veryVerbose = 0;
int doFunctions = 1;
int doOperators = 1;
int omitAllSigs = 0;
int omitFirstInstanceSigs = 0;
int overwriteAout = 1;
char *qualifierToken = "::";
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

char *tfilename = "/tmp/dqXXXXXX";

#ifndef TRUE
#define FALSE   0
#define TRUE    (!FALSE)
#endif


static int bufsize = 6*BUFSIZ;	// default buffer size

void fixrefs(FILE *, Elf_Data *, int);
static void postProcess(int, const String&, String&, const char *);

extern "C" {
	extern char *demangle( const char *);
}


#ifdef i386
int get4(FILE *f)
{
	int retval;
	retval = getc(f);
	retval += (getc(f)<<8);
	retval += (getc(f)<<16);
	retval += (getc(f)<<24);
	return retval;
}
int get2(FILE *f)
{
	int retval;
	retval = getc(f);
	retval += (getc(f)<<8);
	return retval;
}

/*
 *  getval() converts a series of two or four characters into
 *  an integer.  The "num" argument should be 2 or 4.
 */
int getval(char *p, int num) {
	int j;
	int retval = 0;
	for (j = num - 1; j >= 0; j--) {
		retval = retval << 8;
		retval += (unsigned char) p[j];
	}
	return retval;
}
void putval(int val, char *p, int num) {
	int j;
	for (j = num; j > 0; j--) {
		*p++ = val & 0377;
		val >>= 8;
	}
}
#else
int get4(FILE *f)
{
	int retval;
	retval = (getc(f)<<24);
	retval += (getc(f)<<16);
	retval += (getc(f)<<8);
	retval += getc(f);
	return retval;
}
int get2(FILE *f)
{
	int retval;
	retval = (getc(f)<<8);
	retval += getc(f);
	return retval;
}

/*
 *  getval() converts a series of two or four characters into
 *  an integer.  The "num" argument should be 2 or 4.
 */
int getval(char *p, int num) {
	int j;
	int retval = 0;
	for (j = 0; j < num; j++) {
		retval = retval << 8;
		retval += (unsigned char) p[j];
	}
	return retval;
}
void putval(int val, char *p, int num) {
	int j;
	p += num - 1;
	for (j = num; j > 0; j--) {
		*p-- = val & 0377;
		val >>= 8;
	}
}
#endif

/*
 *  The following values are part of the DWARF debugging format.
 *  See "DWARF Debugging Information Format", a memorandum by the
 *  UNIX International Programming Languages Special Interest
 *  Group.
 */
#define FORM_NONE	 0x0
#define FORM_ADDR	 0x1
#define FORM_REF	 0x2
#define FORM_BLOCK2	 0x3
#define FORM_BLOCK4	 0x4
#define FORM_DATA2	 0x5
#define FORM_DATA4	 0x6
#define FORM_DATA8	 0x7
#define FORM_STRING	 0x8

#define TAG_member	0x000d


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
	

static void cleanup(int)
{
	signal(SIGINT, (SIG_TYP)SIG_IGN);
	unlink(tfilename);
	exit(1);
	_exit(1);
}


/* Returns true just if sig is the signature of the first encountered
* instance of the function with selector sel.  That is, future calls with the same
* sel but a different sig will return false; future calls with the same sel and
* the same sig will return true.
*/

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

/*
 *  demangleAndSimplify takes a mangled name and produces an unmangled name,
 *	depending on user options and whether this section is permitted to grow.
 */

String demangleAndSimplify(const String & mangled, int isDataMember, int entryType)
{
	String	buildingResult;
	char	m[MAXDBUF];
	char	buf[MAXDBUF];
	char	sbuf[MAXDBUF];
	DEM	dm;
	int	putbackUnder = 0;
	
	mangled.dump(m);
        if (*m == '_' && hasextra_(entryType) && !isDataMember)
        {
		putbackUnder = 1;
        }

	if( dem((char*)m, &dm, sbuf) < 0 || //Error
		 dm.type == DEM_PTBL 	||
		 dm.type == DEM_STI 	||
		 dm.type == DEM_STD )
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
	if( ( buildingResult != mangled ) && ( putbackUnder == 1 ) )
		buildingResult = "_" + buildingResult;

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



/*
 *  The reformat(sptr, outfile) function has the following
 *  arguments:
 *
 *    sptr: a Elf_Scn* that is currently pointing to a .debug section
 *    outfile: a FILE* that is currently open for writing on a temporary file
 *
 *  reformat() returns the new length of this .debug section, 0 if it hasn't
 *	changed.
 *
 *  reformat() reads the debugging information entries one-by-one from the
 *  file f and writes them to the file outfile.  In the process, any strings
 *  that are mangled C++ function or variable names are demangled.
 *
 *  As a side effect, a Map called debug_refs is built.  This Map contains
 *  a cross reference from the offsets of individual records in the existing
 *  .debug section and the offsets of the corresponding records that are
 *  being written to the temporary file outfile.
 *
 *  After the completion of this function, the file referred to by outfile
 *  will contain a new .debug section that is almost correct.  The only thing
 *  remaining to be fixed are all of the "reference" attributes -- since the
 *  offsets of the individual records have been changed, the values of most
 *  of the "reference" attributes must be updated.  (This will be done
 *  in the function fixrefs().)
 */
Map<int,int> debug_refs;
int padding_size = 0;

void reformat(Elf_Scn *sptr, FILE *outfile)
{
	int index;
	int newi;
	int curbuflen = bufsize;
	char *recordptr = new char [bufsize + 10];
	Elf_Data *dptr;

	index = 0;
	newi = 0;
	dptr = 0;

	dptr = elf_getdata(sptr, dptr);
	char *p;
	p = (char*)dptr->d_buf;
	
	while (index < dptr->d_size) {
		int length, newlength;
		short tag;
		int isMember;
		int attr;
		char *endp;
		char *nsptr;

		debug_refs[index] = newi;

		length = getval(p, 4);
		p += 4;
		newlength = length;
		if (length > curbuflen) {
			delete [] recordptr;
			recordptr = new char [length + 10];
			curbuflen = length;
		}

		if (length <= 4) {
			fwrite(&length, 4, 1, outfile);
			index += length;
			newi += 4;
			continue;
		}

		/* now parse the rest of the entry */
		index += length;
		endp = p + length - 4;

		if (length < 8) {
			int fillcount, k;
			fillcount = 4 - (newi % 4);
			if (fillcount == 4)
				fillcount = 0;
			newlength = 4 + fillcount;
			newi += newlength;
			fwrite(&newlength, 4, 1, outfile);
			for (k = 0; k < fillcount; k++)
				putc(0, outfile);
			p = endp;
			continue;
		}
		
		tag = getval(p, 2);
		p += 2;

		nsptr = recordptr;
		while (p < endp) {
			int siz, j;
			int input_length;
			String new_string;
			String tmp_string;

			attr = getval(p, 2);
			p += 2;
			putval(attr, nsptr, 2);
			nsptr += 2;

			switch (attr&017) {
			case FORM_NONE:
				break;
			case FORM_DATA2:
				*nsptr++ = *p++;
				*nsptr++ = *p++;
				break;
			case FORM_ADDR:
			case FORM_REF:
			case FORM_DATA4:
				*nsptr++ = *p++;
				*nsptr++ = *p++;
				*nsptr++ = *p++;
				*nsptr++ = *p++;
				break;
			case FORM_DATA8:
				*nsptr++ = *p++;
				*nsptr++ = *p++;
				*nsptr++ = *p++;
				*nsptr++ = *p++;
				*nsptr++ = *p++;
				*nsptr++ = *p++;
				*nsptr++ = *p++;
				*nsptr++ = *p++;
				break;
			case FORM_BLOCK2:
				siz = getval(p, 2);
				p += 2;
				*nsptr++ = *(p-2);
				*nsptr++ = *(p-1);
				for (j = 0; j < siz; j++) {
					*nsptr++ = *p++;
				}
				break;
			case FORM_BLOCK4:
				siz = getval(p, 4);
				p += 4;
				*nsptr++ = *(p-4);
				*nsptr++ = *(p-3);
				*nsptr++ = *(p-2);
				*nsptr++ = *(p-1);
				for (j = 0; j < siz; j++) {
					*nsptr++ = *p++;
				}
				break;
			case FORM_STRING:
				tmp_string = p;
				input_length = strlen(p) + 1;
				p += input_length;

				isMember = (tag == TAG_member);
				new_string = demangleAndSimplify(tmp_string, isMember, (int)tag);

				
				/*
				 *  check to see if we are going to 
				 *  overflow our buffer
				 */
				int len;
				char *step, *old;
				char * nrecordptr;
				len = new_string.length() - input_length +
					curbuflen + 1;
				if (len > curbuflen) { /* YIKES */
					curbuflen = len;
					nrecordptr = new char[len + 10];
					step = nrecordptr;
					old = recordptr;
					while (old < nsptr)
						 *step++ = *old++;
					nsptr=step;
					delete[] recordptr;
					recordptr = nrecordptr;
				}
						
				new_string.dump(nsptr);
				len = strlen(nsptr) + 1;  // new string length
				nsptr += len;
				newlength += len;
				newlength -= input_length;
				break;

			}
		}

		newi += newlength;
		fwrite(&newlength, 4, 1, outfile);
		fwrite(&tag, 2, 1, outfile);
		char *sptr1;
		sptr1 = recordptr;
		while (sptr1 < nsptr)
			 putc(*sptr1++, outfile);

	}
	debug_refs[index] = newi;
	rewind(outfile);
	fixrefs(outfile, dptr, newi);
	delete [] recordptr;
}

/*
 *  fixrefs(f1, dptr, final_length) copies the contents of file f1 to the
 *	 section data buffer
 *
 *    f1 is positioned at the beginning of a temporary file that contains
 *     only the rewritten .debug section.  The "reference" attributes
 *     in this section are the "old" values and must be updated to "new"
 *     values before writing them into the object file.
 *    dptr is a pointer to an Elf_Data structure for the .debug section.
 *    final_length is the new length of the data structure
 *
 * fixref returns the new length of the .debug section, 0 if it hasn't changed.
 *
 *    debug_refs is a Map that contains the mapping of "old" offsets to 
 *     "new" offsets.  This Map was built by the function reformat().
 *
 *  It may be necessary to put padding at the end of the .debug section,
 *  because the new demangled names may be shorter than the original names.
 */
void fixrefs(FILE *f1, Elf_Data *dptr, int final_length) {
	int length;
	int attr;
	int val, val1;
	char *p, *endp;
	int i;
	int curbuflen = bufsize;
	char *bufptr = new char [bufsize];
	char *q;
//	Elf32_Word ret_size = 0;

//fprintf(stderr, "old size %d, new size %d\n", dptr->d_size, final_length);

	if (dptr->d_size < final_length){
		q = new char[final_length];
		dptr->d_size = final_length;
//		ret_size = (Elf32_Word) final_length;
		dptr->d_buf = q;
	}
	else
		q = (char*)dptr->d_buf;

	while (!feof(f1)) {
		/*
		 * First read the length of the next record (4 bytes).
		 * If length > 4, copy the tag field (2 bytes).
		 */
		length = get4(f1);
		if (length < 0) break;
		putval(length, q, 4);
		q += 4;
		if (length <= 4) continue;

		if (length > curbuflen) {
			delete [] bufptr;
			bufptr = new char [length];
			curbuflen = length;
		}
		fread(bufptr, 1, length - 4, f1);

		p = bufptr;
		endp = bufptr + length - 4;

		if (length < 8) {
			while (p < endp)
				*q++ = *p++;
			continue;
		}
		
		/*int tagval =*/ getval(p, 2);

		*q++ = *p++; /* tag is two bytes */
		*q++ = *p++;

		while (p < endp) {
			/*
			 * Read the next attribute entry and determine
			 * how much data follows the attribute.
			 */

			attr = getval(p, 2);
			*q++ = *p++;
			*q++ = *p++;

			switch (attr&017) {
			case FORM_REF:
				/*
				 * If the attribute is a "reference", we have
				 * to consult the Map to get the new reference
				 * value.
				 */
				val = getval(p, 4);
				p += 4;
				if (debug_refs.element(val)) {
					val1 = debug_refs[val];
				}
				else {
					val1 = val;
				}
				putval(val1, q, 4);
				q += 4;
				break;
			case FORM_ADDR:
			case FORM_DATA4:
				*q++ = *p++;
				*q++ = *p++;
				*q++ = *p++;
				*q++ = *p++;
				break;
			case FORM_DATA2:
				*q++ = *p++;
				*q++ = *p++;
				break;
			case FORM_DATA8:
				*q++ = *p++;
				*q++ = *p++;
				*q++ = *p++;
				*q++ = *p++;
				*q++ = *p++;
				*q++ = *p++;
				*q++ = *p++;
				*q++ = *p++;
				break;
			case FORM_BLOCK2:
				val = getval(p, 2);
				p += 2;
				putval(val, q, 2);
				q += 2;
				for (i = 0; i < val; i++) {
					*q++ = *p++;
				}
				break;
			case FORM_BLOCK4:
				val = getval(p, 4);
				p += 4;
				putval(val, q, 4);
				q += 4;
				for (i = 0; i < val; i++) {
					*q++ = *p++;
				}
				break;
			case FORM_STRING:
				while (*p != '\0') {
					*q++ = *p++;
				}
				*q++ = *p++;
				break;
			}
		}
	}

	dptr->d_size = q - (char*)dptr->d_buf;
	elf_flagdata(dptr, ELF_C_SET, ELF_F_DIRTY);
	delete [] bufptr;
}

/*
 *  tryToDemangleAout() opens an ELF format a.out file and examines it using the
 *  functions in the -lelf library.
 *
 *  elf_begin() opens the file and returns a descriptor of type ELF *
 *  elf32get_phdr() gets the process header table of the a.out file.  This
 *	will tell us the loadable sections that we cannot affect during the
 *	course of our demangling of the debug section.
 *
 *  elf_nextscn() goes to the next section of the a.out file and returns
 *   a descriptor of type Elf_Scn *
 *
 *  elf32_getshdr() gets the section header for a specific section.  This
 *   section header contains useful information such as the starting point
 *   of the section in the file and the size of the section.  It also contains
 *   the offset of the name of the section within the section header string
 *   table section.
 *
 *  There is only one section of primary interest for this program, the
 *  debugging information section called ".debug".
 */

static void tryToDemangleAout(char *file)
{
	FILE *f;
	Elf *e;
	Elf32_Phdr *phdrptr;	/* pointer to the process table header */
	Elf32_Phdr *pptr;	/* pointer to the process table header */
	Elf32_Ehdr *ehdrptr;
	Elf_Scn *sptr;
	Elf32_Shdr *shdrptr;
	Elf32_Half e_phnum;	/* number of entries in process table header */
//	Elf32_Off new_offset = 0;
	struct process_tbl{
		Elf32_Off	offset;
		Elf32_Word	end;
	};

	char *nameptr;
	FILE *tfile;
	int past_debug = FALSE;
	int i;

	f = fopen(file, "r+");
	if (f == NULL) {
		fprintf(stderr, "Can't open %s\n", file);
		exit(2);
	}

	signature = empty;

	mktemp(tfilename);

	tfile = fopen(tfilename, "w+");
//fprintf(stderr,"filename=%s\n",tfilename);

	if (tfile == NULL) {
		fprintf(stderr, "Can't create temp file %s\n", tfilename);
		exit(2);
	}

	/*
	 *  This function call must precede the call to elf_begin()
	 */
	if (elf_version(EV_CURRENT) == EV_NONE)
	{
		fprintf(stderr, "library out of date\n");
		exit(2);
	}


	e = elf_begin(fileno(f), ELF_C_RDWR, (Elf *) 0);

	if (e == 0) {
		fprintf(stderr, "%s is not an ELF object file\n", file);
		exit(2);
	}

	ehdrptr = elf32_getehdr(e);
	if (ehdrptr == 0 ){	/* is there a elf header? */
		fprintf(stderr, "%s does not have an ELF header\n", file);
		exit(2);
	}

	phdrptr = elf32_getphdr(e);
	if (phdrptr == 0 ){	/* is there a program header table? */
		fprintf(stderr, "%s is not an ELF executable file\n", file);
		exit(2);
	}
	
	/*
	 * build a table of offsets of the loadable program segments.
	 * We'll use this to tell us if any section following .debug is loadable
	 */
	e_phnum = ehdrptr->e_phnum;

	struct process_tbl *proc_tbl = new struct process_tbl[e_phnum];
	pptr = phdrptr;
	for (i = 0; i < (int) e_phnum; i++, pptr++) {
		proc_tbl[i].offset = pptr->p_offset; 
		proc_tbl[i].end = pptr->p_filesz + pptr->p_offset; 
	}


	sptr = 0;
	while ((sptr = elf_nextscn(e, sptr)) != 0) {
		/*
		 *  Each pass through this loop advances to the
		 *  next section.
		 */
		shdrptr = elf32_getshdr(sptr);
		if ( shdrptr != 0 && !(shdrptr->sh_type & SHT_NOBITS) ) {
			if (past_debug) {
				/*
				 * determine whether names can grow
				 * in the debug section
				 * - can't expand if this section 
				 *   is brought into memory during
				 *   execution.
				 * so remove function sigs if we can't grow!!
				 */

				for (i = 0; i < (int) e_phnum; i++) {
				  if (shdrptr->sh_offset >= proc_tbl[i].offset 
				      && shdrptr->sh_offset <= proc_tbl[i].end){
						
						cerr << "Function signatures will be removed due to file layout" <<endl;
						omitAllSigs = TRUE;
						break;
				  }
				}
			}
			else if (shdrptr->sh_type == SHT_PROGBITS) {
				nameptr = elf_strptr(e, ehdrptr->e_shstrndx,
					(size_t)shdrptr->sh_name);
				if (strcmp(nameptr, ".debug") == 0) {
					/* mark that we found .debug information */
					past_debug=TRUE;
				}
			}
		}
	}

	sptr = 0;
	while ((sptr = elf_nextscn(e, sptr)) != 0) {
		/*
		 *  Each pass through this loop advances to the
		 *  next section.
		 *  Now we know if we can expand the .debug section or not
		 */
		shdrptr = elf32_getshdr(sptr);
		if (shdrptr != 0) {
			if (shdrptr->sh_type == SHT_PROGBITS) {
				nameptr = elf_strptr(e, ehdrptr->e_shstrndx,
					(size_t)shdrptr->sh_name);
				if (strcmp(nameptr, ".debug") == 0) {
					/* modify the .debug information */
					reformat(sptr, tfile);
				}
			}
		}
	}

	if (overwriteAout)
		elf_update(e, ELF_C_WRITE);
	fclose(f);
	fclose(tfile);
	elf_end(e);
	unlink(tfilename);
}


main(int argc, char **argv)
{
	getopts(argc, argv);

	signal(SIGINT, (SIG_TYP)cleanup);

        if (optind == argc)
                tryToDemangleAout("a.out");
        else
        {   
                for (; optind < argc; optind++)
                        tryToDemangleAout(argv[optind]);
        }

	return(0);
}
