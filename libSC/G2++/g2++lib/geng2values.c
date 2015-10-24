/*ident	"@(#)G2++:g2++lib/geng2values.c	3.4" */
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

//  Generate file g2values.h

#include <osfcn.h>
#include <stdlib.h>
#include <stdio.h>

char* g2filename_ATTLC = "g2values.h";

main(){
    FILE* f;
    short shortnbr = 1;
    int	intnbr = 1;
    long longnbr = 1;

    if( (f=fopen(g2filename_ATTLC,"w")) == NULL ){
	fprintf(stderr, "cannot open '%s' for writing\n", g2filename_ATTLC);
	exit(1);
    }
    fprintf(f, "/*ident	\"@(#)G2++:incl/g2values.h	3.0\"  */\n");

    fprintf(f, "/******************************************************************************\n");
    fprintf(f, "*\n");
    fprintf(f, "* C++ Standard Components, Release 3.0.\n");
    fprintf(f, "*\n");
    fprintf(f, "* Copyright (c) 1991, 1992 AT&T and Unix System Laboratories, Inc.\n");
    fprintf(f, "* Copyright (c) 1988, 1989, 1990 AT&T.  All Rights Reserved.\n");
    fprintf(f, "*\n");
    fprintf(f, "* THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T and Unix System\n");
    fprintf(f, "* Laboratories, Inc.  The copyright notice above does not evidence\n");
    fprintf(f, "* any actual or intended publication of such source code.\n");
    fprintf(f, "*\n");
    fprintf(f, "******************************************************************************/\n\n");
    fprintf(f, "#ifndef G2VALUESH\n#define G2VALUESH\n\n");
    fprintf(f, "//  This file is written by the program 'geng2values'\n");
    fprintf(f, "//  which should be run on the target machine\n");
    fprintf(f, "//  to generate these machine dependent values.\n\n");
	    
    while (shortnbr >= 0){
	shortnbr <<= 1;
    }
    fprintf(f, "#ifndef HIBITS\n#define HIBITS 0x%x\n#endif\n", (unsigned short)shortnbr);

    while (intnbr >= 0){
	intnbr <<= 1;
    }
    fprintf(f, "#ifndef HIBITI\n#define HIBITI 0x%x\n#endif\n", (unsigned int)intnbr);

    while (longnbr >= 0){
	longnbr <<= 1;
    }
    fprintf(f, "#ifndef HIBITL\n#define HIBITL 0x%lx\n#endif\n", longnbr);
    fprintf(f, "\n#endif\n");
    return 0;
}
