/*ident	"@(#)fs:fsippsrc/fsipp.c	3.1" */
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

#include "fsipp.h"

CXXLexer *lexer;
bool_type alternateLineDirectiveFormat = 0;

static bool_type verbose = 0;

static void getopts(int argc, char *argv[])
{
	extern char *optarg;
	extern int opterr;
	bool_type errflg = no;
	opterr = 0;
	int c;
	while ((c=getopt(argc, argv, "Lv")) != EOF)
	{
		switch (c)
		{
//			case 'f':
//				filename = optarg;
//				break;	
			case 'v':
				verbose = 1;
				break;
			case 'L':
				alternateLineDirectiveFormat = 1;
				break;
			case '?':
				errflg++;
				break;
		}
	}
	if (errflg)
	{
		cerr << "usage: fsipp [-vL] file ...\n";
		exit(2);
	}
}



int const bufsize=2048;
char buf[bufsize];

main(int argc, char *argv[])
{
	extern int optind;
	extern char *optarg;

	getopts(argc, argv);
	lexer = new CXXLexer;
	lexer->verbose(verbose);

	int nErrors = 0;
	if (optind == argc)
	{
		lexer->attach(cin);
		nErrors += parse();
	}
	else 
	{
		for (; optind < argc; optind++) 
		{
			ifstream in(argv[optind], ios::nocreate);
			if (in)
			{
				in.setbuf(buf, bufsize);
				lexer->attach(in, argv[optind]);
				nErrors += parse();
			}
			else 
			{
				cerr << "freestore: can't open " << argv[optind] << endl;
			}
		}			
	}
	exit(nErrors);
}





