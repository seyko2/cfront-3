/*ident	"@(#)Graph_alg:dfs.c	3.1" */
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

//dfs

#include "Graph_alg.h"

static Vertex* nextv(char, Edge*, Vertex*);

Vertex*
nextv(char gtype, Edge* e, Vertex* v) {

	if (!e)
		return(0);
	else
		switch (gtype) {
			case 'd': return(e->dst()); 
			case 'u': return( (e->src()==v) ? e->dst() : e->src());
		}
	return(0);  // no warning re "non-void function should return a value"
}


/* dfs pseudocode

Aho, Hopcroft, Ullman, Data Structures and Algorithms, 1983, p. 216

	mark the incoming v as visited and put it on the list
	while the user's func doesn't terminate the search and there is
	  another vertex to check
		if the next vertex (being an out_edge of the incoming v)
		   hasn't been visited, repeat		
*/
	
static tcwbool_ATTLC dfs_sub(char, Vertex*, List_of_p<Vertex>&, GApredicate(Vertex)*, Graph&, Vis_v_ticket);

List_of_p<Vertex>
intermediate_dfs_p_ATTLC(char gtype, Graph& g, Vertex* v, GApredicate(Vertex)* funcaddr) {
	List_of_p<Vertex> vlist;

	if (v) {
		Vis_v_ticket vt = Vertex::get_vis_v_ticket();
		dfs_sub(gtype, v, vlist, funcaddr, g, vt);
		reset_visited(vt, (Set_of_p<Vertex>&)g.vertices());
		Vertex::free_vis_v_ticket(vt);
	}
	return(vlist);
}


tcwbool_ATTLC
dfs_sub(char gtype, Vertex* v, List_of_p<Vertex>& vlist, GApredicate(Vertex)* funcaddr,Graph& g, Vis_v_ticket vt) {
	Set_of_p<Edge> e_pset;
	Vertex* next_v;

	tcwbool_ATTLC go = TRU;    
	v->set_visited(vt);    
	if (g.contains(v)) //test needed for initial v param
		vlist.put(v);
	
	if (funcaddr!=0 && !(*funcaddr)(v)) //user's func terminates traverse
		return(FALS);
	else {
		switch (gtype) {
			case 'd': e_pset = v->out_edges_g(g);
				  break;
			case 'u': e_pset = v->edges_g(g);
				  break;
		}
		Set_of_piter<Edge> epset_iter(e_pset);
		next_v = nextv(gtype, epset_iter.next(), v);
		while (next_v) {	//there is another v to check
		  if (!next_v->visited(vt))  
		  	if (!dfs_sub(gtype, next_v, vlist, funcaddr, g, vt)) {
				go = FALS;
				break;
			}
		  next_v = nextv(gtype, epset_iter.next(), v);
		}
	}
	return(go);
}


