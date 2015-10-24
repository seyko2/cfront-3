/*ident	"@(#)Graph_alg:demos/GA_bfsdemo.c	3.1" */
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
   This demonstration program uses a Graph as a tree, traversing via
   breadth-first search until the first instance of a given Vertex is 
   reached.  It then uses breadth-first search again to identify the 
   Vertices in the subtree using that instance as its root.
*/

#include "GA_demo.h"
#include <stream.h>

// Graphimplement(MyGraph,MyVertex,MyEdge)
Graph_algimplement(MyGraph,MyVertex,MyEdge)

int dobbs_find(MyVertex* v) {
	return (v->name != "bob dobbs");
	}

main() {
	MyGraph g;
	MyVertex gv[17];
	
/*  insertion of Vertices will occur when an Edge pulls it in */
	for (int i = 0; i < 7; i++) {
		g.insert(new MyEdge(&gv[i], &gv[(2*i)+1]));
		g.insert(new MyEdge(&gv[i], &gv[(2*i)+2]));
		}
	g.insert(new MyEdge(&gv[10], &gv[15]));
	g.insert(new MyEdge(&gv[10], &gv[16]));
	g.insert(new MyEdge(&gv[11], &gv[15]));
	g.insert(new MyEdge(&gv[11], &gv[16]));

	gv[0].name = "sally smith";
	gv[1].name = "debbie dobbs";
	gv[2].name = "sam smith";
	gv[3].name = "betty black";
	gv[4].name = "bob dobbs"; //first instance
	gv[5].name = "gail green";
	gv[6].name = "steve smith";
	gv[7].name = "wendy white";
	gv[8].name = "bill black";
	gv[9].name = "joan jones";
	gv[10].name = "dan dobbs";
	gv[11].name = "donna dobbs";
	gv[12].name = "gus green";
	gv[13].name = "randi rogers";
	gv[14].name = "stuart smith";
	gv[15].name = "mabel morris";
	gv[16].name = "bob dobbs"; //second instance


	Set_of_p<MyVertex> vpset; 
         //these are the Vertices in the subgraph starting at the 1st instance 
	 //of "bob dobbs", the solution set
	vpset.insert(&gv[4]);
	vpset.insert(&gv[9]);
	vpset.insert(&gv[10]);
	vpset.insert(&gv[15]);
	vpset.insert(&gv[16]);

	/* Now, let's use bfs to accomplish this task */

	List_of_p<MyVertex> vlist = bfs(g, &gv[0], dobbs_find); 
	List_of_piter<MyVertex> vlisti(vlist);
	    //using gv[0] as the root, stop when a Vertex with name
	    //"bob dobbs" is found
	MyVertex* vp = vlist.unput(); //locates the "bob dobbs" Vertex easily
	vlist = bfs(g, vp); //now, get the same Vertex set through bfs 
	Set_of_p<MyVertex> vpset2; 
	while (vlisti.next(vp))
		vpset2.insert(vp);

	if (vpset == vpset2) //explicit vs. bfs-generated set
		cout << "bfs: The two Vertex sets are equal\n" << flush;
	
	return(0);
	}


