/*ident	"@(#)Graph_alg:demos/GA_aptsdemo.c	3.1" */
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

/* This demonstration program uses Vertices as U.S. cities, and
   identifies cities that, if disabled, isolate other cities from
   the network.  It assumes that all cities in the network have at
   least one connection to another city.
*/

#include "GA_aptsdemo.h"

// Graphimplement(Citygraph,Cityvertex,Cityedge)
Graph_algimplement(Citygraph,Cityvertex,Cityedge)


main() {
	Citygraph g;
	Cityvertex v[6];

	v[0].id = "NYC";
	v[1].id = "LA";
	v[2].id = "CHGO";
	v[3].id = "BOST";
	v[4].id = "SFRAN";
	v[5].id = "KISHNEV";

	g.insert(new Cityedge(&v[4], &v[1]));
	g.insert(new Cityedge(&v[4], &v[2]));
	g.insert(new Cityedge(&v[2], &v[3]));
	g.insert(new Cityedge(&v[1], &v[5]));
	g.insert(new Cityedge(&v[2], &v[0]));
	g.insert(new Cityedge(&v[3], &v[0]));

	Set_of_p<Cityvertex> vpset = artic_pts(g, g.vertices());
	  //articulation points identify the isolating cities

	//the following is the solution set
	Set_of_p<Cityvertex> v2pset;
	v2pset.insert(&v[1]);
	v2pset.insert(&v[2]);
	v2pset.insert(&v[4]);
	
	if (vpset == v2pset)
		cout << "artic pts: The Vertex sets are equal\n" << flush;

	return 0;
	}
