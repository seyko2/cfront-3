/*ident	"@(#)Graph_alg:demos/GA_cycdemo.c	3.1" */
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

/* 
   This demonstration program determines whether a given Graph is a tree.
*/

#include "GA_demo.h"
#include <stream.h>

// Graphimplement(MyGraph,MyVertex,MyEdge)
Graph_algimplement(MyGraph,MyVertex,MyEdge)

main() {
	MyGraph g;
	MyVertex gv[15];
	
/*  insertion of Vertices will occur when an Edge pulls it in */
	for (int i = 0; i < 7; i++) {
		g.insert(new MyEdge(&gv[i], &gv[(2*i)+1]));
		g.insert(new MyEdge(&gv[i], &gv[(2*i)+2]));
		}
	g.insert(new MyEdge(&gv[3], &gv[4]));

	for (i = 0; i < 15; i++)
		gv[i].id = i;

	MyVertex* v = cycle_u(g);
	if (v)  { //a cycle was found
		cout << "a cycle was found: this is not a tree\n" << flush;

		List_of_p<MyEdge> elist = cycle_list_u(g, v);
		List_of_piter<MyEdge> elisti(elist);
			//let's see where the cycle is

		Set_of_p<MyVertex> vset;
		MyEdge* e;
		while (elisti.next(e)) {
			vset.insert(e->src());
			vset.insert(e->dst());
			}

			// ... and compare it to where we think it is, 
			// the solution set
		Set_of_p<MyVertex> v2set;
		v2set.insert(&gv[1]);
		v2set.insert(&gv[3]);
		v2set.insert(&gv[4]);

		if (vset == v2set) 
			cout << "cyc: The two Vertex sets are equal\n" << flush;
		}

	return(0);
	}


