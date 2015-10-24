/*ident	"@(#)Graph_alg:demos/GA_demo.h	3.1" */
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

#ifndef GA_DEMO_DOT_H
#define GA_DEMO_DOT_H

#include "Graph_alg.h"
#include <String.h>
#include <stream.h>

/*
   Graph_alg needs the interface as defined in Graph.

   Except for the new members name and id under MyVertex, the following 
   is the basic format necessary for Graph declaration as it is currently
   defined.  
*/

Graphdeclare1(MyGraph,MyVertex,MyEdge)

class MyGraph: public Graph {
	public:
		MyGraph() : Graph() {}
		MyGraph(const MyGraph& g) : Graph(g) {}
		derivedGraph(MyGraph,MyVertex,MyEdge)
	};
class MyVertex: public Vertex {
	public:
		String name;
		int id;
		MyVertex() : Vertex() {id = 0;}
		derivedVertex(MyGraph,MyVertex,MyEdge)
	};
class MyEdge: public Edge {
	public:
		MyEdge(MyVertex* v1, MyVertex* v2) : Edge(v1, v2) {}
		MyEdge(const MyEdge& e) : Edge(e) {}
		derivedEdge(MyGraph,MyVertex,MyEdge)
	};
Graphdeclare2(MyGraph,MyVertex,MyEdge)

/* All that is necessary for Graph_alg declaration */

Graph_algdeclare(MyGraph,MyVertex,MyEdge)

#endif
