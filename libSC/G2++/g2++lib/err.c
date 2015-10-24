/*ident	"@(#)G2++:g2++lib/err.c	3.1" */
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

#include <g2io.h>

int (*g2errfp_ATTLC)(int)	// user settable error function pointer
#if NOISY_ATTLC != 0
    = g2errf_ATTLC
#endif
    ;
	
char* g2prog_ATTLC;		// used if set by user
char* g2file_ATTLC;		// ditto 
int g2err_ATTLC;		// error flag, not reset

char* g2errmsg_ATTLC[] = {
    "OK",
    "name longer than G2MAXNAME",
    "value longer than user buffer",
    "name where index is required",
    "non-integers in value of integer leaf",
    "G2 record descriptor corruption",
    "G2 input record too long",
    "G2 input record too deeply nested",
    "G2 input record with bad checksum",
};

int g2errf_ATTLC(int code){

    g2err_ATTLC = code;

    if(g2prog_ATTLC){
	fprintf(stderr, "%s: ", g2prog_ATTLC);
    }
    if(g2file_ATTLC){
	fprintf(stderr, "%s: ", g2file_ATTLC);		
    }
    if( code < 0 || code >= G2HIGHCODE_ATTLC ){
	fprintf(
	    stderr, 
	    "g2 error code %d: %s\n", 
	    code, 
	    "(no error msg avail; something's broken)"
	);
    }else{
	fprintf(
	    stderr, 
	    "g2 error code %d: %s\n", 
	    code, 
	    g2errmsg_ATTLC[code]
	);
    }
    return code;
}
