/*ident	"@(#)List_old:demos/Pair.c	3.1" */
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

#include "Pair.h"
#include <List_old.h>

// second version from tutorial

List<Pair>&
sort ( List<Pair>& aList )
{
    Listiter<Pair>  unsorted(aList);
    if ( unsorted.step_next() ) {
        Pair p;
        while( unsorted.remove_next(p) ) {
            Listiter<Pair> sorted = unsorted;
            Pair *q;  // pointer into sorted part
            while( sorted.prev(q) && q->name > p.name )
                ;
            if ( q->name < p.name )
                sorted.step_next();   // back up
            else if ( q->name == p.name ) {
                q->count++;
                continue;
            }
            sorted.insert_next(p);
        }
    }
    return aList;
}


main() {
    String	s;
    List<Pair>	myList;

    while (cin >> s)
        myList.put(Pair(s));

    cout << sort(myList) << "\n";
    return 0;
}

