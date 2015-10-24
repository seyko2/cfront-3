/*ident	"@(#)Symbol:demos/symtab.c	3.1" */
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

#include <Map.h>
#include <String.h>
#include <Symbol.h>
#include <stream.h>
#include <Stopwatch.h>
#include <stdlib.h>

#define SYMBOL 1

#ifdef SYMBOL
typedef Symbol KEY;
#else
typedef String KEY;
#endif

Map<KEY,int> table;

char* tablekeys[] = { "now", "is", "the", "time", "for", "all", 
	"good", "people", "to", "come", "aid", 
	"of", "their", "party", 0 };

struct probekey {
	char* s;
	KEY* k;
} probekeys[] = { 
	"now", 0,
	"come", 0,
	"foo", 0,
	"bar", 0,
	"all", 0,
	"symbol", 0,
	"x", 0,
	"good", 0,
	"quick", 0,
	"brown", 0,
	"fox", 0,
	"aid", 0,
	"zzz", 0,
	"probes", 0,
	"table", 0,
	"map", 0,
	"their", 0,
	"x1", 0,
	"x2", 0,
	"x3", 0,
	0, 0
};

void build_table()
{
	for ( char** p = tablekeys ; *p != 0 ; ++p )
	{
		table[*p] = 0;
	}
}

void make_probes()
{
	for ( probekey* p = probekeys ; p->s != 0 ; ++p)
	{
		p->k = new KEY(p->s);
	}
}

void lookup(int lookups_per_key)
{
	for ( int i = lookups_per_key ; i > 0 ; --i )
	{
		for ( probekey* p = probekeys ; p->s != 0 ; ++p )
		{
			(void)table.element(*p->k);
		}
	}
}

main(int argc, char** argv)
{
	if ( argc != 3 )
	{
		cerr << "usage: symtab #trials #lookups_per_key" << endl;
		exit(2);
	}
	int trials = atoi(argv[1]);
	int lookups_per_key = atoi(argv[2]);
	Stopwatch w;
	w.start();
	while ( trials-- > 0 )
	{
		build_table();
		make_probes();
		lookup(lookups_per_key);
	}
	w.stop();
	//cout << "user: " << w.user() << "\nreal: " << w.real();
	cout << endl;
	return 0;
}
