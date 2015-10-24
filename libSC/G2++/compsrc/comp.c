/*ident	"@(#)G2++:compsrc/comp.c	3.1" */
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

//  g2++comp - Compile g2++ record definitions
//
//  g2++comp compiles the g2++ record definitions in one or 
//  more files.  The files must have a ".g" suffix in their 
//  name to be considered.  Each ".g" file is compiled into 
//  corresponding ".h" and ".c" files.
//
//  g2++comp is an extension of g2comp, written by
//  Jim Weythman.  

////--------------------------------------------
//#include "compfns.h"
#ifdef __GNUG__
#pragma implementation "Map.h"
#endif
#include <Map.h>
#include <g2comp.h>
#include <g2ctype.h>
#include <g2debug.h>
#include <g2io.h>
#include <g2mach.h>

#include <ctype.h>
#define IOSTREAMH
#ifdef IOSTREAMH
#include <fstream.h>
#else
#include <iostream.h>
#endif
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <iostream.h>
#include <String.h>

//  Global data

extern String	progname;
extern String	filename;
extern int	err;
extern G2NODE*	stack[G2MAXDEPTH_ATTLC];   // should use a block
extern /*long*/time_t 	seed;  // for ranstr()

extern G2BUF*  gbuf; // global so transorm can grow gbuf->buf
extern G2NODE* head;
extern G2NODE* tail;

extern G2NODE nulldef;
extern G2NODE chardef;
extern G2NODE shortdef;
extern G2NODE longdef;
extern G2NODE stringdef;

extern G2BUF char_def; 
extern G2BUF short_def;
extern G2BUF long_def;
extern G2BUF string_def;

struct mtbl{
    G2BUF*	buf;
    mtbl* 	next;
};

extern mtbl*	mtblhead;
extern mtbl*	mtbltail;

#ifdef _MSC_VER
Mapdeclare(String,int)
extern Map(String,udt_info_ATTLC) udt_map_ATTLC;
extern Map(String,int) SoS;
#else
#ifdef __GNUG__
template class Map<String,udt_info_ATTLC>;
template class Map<String,int>;
#endif
extern Map<String,udt_info_ATTLC> udt_map_ATTLC;
extern Map<String,int> SoS;
#endif

//  Local functions

mtbl* addtbl(G2BUF*);
void bdef(G2NODE* t, FILE* hf, FILE* cf);
void blockdef(G2NODE* t, FILE* hf, FILE* cf);
void bolt(const String& stem, FILE* hf, FILE* cf);
int dotg(const char*, const char *);
void errf(int level, const char* msg);
void errf(int level, const char* msg, const char* arg);
void errf(int level, const char* msg, const char* arg1,
    const char* arg2);
void errf(int level, const char* msg, const char* arg1,
    const char* arg2, const char* arg3);
void error(const char* msg);
void error(const char* msg, const char* a);
void error(const char* msg, const char* a, const char* b);
void error(const char* msg, const char* a, long b);
void extract_udt_info(G2NODE* gp);
void generate(G2NODE* t, FILE* hf, FILE* cf);
void gen_io(G2NODE* t, FILE* hf, FILE* cf);
void include_udt(FILE* f);
String lower(const String&);
void nullify(FILE*);
FILE* openfile(const String& filename);
void parse(const String& path, String& dir, String& stem);
String pathname(int level);
#ifdef IOSTREAMH
int perfile(ifstream&, const String& stem, const String& hfilename,
    const String& cfilename);
#else
int perfile(istream&, const String& stem, const String& hfilename,
    const String& cfilename);
#endif
void prologue(FILE* hf, FILE* cf, const String& stem,
    const String& hfilename);
void prtbls(const String& stem, FILE* hf, FILE* cf);
String ranstr(int);
void remtbl();
void resettbl();
void show_udt_info(udt_info_ATTLC x);
void show_udt_map();
void sizealign(FILE* cf);
void transform(G2NODE* p);
int valid(int level, G2NODE* gp);
void valid1(int, G2NODE*);
void wrap(FILE*);
unsigned do_checksum(const char *);
////--------------------------------------------
#ifdef _MSC_VER
Map(String,udt_info_ATTLC) udt_map_ATTLC;
Map(String,int) SoS;
#else
Map<String,udt_info_ATTLC> udt_map_ATTLC;
Map<String,int> SoS;
#endif
mtbl*	mtblhead=0;
mtbl*	mtbltail=0;

String	progname;
String	filename;
int	err;
G2NODE*	stack[G2MAXDEPTH_ATTLC];   // should use a block
/*long*/time_t 	seed;  // for ranstr()

G2BUF*  gbuf; // global so transorm can grow gbuf->buf
G2NODE* head;
G2NODE* tail;

G2NODE nulldef;
G2NODE chardef;
G2NODE shortdef;
G2NODE longdef;
G2NODE stringdef;

G2BUF char_def; 
G2BUF short_def;
G2BUF long_def;
G2BUF string_def;

int no_cap_flag = 0;
int vblock_flag = 0;

main(int argc, char* argv[]){
    String	stem;
    String 	dir;
    int		i; 
//    int 	tflag = 0; 
//    int 	Tflag = 0;
#ifdef IOSTREAMH
//    ifstream 	ins;	      // input fstream
#else
    FILE*	f_file;
#endif
    char       *cur_suffix = ".g";

    progname = argv[0];

    seed = time(0);
//    srand((unsigned)seed);
	    
//  Initialize table of G2NODE 

    resettbl();	  

//  Loop over .g arguments in command line

    for(i = 1; i < argc; i++){
	if(strcmp(argv[i],"-n") == 0) {
	    // the -n option causes g2++comp to refrain from
	    // converting typenames to all uppercase in the
	    // generated code
	    no_cap_flag = 1;
	    continue;
	}
	if(strcmp(argv[i],"-s") == 0) {
	    // the -s option affects the code that is generated
	    // for simple types (G2++ types that are not structures)
	    // Arrays will be derived from Vblock<XXX>, character
	    // strings will be derived from String, and the integer-based
	    // types (long, short, char) will have special class code
	    // generated
	    vblock_flag = 1;
	    continue;
	}
	if(strcmp(argv[i],"-g") == 0) {
	    cur_suffix = "";
	    continue;
	}
	if(strncmp(argv[i],"-g",2) == 0) {
	    cur_suffix = &argv[i][2];
	    continue;
	}
	if(!dotg(argv[i], cur_suffix)){
	    continue;
	}
#ifdef IOSTREAMH
	ifstream ins(argv[i],ios::in);

	if(!ins){
	    error("cannot open '%s': ", argv[i]);
	    perror("");
	    err++;
	    continue;
	}
#else
	if ((f_file = fopen(argv[i],"r")) == NULL) {
	    error("cannot open '%s': ", argv[i]);
	    perror("");
	    err++;
	    continue;
	}
	istream ins(f_file);
#endif
	srand(do_checksum(argv[i]));
	filename = String(argv[i]);  //  might be "a/b/c/d.g"
	DEBUG(cerr << "in main, filename=" << filename << "\n" ;)

//  Display the filename on stderr

	fprintf(stderr, "%s:\n", (const char*)filename);

//  Extract directory part of the filename into 'dir' 
//  and file part (less .g) into 'stem'

	parse(
	    filename,         // might be a/b/c/d.g
	    dir,              // would be a/b/c
	    stem              // would be d
	);
	DEBUG(cerr << "on return from parse:\n" << "    dir=" << dir << "\n"
	   << "    stem=" << stem << "\n" ;)

//  We will put the .h and .c files in the current directory

	String hfilename = /* dir + "/" + */ stem + ".h";
#if defined(_MSC_VER) || defined(__TCPLUSPLUS__)
	String cfilename = /* dir + "/" + */ stem + ".cpp";
#else
	String cfilename = /* dir + "/" + */ stem + ".c";
#endif
	DEBUG(cerr << "    hfilename=" << hfilename << "\n" ;)
	DEBUG(cerr << "    cfilename=" << cfilename << "\n" ;)

//  Do it

	if(perfile(ins,stem,hfilename,cfilename)){

//  Success!  Display the rest of the message to user

#if defined(_MSC_VER) || defined(__TCPLUSPLUS__)
	    fprintf(stderr, " => %s.h, %s.cpp\n", (const char*)stem, (const char*)stem);
#else
	    fprintf(stderr, " => %s.[hc]\n", (const char*)stem);
#endif
	}

//  Prepare to process the next .g file

	ins.close();
	resettbl();
    }
    return err;
}

int 
valid(int level, G2NODE* gp){
    DEBUG(cerr << "enter valid with level=" << level << "\n" ;)
    int	saverr = err;
    valid1(level, gp);
    return err - saverr;
}

void 
valid1(int level, G2NODE* gp){
    stack[level] = gp;
    DEBUG(cerr << "enter valid1 with level = " << level << ", gp = " << "\n" ;)
    DEBUG( showtree_ATTLC(gp,0); )
    String temp=g2name_ATTLC(gp); // elim "sorry not implemented"
    DEBUG( cerr << "temp = " << temp << endl; ) 
    DEBUG(
	cerr << "stack contains:" << endl;
	for(int k = 0;k<=level;k++){
	    cerr << "    " << g2name_ATTLC(stack[k]) << endl;
	}
    )
    if(level<=0 && !isname_ATTLC(temp) ){
	errf(level, " root is not a name\n");
	err++;
	return;
    }
    if(isname_ATTLC(temp)){
	G2NODE* next=g2next_ATTLC(gp);
	if(next){
	    temp=g2name_ATTLC(next);

	    if(!temp.is_empty() && isint_ATTLC(temp)){
		stack[level] = next;
		errf(level, " mixed names and indexes\n");
		err++;
	    }

	}else{

//  Check for variable size String declared at level zero
//  and issue initial allocation warning message

	    temp = g2val_ATTLC(gp);
	    const char* pn = (const char*)temp;
	    int n = atoi(pn);
	    DEBUG( cerr << "max size = " << n << endl; )

	    if(n == 0 && temp.length() >= 4 && temp.char_at(1) == '('){
		n = atoi(pn+2);
	    }
	    DEBUG( cerr << "initial capacity = " << n << endl; )
	    if(n > 0 && level==0){
	      if (vblock_flag == 0) {
		String nn = int_to_str(n);
		String type = upper(
		    g2name_ATTLC(stack[0])
		);
		errf(
		    level, 
		    " warning: %s will not be\nused as the initial string size;for proper preallocation,\nuse a constructor argument, e.g. %s x(Stringsize(%s));\n --> or use the -s option\n",
		    (const char*)nn,
		    (const char*)type,
		    (const char*)nn
		); 
	      }
	    }
	}

    }else if(isdigit_ATTLC(temp.char_at(0))){

//  Array 
//
//  Remember: all arrays should be at level 0,
//  so we should now be at level 1 looking at the
//  size info.

	DEBUG(cerr << "array" << endl; )
	G2NODE* next=g2next_ATTLC(gp);

	if(next){
	    stack[level] = next;

	    if(isint_ATTLC(g2name_ATTLC(next))){
		errf(level, " only one dimension allowed per level\n");
	    }else{
		errf(level, " mixed names and indexes\n");
	    }
	    err++;

	}else{

//  Decode size and initial allocation information

	    const char* pn = (const char*)temp;
	    int n = atoi(pn);
	    DEBUG( cerr << "max size = " << n << endl; )

	    if(n == 0 && temp.length() >= 4 && temp.char_at(1) == '('){
		n = atoi(pn+2);
	    }
	    DEBUG( cerr << "initial capacity = " << n << endl; )
	    if(n > 0){
		String pval = g2val_ATTLC(stack[level-1]);
		DEBUG( cerr << "level, pval = " << level << ", " << pval << endl; )
		if(level==1 && pval != "HOISTED"){
		    DEBUG(cerr << "level==1 && pval!=HOISTED" << endl;)

//  Corrective action: use a constructor argument 

		  if (vblock_flag == 0) {
		    String nn = int_to_str(n);
		    String type = upper(g2name_ATTLC(stack[0]));
		    errf(
			level, 
			" warning: %s will not be\nused as the initial array capacity; for proper preallocation,\nuse a constructor argument, e.g. %s x(%s);\n --> or use the -s option\n",
			(const char*)nn,
			(const char*)type,
			(const char*)nn
		    );
		  }
		}else{

//  level>1 || (level==1 && pval == HOISTED)
//  Corrective action: modify record structure

		    DEBUG( cerr << "level>1 || (level==1 && pval == HOISTED)" << endl; )
#if 0
		    String nn = int_to_str(n);

		    errf(
			level,
			" warning: %s will not be used as the initial array capacity; consider making this array the member of a structure\n",
			(const char*)nn
		    ); 
#endif

		}
	    }

//  Issue warning for array elements declared as n or 0(n)

	    pn = (const char*)gp->val;
	    n = atoi(pn);
	    DEBUG( cerr << "max size = " << n << endl; )

	    if(n == 0 && gp->val.length() >= 4 &&
		gp->val.char_at(1) == '('){
		n = atoi(pn+2);
	    }
	    DEBUG( cerr << "initial capacity = " << n << endl; )
#if 0
	    if(n > 0){
		DEBUG(cerr << "n>0: issue array element warning" << endl;)
		char buf[10];
		sprintf(buf,"%d",n);
		errf(level, " warning: %s will not be used as the initial string capacity; consider redefining the element type as a structure\n",buf);
	    }
#endif
        }

    }else{

	errf(level, " name is neither a name nor an index\n");
	err++;
    }
    temp=g2val_ATTLC(gp);

    if(temp.is_empty() && !g2child_ATTLC(gp)){
	errf(level, " missing value\n");
	err++;
    }
    if(!g2child_ATTLC(gp)){
	temp=g2val_ATTLC(gp);
	DEBUG(cerr << "gp has no children; set temp=" << temp << "\n" ;)
	if(!isdigit_ATTLC(temp.char_at(0))){
	    
//  Cludge -- prevent user from saying "STRING"

	    if(temp=="STRING"){
		errf(level, " type '%s' not defined\n", (const char*)temp);
		err++;
	    }else{
		if(!lookup(temp) ){
		    if(!udt_map_ATTLC.element(temp)){
			errf(level, " type '%s' not defined\n",
			    (const char*)temp);
			err++;
		    }
		}
	    }
	}
    }
    DEBUG( cerr << "ready to call valid1 recursively for children" << endl; )
    for(G2NODE* cp=g2child_ATTLC(gp); cp; cp=g2next_ATTLC(cp)){
	valid1(level+1,cp);
    }
}

void 
errf(int level, const char* msg){
    fprintf(stderr, "%s: ", (const char*)progname);
    if(filename.length()>0){
	fprintf(stderr, "file '%s': ", (const char*)filename);
    }
    fprintf(stderr, "path '%s'", (const char*)pathname(level));
    fprintf(stderr, msg);
}

void 
errf(int level, const char* msg, const char* arg){
    fprintf(stderr, "%s: ", (const char*)progname);
    if(filename.length()>0){
	fprintf(stderr, "file '%s': ", (const char*)filename);
    }
    fprintf(stderr, "path '%s'", (const char*)pathname(level));
    fprintf(stderr, msg, arg);
}

void 
errf(int level, const char*  msg, const char* arg1, const char* arg2){
    fprintf(stderr, "%s: ", (const char*)progname);
    if(filename.length()>0){
	fprintf(stderr, "file '%s': ", (const char*)filename);
    }
    fprintf(stderr, "path '%s'", (const char*)pathname(level));
    fprintf(stderr, msg, arg1, arg2);
}

void 
errf(int level, const char* msg, const char* arg1, const char* arg2,
 const char* arg3){
    fprintf(stderr, "%s: ", (const char*)progname);
    if(filename.length()>0){
	fprintf(stderr, "file '%s': ", (const char*)filename);
    }
    fprintf(stderr, "path '%s'", (const char*)pathname(level));
    fprintf(stderr, msg, arg1, arg2, arg3);
}

String 
pathname(int level){
    String result="";

    for(int i = 0; i <= level; i++){
	String name;

        if(g2val_ATTLC(stack[i]) == "HOISTED"){
	    name = "?";
	}else{
	    name=g2name_ATTLC(stack[i]);
	}
	result+=name;

	if(i<level){
	    result += ".";
	}
    }
    return result;
}

String 
upper(const String& s){
    String result(Stringsize(s.length()));;

    if (no_cap_flag) {
	result = s;
    }
    else {
    for( int i=0;i<s.length();i++ ){

	if( islower(s.char_at(i)) ){
	    result += toupper(s.char_at(i));
	}else{
	    result += s.char_at(i);
	}
    }
    }
    return result;
}

G2NODE* 
lookup(const String& name){
    if(isdigit_ATTLC(name.char_at(0))){  // optimization
	return NULL;
    }
    DEBUG(cerr << "enter lookup with name=" << name << "\n" ;)
    for(mtbl* mp = mtblhead; mp; mp = mp->next){
/*	DEBUG(cerr << "consider buffer at address " 
	    << long(mp) << "\n" ;)
*/
	for(G2NODE* t=g2root_ATTLC(mp->buf); t; t=g2next_ATTLC(t)){
/*	    DEBUG(cerr << "consider G2NODE t:\n";)
	    DEBUG(shownode_ATTLC(t);)
	    DEBUG(cerr << "g2name_ATTLC(t)=" 
		<< g2name_ATTLC(t) << "\n" ;)
*/
	    if(name == g2name_ATTLC(t)){
		DEBUG(cerr << "hit!\n";)
		return t;
	    }
	}
	DEBUG(cerr << "exit from inner forloop\n";)
    }
    DEBUG(cerr << "lookup failed!\n";)
    return NULL;
}

void 
error(const char* msg){
    fprintf(stderr, "%s: ", (const char*)progname);
#ifdef _MSC_VER
    if( filename != "" ){
#else
    if( filename ){
#endif
	fprintf(stderr, "file '%s': ", (const char*)filename);
    }
    fprintf(stderr, msg);
}
void 
error(const char* msg, const char* a){
    fprintf(stderr, "%s: ", (const char*)progname);
#ifdef _MSC_VER
    if( filename != "" ){
#else
    if( filename ){
#endif
	fprintf(stderr, "file '%s': ", (const char*)filename);
    }
    fprintf(stderr, msg, a);
}
void 
error(const char* msg, const char* a, const char* b){
    fprintf(stderr, "%s: ", (const char*)progname);
#ifdef _MSC_VER
    if( filename != "" ){
#else
    if( filename ){
#endif
	fprintf(stderr, "file '%s': ", (const char*)filename);
    }
    fprintf(stderr, msg, a, b);
}
void 
error(const char* msg, const char* a, long b){
    fprintf(stderr, "%s: ", (const char*)progname);
#ifdef _MSC_VER
    if( filename != "" ){
#else
    if( filename ){
#endif
	fprintf(stderr, "file '%s': ", (const char*)filename);
    }
    fprintf(stderr, msg, a, b);
}

FILE* 
openfile(const String& filename){
    DEBUG(cerr << "enter openfile with filename=" << filename << "\n" ;)
    FILE* result=fopen(filename, "w");

    if(result == NULL){
	error("cannot open '%s': ", (const char*)filename);
	perror("");
	err++;
    }
    return result;
}

int 
dotg(const char *name, const char *suffix){
// screen out files w/o '.g' suffix 
    if(suffix == 0 || strcmp(suffix, "") == 0) return 1;
    int suflen = strlen(suffix);
    if(strlen(name) <= suflen || strcmp(name+strlen(name)-suflen,suffix)){
	error("skipping '%s'; doesn't have a '%s' suffix\n", name, suffix);
	err++;
	return 0;
    }
    return 1;
}

void 
parse(

//  Parse directory part of path into 'dir' 
//  and file part (less '.g') into 'stem'

    const String&	path,     // could be a/b/c/d.g
    String& 		dir,      // would be a/b/c
    String& 		stem      // would be d
){
    DEBUG(cerr << "enter parse with path=" << path << "\n" ;)
    int index = path.length()-1;

    while(index>=0){
#if defined(_MSC_VER) || defined(__TCPLUSPLUS__)
	if(path[index]=='/' || path[index]=='\\'){
#else
	if(path[(unsigned) index]=='/'){
#endif
	    break;
	}
	index--;
    }
    DEBUG(cerr << "break from loop with index=" << index << "\n" ;)
    if(index<0){
	dir="";
    }else{
	dir=path.chunk(0,index);
    }
    DEBUG(cerr << "dir=" << dir << "\n" ;)
    stem = path.chunk(
	index+1,               // starting character
	path.length()-index-3  // number of characters
    );
    DEBUG(cerr << "stem=" << stem << "\n" ;)
}
#ifdef _MSC_VER
Mapimplement(String,udt_info_ATTLC)
Mapimplement(String,int)
#endif
////--------------------------------------------
//#include "compfns.h"

void 
prologue(FILE* hf, FILE* cf, const String& stem, const String& hfilename){
    DEBUG(cerr << "enter prologue with stem=" << stem << "\n" << "hfilename=" 
	<< hfilename << "\n" ;)
    fprintf(hf, "#include <String.h>\n");
    fprintf(hf, "#include \"Vblock.h\"\n");
#if defined(_MSC_VER) || defined(__TCPLUSPLUS__)
    fprintf(hf, "#include \"g2.h\"\n\n");
#else
    fprintf(hf, "#include \"g2++.h\"\n\n");
#endif
    fprintf(hf, "class istream;\nclass ostream;\n\n"); 
    fprintf(hf, "typedef int %sTYPE;\n", (const char*)upper(stem));
    fprintf(hf, "typedef long LONG;\n");
    fprintf(hf, "typedef char CHAR;\n");
    fprintf(hf, "typedef short SHORT;\n");
    fprintf(hf, "typedef String STRING;\n");
    fprintf(hf, "\n");
    fflush(hf);  // Debug

//  Write necessary #include's to cf

#ifdef __GNUG__
    fprintf(cf, "#ifdef __GNUG__\n");
    fprintf(cf, "#pragma implementation \"Vblock.h\"\n");
    fprintf(cf, "#pragma implementation \"Block.h\"\n");
    fprintf(cf, "#endif\n");
#endif
    fprintf(cf, "#include \"%s\"\n", (const char*)hfilename);
    fprintf(cf, "#include \"g2desc.h\"\n");
    fprintf(cf, "#include \"g2inline.h\"\n");
    fprintf(cf, "#include \"g2io.h\"\n");
    fprintf(cf, "#include \"g2mach.h\"\n");
#if defined(_MSC_VER) || defined(__TCPLUSPLUS__)
    fprintf(cf, "#include <iostream.h>\n");
#else
    fprintf(cf, "#include <stream.h>\n");
#endif
    fprintf(cf, "\n");
    fflush(cf);  // Debug

//  Generate declaration for array used to hold intermediate 
//  alignment expressions in constructors

    fprintf(cf,"static int align_val[%d];\n",G2MAXDEPTH_ATTLC);
}

int
#ifdef IOSTREAMH
perfile(ifstream& ins, const String& stem, const String& hfilename,
 const String& cfilename){
#else
perfile(istream& ins, const String& stem, const String& hfilename,
 const String& cfilename){
#endif
    FILE* hf;
    FILE* cf;

//  Use untyped I/O to extract information from 
//  g2 record definitions in ifstream 'ins'

//    G2NODE* root;
    G2NODE* t;
    mtbl* mp;
    mtbl* first = 0;
    int OK = 1;

    for(;;){

//  Get a new buffer

	if(!(gbuf=new G2BUF)){
	    error("out of memory\n");
	    exit(1);
	}
	if(!getbuf_ATTLC(gbuf,ins)){
	    DEBUG(cerr << "getbuf_ATTLC returns EOF -- break\n";)
	    break;
	}
	DEBUG(cerr << "in perfile, getbuf_ATTLC returns *gbuf=\n";)
	DEBUG(showbuf_ATTLC(gbuf);)

	if( gbuf->root->val == "USER" ){
	    DEBUG(cerr << "USER type!\n";)

//  This is a USER type: store info in udt_map_ATTLC.
//  
//  Note: I tried to do this in a more uniform
//  fashion (treating user-defined type as an
//  ordinary type definition (linking it into
//  mtbl, etc.), and ran into lots of problems.
//  Now, all information is stored in a separate
//  table, udt_map_ATTLC.

	    extract_udt_info(gbuf->root);

	}else{
	    DEBUG(cerr << "ordinary record type\n";)

//  Transform the type tree rooted at gbuf->root:
//
//      o  Hoist array types to level 0 
//      o  Change string and block definitions
//      o  Generate structure tags for anonymous structures

//  Begin debug:

	    DEBUG(cerr << "before calling transform, ";)
	    DEBUG(cerr << "list of type trees=\n";)
	    DEBUG(
		for(t=gbuf->root; t; t=t->next){
		    showtree_ATTLC(t,0);
		}
	    )

//  End debug

	    head=tail=0;
	    transform(gbuf->root);

//  Debug: display the list of hoisted types

	    G2NODE* ptr=tail;
	    DEBUG(cerr << "tail:\n";)
	    while( ptr ){
		DEBUG(showtree_ATTLC(ptr,1);)
		ptr=ptr->next;
		DEBUG(cerr << "tail->next:\n";)
	    }
	
//  If any array types were hoisted, append the original 
//  type definition to the end of the chain

	    if( tail ){
		DEBUG(cerr << "append the original typedef to the end\n";)
		tail->next = gbuf->root;
		/*root =*/ gbuf->root = head;
	    }

//  Begin debug

	    DEBUG(cerr << "after calling transform, ";)
	    DEBUG(cerr << "list of type trees=\n";)
	    DEBUG(for(t=gbuf->root; t; t=t->next){
		DEBUG(showtree_ATTLC(t,0);)
	    })

//  End debug

//  Link this G2BUF into the mtblhead list
//  Note: this was done previously AFTER checking the
//  types.  Now we must remove the buffer from the mtbl
//  list before deleting it whenever errors are found.

	    DEBUG(cerr << "ready to call addtbl\n";)
	    if(first==0){
		DEBUG(cerr << "first call to addtbl\n";)
		first = addtbl(gbuf);
		DEBUG(cerr << "addtbl returns:\n";)
		DEBUG(showbuf_ATTLC(first->buf);)
	    }else{
		DEBUG(cerr << "subsequent call to addtbl\n";)
		addtbl(gbuf);
	    }
	    DEBUG(cerr << "on return from addtbl, mtblhead-> =\n";)

//  Begin debug

	    DEBUG(for(mp=mtblhead; mp; mp=mp->next){
		DEBUG(cerr << "->\n";)
		DEBUG(showbuf_ATTLC(mp->buf);)
	    })

//  End debug

//  Check the types...

	    DEBUG(cerr << "Check the types\n";)
	    int ok=1;

	    for(t=gbuf->root; t; t=t->next){
//		t->name=lower(t->name);

//  ...for typename clashes...

		DEBUG(cerr << "...for typename clashes...\n";)
		DEBUG(cerr 
		    << "ready to call lookup with t->name = " 
		    << t->name 
		    << "\n"
		;)
		if(lookup(t->name)!=t){
		    error("duplicate record: '%s'\n", (const char*)t->name);
		    err++;
		    ok=0;
		}

//  ...and for well-formedness of the definition

		DEBUG(cerr << "and for well-formedness of the definition\n";)
		DEBUG(cerr << "ready to call valid\n";)
		int temp=valid(0,t);
		DEBUG(cerr << "valid returns " << temp << "\n" ;)
		ok &= (temp==0);
	    }
	    DEBUG(cerr << "exit from check-ok loop with ok=" << ok << "\n" ;)
	    if(!ok){
		DEBUG(cerr << "ready to call remtbl\n";)
		remtbl();

//  Begin debug

		DEBUG(cerr << "after remtbl(), mtblhead-> =\n";)
		DEBUG(for(mp=mtblhead; mp; mp=mp->next){
		    DEBUG(cerr << "->\n";)
		    DEBUG(showbuf_ATTLC(mp->buf);)
		})

//  End debug

		delete gbuf;

	    }
	    OK &= ok;
	}
    }
    DEBUG(cerr << "exit from getbuf_ATTLC-loop with OK = " << OK << "\n";)
    if(!OK){

//  Do not generate code if errors were found

	return 0;

    }else{

//  ***********************************************
//  *                                             *
//  *            BEGIN CODE GENERATION            *
//  *                                             *
//  ***********************************************

//  Debug: display the list of G2BUF's

	DEBUG(cerr << "mtbl list=\n";)
	DEBUG(for(mp = mtblhead; mp; mp = mp->next){
	    cerr << "->\n";
	    showbuf_ATTLC(mp->buf);
	})

//  Open hfilename for output, setting file pointer hf

	if((hf=openfile(hfilename))==NULL){
	    return 0;
	}

//  Open cfilename for output, setting file pointer cf

	if((cf=openfile(cfilename))==NULL){
	    return 0;
	}

//  Write header file guard to protect against multiple 
//  inclusion

	fprintf(hf, "#ifndef %sH\n#define %sH\n\n",
	    (const char*)upper(stem),
	    (const char*)upper(stem));

//  Generate the prologue at beginning of .h and .c files

	prologue(hf,cf,stem,hfilename);

//  For each user-defined type in udt_map_ATTLC, generate
//  the appropriate #include directives.  
//
//  Note: include_udt(), sizealign(), wrap(), and
//  nullify() should be bundled into one routine. 

	DEBUG(cerr << "ready to call include_udt\n";)
	include_udt(hf);

//  For each user-defined type T in udt_map_ATTLC, generate 
//  size and alignment constants (these are used in 
//  size and offset computations --  see desc2)

	DEBUG(cerr << "ready to call sizealign\n";)
	sizealign(cf);

//  For each user-defined type T in udt_map_ATTLC, generate
//  x_put_T and x_get_T wrappers for the client-provided
//  I/O routines.

	DEBUG(cerr << "ready to call wrap\n";)
	wrap(cf);

//  For each user-defined type T in udt_map_ATTLC, generate
//  nullify() and isnull() routines that can be called 
//  through a pointer by g2clear().

	DEBUG(cerr << "ready to call nullify\n";)
	nullify(cf);

//  Generate everything else on a type-by-type basis.

	DEBUG(cerr << "Generate everything else on a type-by-type basis\n";)
	for(mp = first; mp; mp = mp->next){
	    gbuf = mp->buf;

	    for(t=gbuf->root; t; t=t->next){
		DEBUG(cerr << "consider tree:\n";)
		DEBUG(showtree_ATTLC(t,0);)
		DEBUG(cerr << "ready to call generate\n";)
		generate(t,hf,cf);

		if(t->next){
		    DEBUG(cerr << "this is a hoisted type\n";)

//  This is a hoisted type; generate a
//  Vblockdeclare/Vblockimplement pair.

#ifdef __GNUG__
		    blockdef(t,hf,cf);
#endif
		}
	    }   
	}

//  Bolt the .c file and the .h file together

	bolt(stem,hf,cf);

//  Closing endif for file guard

	fprintf(hf, "\n#endif\n");

//  Close the files

	fclose(hf);
	fclose(cf);

//  Return success

	return 1;
    }
}

void 
extract_udt_info(G2NODE* t){
    DEBUG(cerr << "enter extract_udt_info with *t=\n";)
    DEBUG(showtree_ATTLC(t,0);)
    udt_info_ATTLC x;

//  Assume most user-defined types will name five
//  or fewer header files

    x.headers.size(5);

#ifdef _MSC_VER
    if(((void *) udt_map_ATTLC.element(g2name_ATTLC(t))) != 0){
#else
    if(udt_map_ATTLC.element(g2name_ATTLC(t))){
#endif

//  This type has been defined earlier!

	error("duplicate user-defined type %s ignored\n",
	 (const char*)g2name_ATTLC(t));
	
    }else{

//  Set defaults (may be overridden by attributes)

	x.headers[0] = g2name_ATTLC(t) + ".h";
	x.null = g2name_ATTLC(t) + "()";
	x.isnull="";
	x.put="operator<<";
	x.get="operator>>";

//  Get attributes

	int header_count=0;
	int put_count=0;
	int get_count=0;
	int null_count=0;
	int isnull_count=0;

	for(G2NODE* c=g2achild_ATTLC(t); c; c=g2anext_ATTLC(c)){
	    DEBUG(cerr << "consider attribute child:\n";)
	    DEBUG(shownode_ATTLC(c);)
	    if(g2name_ATTLC(c) == ".header"){
		DEBUG(cerr << "header\n";)

		if(header_count==x.headers.size()){
		    x.headers.size(2*x.headers.size());
		}
		if (g2val_ATTLC(c) == "") {
			error("missing file name (or missing tab character) in .header statement for %s\n", g2name_ATTLC(t));
		}
		x.headers[header_count++]=g2val_ATTLC(c);
	    }else if(g2name_ATTLC(c) == ".get"){
		DEBUG(cerr << "get\n";)
		if(++get_count>1){
		    fprintf(stderr,
			"file %s: user-defined type %s: duplicate get attribute\n",
			(const char*)filename,
			(const char*)t->name);
		}else{
		    x.get=g2val_ATTLC(c);
		}
	    }else if(g2name_ATTLC(c) == ".put"){
		DEBUG(cerr << "put\n";)
		if(++put_count>1){
		    fprintf(stderr,
			"file %s: user-defined type %s: duplicate put attribute\n",
			(const char*)filename,
			(const char*)t->name);
		}else{
		    x.put=g2val_ATTLC(c);
		}
	    }else if(g2name_ATTLC(c) == ".null"){
		DEBUG(cerr << "null\n";)
		if(++null_count>1){
		    fprintf(stderr,
			"file %s: user-defined type %s: duplicate null attribute\n",
			(const char*)filename,
			(const char*)t->name);
		}else{
		    x.null=g2val_ATTLC(c);
		}
	    }else if(g2name_ATTLC(c) == ".isnull"){
		DEBUG(cerr << "isnull\n";)
		if(++isnull_count>1){
		    fprintf(stderr,
			"file %s: user-defined type %s: duplicate isnull attribute\n",
			(const char*)filename,
			(const char*)t->name);
		}else{
		    x.isnull=g2val_ATTLC(c);
		}
	    }
	}
	if(header_count==0){
	    header_count=1; // the default;
	}
	x.header_count=header_count;
	DEBUG(cerr << "after for loop, x=\n";)
	DEBUG(show_udt_info(x);)

//  Store x in udt_map_ATTLC

	DEBUG(cerr << "ready to store x in udt_map_ATTLC\n";)
	udt_map_ATTLC[g2name_ATTLC(t)]=x;
	DEBUG(cerr << "ready to return\n";)
    }
}
////--------------------------------------------
//#include "compfns.h"
void 
show_udt_info(udt_info_ATTLC x){
    int nheaders=x.header_count;
    DEBUG(cerr << "nheaders=" << nheaders << "\n" ;)
    for(int k=0; k < nheaders; k++){
	DEBUG(cerr << x.headers[k] << "\n";)
    }
    DEBUG(cerr << ".put=" << x.put << "\n" << ".get=" << x.get
	<< "\n" << ".null=" << x.null << "\n" << ".isnull=" << x.isnull
	<< "\n" ;)
}

void 
show_udt_map(){
#ifdef _MSC_VER
    Mapiter(String,udt_info_ATTLC) i(udt_map_ATTLC);
#else
    Mapiter<String,udt_info_ATTLC> i(udt_map_ATTLC);
#endif

    DEBUG(cerr << "udt_map_ATTLC:\n";)
    while(++i){
	DEBUG(cerr << "    " << i.key() << "\t" << "(" ;)
	DEBUG(udt_info_ATTLC x = i.value();)
	DEBUG(cerr << ",";)
	DEBUG(show_udt_info(x);)
	DEBUG(cerr << ")";)
    }
}

void 
sizealign(FILE* cf){

//  Generate size and alignment constants for
//  each user-defined type in udt_map_ATTLC

#ifdef _MSC_VER
    Mapiter(String,udt_info_ATTLC) i(udt_map_ATTLC);
#else
    Mapiter<String,udt_info_ATTLC> i(udt_map_ATTLC);
#endif

    while(++i){
	fprintf(cf, 
	    "static struct x%s{\n    char base[1];\n    %s x;\n}x%s;\n\n",
	    (const char*)i.key(), (const char*)i.key(), (const char*)i.key());
	fprintf(cf,
	    "static const int %s_SIZE=sizeof(%s);\n",
	    (const char*)i.key(), (const char*)i.key());
	fprintf(cf,
	    "static const char* %s_ALIGN_SIMPLIFY_EXPRESSION=(char*)(&x%s.x);\n",
	    (const char*)i.key(), (const char*)i.key());
//	fprintf(cf,
//	    "static const int %s_ALIGN=%s_ALIGN_SIMPLIFY_EXPRESSION-x%s.base;\n\n",
//	    (const char*)i.key(), (const char*)i.key(), (const char*)i.key());
	fprintf(cf,
	    "static int get_%s_ALIGN() {\n return(%s_ALIGN_SIMPLIFY_EXPRESSION-x%s.base); }\n\n",
	    (const char*)i.key(), (const char*)i.key(), (const char*)i.key());
    }
}

void 
include_udt(FILE* hf){
#ifdef _MSC_VER
    Mapiter(String,udt_info_ATTLC) i(udt_map_ATTLC);
#else
    Mapiter<String,udt_info_ATTLC> i(udt_map_ATTLC);
#endif
    DEBUG(cerr << "enter include_udt\n";)

    while(++i){
	udt_info_ATTLC x=i.value();
	DEBUG(cerr << "in while loop, x=\n";)
	DEBUG(show_udt_info(x);)

	for(int j=0;j<x.header_count;j++){
	    DEBUG(cerr << "in j loop, consider header " << x.headers[j]
		<< "\n" ;)
	    fprintf(hf, "#include \"%s\"\n", (const char*)x.headers[j]);
	}
    }
    DEBUG(cerr << "ready to return from include_udt\n";)
}

void
wrap(FILE* cf){
#ifdef _MSC_VER
    Mapiter(String,udt_info_ATTLC) i(udt_map_ATTLC);
#else
    Mapiter<String,udt_info_ATTLC> i(udt_map_ATTLC);
#endif
    DEBUG(cerr << "enter wrap\n";)

    while(++i){
	String name=i.key();
	DEBUG(cerr << "in while loop, i.key()=" << name << "\n" ;)
	udt_info_ATTLC x=i.value();
	DEBUG(cerr << "...and i.value()=\n";)
	DEBUG(show_udt_info(x);)

	if(x.put == "operator<<"){
	    fprintf(cf,
		"static ostream& x_put_%s(ostream& os,const void* vp){\n",
		(const char*)name);
	    fprintf(cf, "    return os << *(%s*)vp;\n}\n",
		(const char*)name);
	}else{
	    fprintf(cf,
		"static ostream& x_put_%s(ostream& os,const void* vp){\n",
		(const char*)name);
	    fprintf(cf, "    return %s(os,*(%s*)vp);\n}\n",
		(const char*)x.put,
		(const char*)name);
	}
	if(x.get == "operator>>"){
	    fprintf(cf,
		"static istream& x_get_%s(istream& is,void* vp){\n",
		(const char*)name);
	    fprintf(cf, "    return is >> *(%s*)vp;\n}\n",
		(const char*)name);
	}else{
	    fprintf(cf,
		"static istream& x_get_%s(istream& is,void* vp){\n",
		(const char*)name);
	    fprintf(cf, "    return %s(is,*(%s*)vp);\n}\n",
		(const char*)x.get,
		(const char*)name);
	}
    }

}

void
nullify(FILE* cf){
#ifdef _MSC_VER
    Mapiter(String,udt_info_ATTLC) i(udt_map_ATTLC);
#else
    Mapiter<String,udt_info_ATTLC> i(udt_map_ATTLC);
#endif
    DEBUG(cerr << "enter nullify\n";)

    while(++i){
	String name=i.key();
	udt_info_ATTLC x=i.value();
//	fprintf(cf, "static %s null_%s=%s;\n",
//	    (const char*)name,
//	    (const char*)name,
//	    (const char*)x.null);
	if (x.null != "") {
	fprintf(cf, "static %s& null_%s(){\n static %s nval = %s; return nval;}\n",
	    (const char*)name,
	    (const char*)name,
	    (const char*)name,
	    (const char*)x.null);
	}
	else {
	fprintf(cf, "static %s& null_%s(){\n static %s nval; return nval;}\n",
	    (const char*)name,
	    (const char*)name,
	    (const char*)name);
	}
	fprintf(cf,
//	    "static void %s_nullify(void* vp){\n    *(%s*)vp=null_%s;\n}\n",
	    "static void %s_nullify(void* vp){\n    *(%s*)vp=null_%s();\n}\n",
	    (const char*)name,
	    (const char*)name,
	    (const char*)name);
	String result;

	if( x.isnull.is_empty() ){
//	    result = "*(" + name + "*)vp==null_" + name;
	    result = "*(" + name + "*)vp==null_" + name + "()";
	}else{
	    result = x.isnull + "(*(" + name + "*)vp)";
	}
	fprintf(cf,
	    "static int %s_is_null(void* vp){\n    return %s;\n}\n",
	    (const char*)name,
	    (const char*)result);
    }
}

void 
bdef(G2NODE* t, FILE* hf, FILE* cf){ 
    DEBUG(cerr << "enter bdef with t=\n";)
    DEBUG(showtree_ATTLC(t,1);)

//  Visit t's children

    for(G2NODE* cp=t->child; cp; cp=cp->next){ 
	DEBUG(cerr << "ready to visit t's next child\n";)
	bdef(cp,hf,cf); 
    } 

//  Then visit t

    DEBUG(cerr << "exit from children loop; now visit t itself\n";)
    DEBUG(showtree_ATTLC(t,1);)
    if(isdigit_ATTLC(t->name.char_at(0))){ 

//  t is an array (note: all arrays have
//  been transformed into terminal arrays)

	DEBUG( cerr << "t->name is a digit: t is an array\n"; )
	String a_typename;

	if(isdigit_ATTLC(t->val.char_at(0))){
	    a_typename = "STRING";
	}else{
	    a_typename = upper(t->val);
	}
	DEBUG(cerr << "t's typename is " << a_typename << "\n" ;)

#ifdef _MSC_VER
//  Invoke vblockdeclare macro

	DEBUG(cerr << "put to .h file:\n    ***Vblockdeclare(" 
	    << a_typename << ");\n" ;)
	fprintf(hf, "Vblockdeclare(%s);\n", (const char*)a_typename);
	fflush(hf);  // Debug

//  Invoke blockimplement macro

	DEBUG(cerr << "put to .c file:\n    ***Vblockimplement("
	    << a_typename << ");\n" ;)
	fprintf(cf, "Vblockimplement(%s);\n", (const char*)a_typename);
	fflush(cf);  // Debug
#endif
    }
}

void 
#ifdef __GNUG__
blockdef(G2NODE* t, FILE* hf, FILE* cf){
    DEBUG(cerr << "put to .c file:\n    ***#ifdef __GNUG__\n";)
    DEBUG(cerr << "put to .c file:\n    ***template class Vblock<"
	<< upper(t->name) << ">;\n" ;)
    DEBUG(cerr << "put to .c file:\n    ***#endif\n";)
    fprintf(cf, "#ifdef __GNUG__\n");
    fprintf(cf, "template class Vblock<%s>;\n", (const char*)upper(t->name));
    fprintf(cf, "#endif\n");
    fflush(hf);  // Debug
#else
#ifndef _MSC_VER
blockdef(G2NODE*, FILE*, FILE*){
#else
blockdef(G2NODE* t, FILE* hf, FILE* cf){
    DEBUG(cerr << "put to .h file:\n    ***Vblockdeclare(" 
	<< upper(t->name) << ");\n" ;)
    fprintf(hf, "Vblockdeclare(%s);\n", (const char*)upper(t->name));
    fflush(hf);  // Debug

    DEBUG(cerr << "put to .c file:\n    ***Vblockimplement("
	<< upper(t->name) << ");\n" ;)
    fprintf(cf, "Vblockimplement(%s);\n", (const char*)upper(t->name));
    fflush(cf);  // Debug
#endif
#endif
}

void 
generate(G2NODE* t, FILE* hf, FILE* cf){

//  Generate typedefs in .h file
//  (.c file is needed for constructor definitions, which
//  used to be inline in the .h file, but which gave "outline
//  inline" warning messages from CC +w 

    DEBUG(cerr << "ready to call tdef\n";)
    bdef(t,hf,cf);
    tdef(t,hf,cf);

//  Generate descriptor tables in .c file

    DEBUG(cerr << "ready to call desc\n";)
    desc(t,t->name,cf);

//  Generate stream insertion and extraction operators

    DEBUG(cerr << "ready to call gen_io\n";)
    gen_io(t,hf,cf);
}

void 
gen_io(G2NODE* t, FILE* hf, FILE* cf){

//  Generate stream insertion and extraction
//  function declarations in .h file

    fprintf(hf, "\nostream& operator<<(ostream& os, const %s& buf);\n",
	(const char*)(upper(t->name)));
    fprintf(hf, "istream& operator>>(istream& is, %s& buf);\n\n",
	(const char*)(upper(t->name)));
    fflush(hf);  // Debug

//  Generate function definitions in .c file

    fprintf(cf, "\nostream& operator<<(ostream& os, const %s& buf){\n",
        (const char*)(upper(t->name)));
    fprintf(cf,
//        "    if(!os)return os;\n    putrec_ATTLC((void*)&buf,%s,os);\n",
        "    if(!os)return os;\n    putrec_ATTLC((void*)&buf,%s_,os);\n",
        (const char*)t->name);
    fprintf(cf, "    return os;\n}\n");

    fprintf(cf, "\nistream& operator>>(istream& is, %s& buf){\n",
	(const char*)(upper(t->name)));
//    fprintf(cf, "    if(!is)return is;\n    getrec_ATTLC(&buf,%s,is);\n",
    fprintf(cf, "    if(!is)return is;\n    getrec_ATTLC(&buf,%s_,is);\n",
	(const char*)t->name);
    fprintf(cf, "    return is;\n}\n");
    fflush(cf);  // Debug
}

void 
transform(G2NODE* p){
//    static int count=0;
    DEBUG(cerr << "enter transform with *p=\n";)
    DEBUG(showtree_ATTLC(p,0);)
    G2NODE* c=p->child;

    if(c){

//  Generate a type tag
//
//  Note: we just ignore the case where p->val is nonempty
//
//      assert(p->val=="");

	String rand=ranstr(G2TAGLEN_ATTLC);
	SoS[rand] = 1;	// record randomly generated class names
	p->val=rand;
	DEBUG(cerr << "after generating tag, *p=\n";)
	DEBUG(showtree_ATTLC(p,0);)

	char temp = c->name.char_at(0);
  	int isarray=(
	    temp=='*' ||                // flexible
	    isdigit_ATTLC(temp)         // fixed
	);
	if(isarray){
	    DEBUG(cerr << "p is an array\n";)
	}else{
	    DEBUG(cerr << "p is a structure\n";)
	}

//  Visit p's children

	DEBUG(cerr << "ready to visit p's children\n";)
	for(G2NODE* cp=c; cp; cp=cp->next){
	    DEBUG(cerr << "visit next child\n";)
	    transform(cp);
	}
	DEBUG(cerr << "after transforming, p=\n";)
	DEBUG(showtree_ATTLC(p,0);)

	if(isarray){

//  Change arrays dimensioned using "*" to "0"
//
//  Note: only modify the first character, preserving
//  any initial-reserve specification that may follow the 
//  asterisk, e.g.: 
//
//      usr    
//              *(100)    LONG  # initially reserve 100

	    if(c->name.char_at(0) == '*'){
		c->name[(unsigned)0]='0';
	    }

//  Hoist a type definition for non-terminal arrays

	    if(c->child!=0){
		DEBUG(cerr << "non-terminal array\n";)
		DEBUG(cerr << "after hoisting,\n";)

//  Set up a new G2NODE for the hoisted type

		if(gbuf->ptr >= gbuf->end){
		    error("out of memory\n");
		    exit(1);
		}
		G2NODE* n = gbuf->ptr++;
		n->name = c->val; 
		n->val = "HOISTED";          // jfi
		n->next = 0;
		n->child = c->child;

//  Detach the subtree rooted at p's child 

		c->child = 0;
		DEBUG(cerr << "p=\n";)
		DEBUG(showtree_ATTLC(p,0);)
		DEBUG(cerr << "n=\n";)
		DEBUG(showtree_ATTLC(n,0);)

//  Link the subtree into the list of generated type trees

		if(tail){
		    tail->next = n;
		    tail = n;
		}else{
		    head = tail = n;
		}
	    }
	}

//  Fix up Strings declared with "*"

    }else if(p->val.char_at(0)=='*'){
	DEBUG(cerr << "p->val.char_at(0)=='*' means p is a flexible string\n";)

//  Again, we only modify the first character, preserving
//  any initial-reserve specification that may follow the 
//  asterisk, e.g.: 
//
//      usr    *(100)    # initially reserve 100   

	p->val[(unsigned)0] = '0';
	DEBUG(showtree_ATTLC(p,0);)

//  Fix up arrays declared with "*"

    }else if(p->name.char_at(0)=='*'){
	DEBUG(cerr << "p is a flexible array\n";)

//  Again, we only modify the first character, preserving
//  any initial-reserve specification that may follow the 
//  asterisk, e.g.: 
//
//      usr    
//              *(100)    LONG  # initially reserve 100

	p->name[(unsigned)0] = '0';
	DEBUG(showtree_ATTLC(p,0);)
    }
}

void 
bolt(const String& stem, FILE* hf, FILE* cf){

//  Bolt .h and .c files together with strange symbol 
    
    String fastener = "g2" + stem + ranstr(4) + "_ATTLC";
    fprintf(hf, "extern int %s;\n", (const char*)fastener);
    fflush(hf);  // Debug
    fprintf(hf, "static int *_%s = &%s;\n\n", 
	(const char*)fastener, 
	(const char*)fastener);
    fflush(hf);  // Debug
    fprintf(cf, "int %s;\n", (const char*)fastener);
    fflush(hf);  // Debug

//  The following trick eliminates the warning about the
//  unused static int* given by cfront (the inline should 
//  never be "laid down"

    fprintf(hf, "inline void __%s(){ (*_%s)++; }\n",
	(const char*)fastener, 
	(const char*)fastener);
    fflush(hf);  // Debug
}

void 
resettbl(){
    DEBUG(cerr << "enter resettbl\n";)
    nulldef.name="";
    nulldef.val="";
    nulldef.next = nulldef.child = 0;

//  set up definition for char type
//
//    char_def:
//
//        tattoo   G2MOTHER_ATTLC
//        root           o--------------> chardef
//        base           o--------------> chardef
//        ptr            o--------------> nulldef
//        end            o-----------|
//        buf      chardef           |
//                 nulldef           |
//                 xxxxxxx <----------
//                 xxxxxxx
//
//    chardef:
//   
//        name    "CHAR"
//        val     "-100"
//        next    o----->
//        child   o----->
//

    DEBUG(cerr << "ready to initialize chardef\n";)
    chardef.name="CHAR";
    chardef.val=CHAR_ASC_ATTLC;
    chardef.next = chardef.child = 0;
    DEBUG(shownode_ATTLC(&chardef);)

    DEBUG(cerr << "ready to initialize char_def\n";)
    char_def.tattoo=G2MOTHER_ATTLC;
    char_def.buf.size(2);
    char_def.buf[0]=chardef;
    char_def.buf[1]=nulldef;
    char_def.base=char_def.root=char_def.buf;
#if defined(_MSC_VER) || defined(__TCPLUSPLUS__) || defined(__SUNPRO_CC) || defined(__GNUG__)
    char_def.ptr=((G2NODE*)char_def.buf)+1;
    char_def.end=((G2NODE*)char_def.buf)+2;
#else
    char_def.ptr=char_def.buf+1;
    char_def.end=char_def.buf+2;
#endif
    DEBUG(showbuf_ATTLC(&char_def);)

//  do same for short...

    DEBUG(cerr << "ready to initialize shortdef\n";)
    shortdef.name="SHORT";
    shortdef.val=SHORT_ASC_ATTLC;
    shortdef.next = shortdef.child = 0;
    DEBUG(shownode_ATTLC(&shortdef);)

    DEBUG(cerr << "ready to initialize short_def\n";)
    short_def.buf.size(2);
    short_def.buf[0]=shortdef;
    short_def.buf[1]=nulldef;
    short_def.base=short_def.root=short_def.buf;
#if defined(_MSC_VER) || defined(__TCPLUSPLUS__) || defined(__SUNPRO_CC) || defined(__GNUG__)
    short_def.ptr=((G2NODE*)short_def.buf)+1;
    short_def.end=((G2NODE*)short_def.buf)+2;
#else
    short_def.ptr=short_def.buf+1;
    short_def.end=short_def.buf+2;
#endif
    DEBUG(showbuf_ATTLC(&short_def);)

//  ...and long

    DEBUG(cerr << "ready to initialize longdef\n";)
    longdef.name="LONG";
    longdef.val=LONG_ASC_ATTLC;
    longdef.next = longdef.child = 0;
    DEBUG(shownode_ATTLC(&longdef);)

    DEBUG(cerr << "ready to initialize long_def\n";)
    long_def.buf.size(2);
    long_def.buf[0]=longdef;
    long_def.buf[1]=nulldef;
    long_def.base=long_def.root=long_def.buf;
#if defined(_MSC_VER) || defined(__TCPLUSPLUS__) || defined(__SUNPRO_CC) || defined(__GNUG__)
    long_def.ptr=((G2NODE*)long_def.buf)+1;
    long_def.end=((G2NODE*)long_def.buf)+2;
#else
    long_def.ptr=long_def.buf+1;
    long_def.end=long_def.buf+2;
#endif
    DEBUG(showbuf_ATTLC(&long_def);)

//  ...and string

    DEBUG(cerr << "ready to initialize stringdef\n";)
    stringdef.name="STRING";
    stringdef.val=STRING_ASC_ATTLC;
    stringdef.next = stringdef.child = 0;
    DEBUG(shownode_ATTLC(&stringdef);)

    DEBUG(cerr << "ready to initialize string_def\n";)
    string_def.buf.size(2);
    string_def.buf[0]=stringdef;
    string_def.buf[1]=nulldef;
    string_def.base=string_def.root=string_def.buf;
#if defined(_MSC_VER) || defined(__TCPLUSPLUS__) || defined(__SUNPRO_CC) || defined(__GNUG__)
    string_def.ptr=((G2NODE*)string_def.buf)+1;
    string_def.end=((G2NODE*)string_def.buf)+2;
#else
    string_def.ptr=string_def.buf+1;
    string_def.end=string_def.buf+2;
#endif
    DEBUG(showbuf_ATTLC(&string_def);)

//  reinitialize mtblhead-> list to empty

    DEBUG(cerr << "ready to initialize mtblhead-> list to empty\n";)
    mtbl* mp = mtblhead;
    DEBUG(cerr << "in resettbl, mp = " << long(mp) << "\n";)

    while( mp ){
	mtbl* t = mp;
	mp = mp->next;
	delete t;
    }
    mtblhead = NULL;

//  Pre-populate list with definitions for string, long, short, 
//  and char types 
//
//                                                     mtbltail
//                                                        |
//                                                        V
//           mtblhead------>o  o-------->o  o------------>o  o
//                          |            |                |
//                          |            |                |
//                          V            V                V
//                       long_def    short_def         char_def
//
    addtbl(&string_def);
    addtbl(&long_def);
    addtbl(&short_def);
    addtbl(&char_def);
}
////--------------------------------------------
//#include "compfns.h"
mtbl*
addtbl(G2BUF* buf){

    // mtbl* mp = (mtbl*)calloc(1,sizeof(mtbl));
    mtbl* mp = new mtbl;
    mp->buf = 0;
    mp->next=0;

    if( !mp ){
	error("out of memory\n");
	exit(1);
    }
    mp->buf = buf;
    
    if( !mtblhead ){  // first time 
	mtblhead = mtbltail = mp;
    }else{
	mtbltail->next = mp;
	mtbltail = mp;
    }
    return mp;
}

void 
remtbl(){
    mtbl* mp=mtblhead;
    mtbl* temp;
    mtbltail=0;

//  Find the end of the list

    while(mp && ((temp=mp->next) != 0)){
	mtbltail=mp;
	mp=temp;
    }

//  Remove the last element and fix up pointers

    if( mtbltail ){
	mtbltail->next=0;
    }else{
	mtblhead=0;
    }
}

#if 0
void 
prtbls(const String& stem, FILE* hf, FILE* cf){
    int	i;
    String ustem=upper(stem);
    mtbl* mp;
    
    fprintf(hf, "extern G2DESC	*%s[];\n", (const char*)stem);
    fflush(hf);  // Debug
    fprintf(hf, "typedef union %s {\n", (const char*)ustem);
    fflush(hf);  // Debug
    for( mp = mtblhead; mp; mp = mp->next ){
	String name = mp->buf->root->name;
	String uname=upper(name);

	if( !isupper(name.char_at(0)) ){
	    fprintf(hf, "	%s	%s;\n", 
		(const char*)uname, 
		(const char*)name);
	    fflush(hf);  // Debug
	}
    }
    fprintf(hf, "} %s;\n\n", (const char*)ustem);
    fflush(hf);  // Debug
    i = 0;

    for( mp = mtblhead; mp; mp = mp->next ){
	String name = mp->buf->root->name;

	if( !isupper(name.char_at(0)) ){
	    String uname=upper(name);
	    fprintf(hf, "#define %s_%s	%d\n",
		(const char*)ustem,
		(const char*)uname,
		i++);
	    fflush(hf);  // Debug
	}
    }
    fprintf(cf, "G2DESC	*%s[] = {\n", (const char*)stem);
    fflush(cf);  // Debug
    for( mp = mtblhead; mp; mp = mp->next ){
	String name = mp->buf->root->name;

	if( !isupper(name.char_at(0)) ){
	    fprintf(cf, "	%s,\n", (const char*)name);
	    fflush(cf);  // Debug
	}
    }
    fprintf(cf, "	0\n");
    fflush(cf);  // Debug
    fprintf(cf, "};\n");
    fflush(cf);  // Debug
    fprintf(cf, "int	%s_nrec_ = %d;\n\n", (const char*)stem, i);
    fflush(cf);  // Debug
}
#endif

String 
lower(const String& s){
    String result(Stringsize(s.length()));

    for( int i=0;i<s.length();i++ ){
	
	if( isupper(s.char_at(i)) ){
	    result += tolower(s.char_at(i));
	}else{
	    result += s.char_at(i);
	}
    }
    return result;
}

String 
ranstr(int len){
//#if defined(_MSC_VER) || defined(__TCPLUSPLUS__)
    Stringsize sz_tmp(len);
    String result(sz_tmp);
//#else
//    String result(Stringsize(len));
//#endif
    String range = "abcdefghijklmnopqrstuvwxyz"; // used to include caps and digits

    int j = range.length();
    while( --len >= 0 ){
	int i=rand();
	int k = i % j;
	result+=range.char_at(k);
    }
    DEBUG(cerr << "ranstr returns result = " << result << "\n";)
    return result;
}
ostream& operator<<(ostream& os, const udt_info_ATTLC &) {
	return os;
}
unsigned do_checksum(const char *fname) {
	unsigned retval = 0;
	FILE *f = fopen(fname, "r");
	int c;
	while ((c = getc(f)) != EOF) {
		retval =  retval * 271 + c;
	}
	fclose(f);
	return (retval);
}
