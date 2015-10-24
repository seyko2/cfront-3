/*ident	"@(#)Graph:demos/Cities.c	3.1" */
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

/* This demonstration program creates a Graph of cities.  It then creates
   a subgraph of these cities.
*/

#include "Cities.h"

// Graphimplement(Citygraph,Cityvertex,Cityedge)

typedef Cityvertex* Vptr;

// Mapdeclare(String,Vptr)
// Mapimplement(String,Vptr)

main() {

	Citygraph g;
	Cityvertex v[6];

	Map<String,Vptr> m;

	m["SFRAN"] = &v[0];
	v[0].id = "SFRAN";

	m["LA"] = &v[1];
	v[1].id = "LA";

	m["CHGO"] = &v[2];
	v[2].id = "CHGO";

	m["DLLS"] = &v[3];
	v[3].id = "DLLS";

	m["BOST"] = &v[4];
	v[4].id = "BOST";

	m["NYC"] = &v[5];
	v[5].id = "NYC";

	Cityedge e0(m["CHGO"], m["BOST"], 0);
	Cityedge e1(m["SFRAN"], m["LA"], 1);
	Cityedge e2(m["LA"], m["SFRAN"], 2);
	Cityedge e3(m["SFRAN"], m["CHGO"], 3);
	Cityedge e4(m["SFRAN"], m["NYC"], 4);
	Cityedge e5(m["SFRAN"], m["BOST"], 5);
	Cityedge e6(m["BOST"], m["SFRAN"], 6);
	Cityedge e7(m["NYC"], m["SFRAN"], 7);
	Cityedge e8(m["CHGO"], m["SFRAN"], 8);
	Cityedge e9(m["DLLS"], m["NYC"], 9);
	Cityedge e10(m["BOST"], m["LA"], 10);

	g.insert(&e0);
	g.insert(&e1);
	g.insert(&e2);
	g.insert(&e3);
	g.insert(&e4);
	g.insert(&e5);
	g.insert(&e6);
	g.insert(&e7);
	g.insert(&e8);
	g.insert(&e9);
	g.insert(&e10);

	Set_of_p<Cityvertex> vpset;
	vpset.insert(m["SFRAN"]);
	vpset.insert(m["LA"]);
	vpset.insert(m["BOST"]);
	vpset.insert(m["NYC"]);
	vpset.insert(m["CHGO"]);

	Citygraph g2 = g.induced_graph(vpset);
		//create the subgraph of these Vertices and appropriate Edges

	// Now let's verify what is in the newly-created subgraph

	for (int i = 0; i < 6; i++) 
		cout << v[i].id << ": " << g2.contains(&v[i]) << "\n" << flush;

	cout << "e with id " << e0.id << ": "<< g2.contains(&e0)<< "\n"<< flush;
	cout << "e with id " << e1.id << ": "<< g2.contains(&e1)<< "\n"<< flush;
	cout << "e with id " << e2.id << ": "<< g2.contains(&e2)<< "\n"<< flush;
	cout << "e with id " << e3.id << ": "<< g2.contains(&e3)<< "\n"<< flush;
	cout << "e with id " << e4.id << ": "<< g2.contains(&e4)<< "\n"<< flush;
	cout << "e with id " << e5.id << ": "<< g2.contains(&e5)<< "\n"<< flush;
	cout << "e with id " << e6.id << ": "<< g2.contains(&e6)<< "\n"<< flush;
	cout << "e with id " << e7.id << ": "<< g2.contains(&e7)<< "\n"<< flush;
	cout << "e with id " << e8.id << ": "<< g2.contains(&e8)<< "\n"<< flush;
	cout << "e with id " << e9.id << ": "<< g2.contains(&e9)<< "\n"<< flush;
	cout << "e with id " << e10.id << ": "<< g2.contains(&e10)<< "\n"<< flush;

	return(0);
	}
