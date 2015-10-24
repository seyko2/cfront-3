/*ident	"@(#)incl:incl2.c	3.1" */
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

#include <stdlib.h>
#include <osfcn.h>
#include <List.h>
#include <Map.h>
#include <String.h>
#include <iostream.h>
#include <fstream.h>
#include <ctype.h>

int asciiOutput = 0;
ostream *asciiout = &cout;
ostream *dagout = &cout;  // normally overridden by -d switch

int kflag = 0;  // Don't draw <> or "" pairs 
int lflag = 0;  // Draw full path names

#ifdef __GNUG__
extern char *optarg;
extern int optind;
#endif

/* incl : draw a picture of an #include hierarchy for a set of files. */

/* General approach: maintain a list of files yet to be scanned. We handle
all files as full path names (the routine found() figures out what the full
path names are), and we use the "names" map to print out shorter versions of
the file names. */

/* Three basic data structures: */

List<String> paths;	// -I path (for mapping short names to full path names)

List<String> to_be_done; // full path names that have not yet been scanned

Map<String,String> names;  // full path names -> names to be printed.

const int READ = 4;

/* This routine attempts to find a file by the name "s" by first checking
   it against dot, and then by trying every String in the "path" in order.
   Returns 1 if the search is successful; the found name is constructed in
   the out-parameter new_s. */

/* This routine should be replaced by the equivalent in the Paths class when
   it becomes available. */

static int
find(const String& s, /* out*/ String& new_s)
{
		/*Known bug: we look in "." even for #include <x> */

	if (access(s,READ) == 0)
	{
		new_s = s;
		return 1;
	}

	if (s[(unsigned)0] == '/')
	{
		new_s = s;
		return 0;
	}

	Listiter<String> iter(paths);
	String path;
	while(iter.next(path))
	{
		/*Note: "s" and "new_s" might be the same, so don't
		  build the temporary result in new_s ! */

		String tryit(path + "/" + s);
		if (access(tryit,READ) == 0)
		{
			new_s = tryit;
			return 1;
		}
	}
	cerr << "incl: could not find " << s << endl;
	new_s = s;
	return 0;
}
	
/*This is the routine that reads through a file, finds the #includes, and
  for each such #include:
		1) generates a dag line to stdout;
		2) "find"s the new file and puts it in the to_be_done list if it
		   has not already been done.

*/

static void
process(istream& in, String file)
{
	const int LINESIZE=1024;
	char line[LINESIZE];
	while( in.getline(line,LINESIZE,'\n').good())
	{
		/*I do this by hand because the Sun regexp is SO hairy! */

		if (line[0] != '#')
			continue;
		int i = 1;
		while (isspace(line[i]))
			++i;
		if (strncmp(line+i,"include",7) != 0)
			continue;
		i += 7;
		while (isspace(line[i]))
			++i;

		String prefix,suffix;

		switch(line[i++])
		{
		case '"':
			if (!kflag)
			{
				prefix = suffix = "\\\"";
			}
			break;
		case '<':
			if (!kflag)
			{
				prefix = "<";
				suffix = ">";
			}
			break;
		default:
			continue;
		}

		while (isspace(line[i]))
			++i;

		String short_name;
		while (isalnum(line[i])
		|| line[i] == '/'
		|| line[i] == '.'
		|| line[i] == '_')
			short_name.put(line[i++]);

		String full_path_name;
		find(short_name,full_path_name);

		if (!names.element(full_path_name))
		{
			if (lflag)
				names[full_path_name] = full_path_name;
			else
				names[full_path_name] = prefix + short_name + suffix;
			to_be_done.put(full_path_name);
		}
		*dagout << "\"" << names[file] << "\"	\"" << names[full_path_name] << "\";\n";
		if (asciiOutput)
			*asciiout << names[file] << '\t' << "#includes" << '\t' << names[full_path_name] << "\n";
	}
}

static void
process(const String& file)
{
	ifstream in(file);
	if (in.fail() || in.bad())
		return;
	process(in,file);
}


main(int argc,char* argv[])
{
	int c;
	
	while ( (c = getopt(argc,argv,"Akld:I:")) != -1)
	{
		switch(c)
		{
		case 'A':
			asciiOutput = 1;
			break;
		case 'd':
			dagout = new ofstream(optarg, ios::app);
			break;
		case 'I': 
			paths.put(String(optarg));
			break;
		case 'k':
			kflag++;
			break;
		case 'l':
			lflag++;
			break;
		}
	}

	paths.put("/usr/include");

	/*If no files were specified just read standard input */

	if (optind >= argc)
	{
		String standard("standard input");
		names[standard] = standard;
		process(cin, standard);
	}
	else
	{
		for (; optind < argc; ++optind)
		{
			String found;
			if (find(argv[optind],found))
			{
				to_be_done.put(found);
				names[found] = argv[optind];
			}
		}
	}
	String current;
	while (to_be_done.get(current))
		process(current);

	if (dagout != &cout)
		delete dagout;
	exit(0);
}
