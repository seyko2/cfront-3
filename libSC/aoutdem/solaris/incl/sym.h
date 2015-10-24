#include <osfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "idebug.h"
#include <stream.h>
#include <String.h>
#include <Map.h>

extern void reformat( char* );
extern String demangleAndSimplify(const String &, int, int);

// from demangle.c
extern int overwriteAout;
extern int bufsize;
extern char* tsymfilename;
extern char* tstrfilename;
extern void  finish(int);
extern void  bombout(int);
#ifdef IDEBUG
extern int readOnly;
#endif
