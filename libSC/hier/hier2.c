/*ident	"@(#)hier:hier2.c	3.1" */
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

#include "hier.h"

CXXLexer *lexer = 0;
int isolateds = 0;	// display isolated nodes?
int diffAggs = 0;	// differentiate type of aggregate?
int diffInher = 1;	// differentiate type of inheritance?
int rootFileOnly = 0;	// produce info for the root file only? (i.e., not for included files)
int doClasses = 0;
int doStructs = 0;
int doUnions = 0;
int whereFrom = 0;
int backwardEdges = 0;
int asciiOutput = 0;
ostream *asciiout = &cout;
ostream *dagout = &cout;  // normally overridden by -d switch

static char *cinName = "standard input";

static void getopts(int argc, char *argv[])
{
	extern int opterr;
	extern char *optarg;
	int errflg = 0;
	opterr = 0;
	int c;
	while ((c = getopt(argc, argv, "d:f:AaikntscTum")) != EOF)
	{
		switch (c)
		{
			case 'A':
				asciiOutput = 1;
				break;
			case 'k':
				backwardEdges = 1;
				break;
			case 'd':
				dagout = new ofstream(optarg, ios::app);
				break;
			case 'f':
				cinName = optarg;
				break;
			case 'c':
				doClasses = 1;
				break;				
			case 's':
				doStructs = 1;
				break;				
			case 'u':
				doUnions = 1;
				break;				
			case 'a':
				diffAggs = 1;
				break;
			case 'i':
				diffInher = 0;
				break;
			case 'n':
				isolateds = 1;
				break;
			case 'm':
				whereFrom = 1;
				break;
			case 't':
				rootFileOnly = 1;
				break;
			case '?':
				errflg++;
				break;
		}
	}
	if (errflg)
	{
		cerr << "usage: hier2 [-AacTimnstu] [-f cinName] [-d dagoutfile] file ..." << endl;
		exit(2);
	}
	if (!doClasses && !doStructs && !doUnions)
		doClasses = 1;
}


main(int argc, char *argv[])
{
	extern int optind;
	extern char *optarg;

	getopts(argc, argv);

	lexer = new CXXLexer;

//	*dagout << (vertical? ".GS\n" : ".GR\n");
	
	if (optind == argc)
	{
		lexer->attach(&cin, cinName);
		parse();
	}
	else 
	{
		for (; optind < argc; optind++) 
		{
			ifstream in(argv[optind], ios::nocreate);
			if (in)
			{
				lexer->attach(&in, argv[optind]);
				parse();
			}
                        else
				cerr << "hier: can't open " << argv[optind] << endl;
		}			
	}
//	*dagout << ".GE\n";
	if (dagout != &cout)
		delete dagout;
	exit(0);
}

