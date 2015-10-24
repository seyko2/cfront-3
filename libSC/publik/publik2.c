/*ident	"@(#)publik:publik2.c	3.1" */
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

#include "publik.h"

CXXLexer *lexer;

bool_type verbose = 0;
bool_type showModifiers = 0;
bool_type showMemberDefs = 0;
bool_type showComments = 1;
bool_type showWhere = 0;

static void getopts(int argc, char *argv[])
{
	extern char *optarg;
	extern int opterr;
	bool_type errflg = no;
	opterr = 0;
	int c;
	while ((c=getopt(argc, argv, "clmpv")) != EOF)
	{
		switch (c)
		{
			case 'c':
				showComments = 0;
				break;
			case 'l':
				showWhere = 1;
				break;
			case 'p':
				showModifiers = 1;
				break;
			case 'm':
				showMemberDefs = 1;
				break;
			case 'v':
				verbose = 1;
				break;
			case '?':
				errflg++;
				break;
		}
	}
	if (errflg)
	{
		cerr << "usage: publik [-clmp] file ...\n";
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
	if (showComments)
		lexer->commentIsToken();

	int nErrors = 0;
	if (optind == argc)
	{
		lexer->attach(&cin, "stdin");
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
				lexer->attach(&in, argv[optind]);
				nErrors += parse();
			}
			else 
			{
				cerr << "publik: can't open " << argv[optind] << endl;
			}
		}			
	}
	exit(nErrors);
}





