/*ident	"@(#)Graph_alg:bfs.c	3.1" */
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

#include "Graph_alg.h"

/* bfs pseudocode

Aho, Hopcroft, Ullman, Data Structures and Algorithms, 1983, p. 243

	mark the first v as visited and put on the list
	while there are v's on the list and the user's func doesn't terminate 
           the search,
		  find the adjacencies for the next list element to be processed
		  put each of these adjacencies that are unmarked on the list, 
		    and mark them
*/

//BREADTH-FIRST SEARCH

static void bfs_sub(char, Vertex*, List_of_p<Vertex>&, GApredicate(Vertex)*, Graph&, Vis_v_ticket);

List_of_p<Vertex>
intermediate_bfs_p_ATTLC(char gtype, Graph& g, Vertex* v, GApredicate(Vertex)* funcaddr) {
	List_of_p<Vertex> vlist;

	if (v) {
		Vis_v_ticket vt = Vertex::get_vis_v_ticket();
		bfs_sub(gtype, v, vlist, funcaddr, g, vt);
		reset_visited(vt, (Set_of_p<Vertex>&)g.vertices());
		Vertex::free_vis_v_ticket(vt);
		// vlist.reset(0);
	}
	return(vlist);
}

void
bfs_sub(char gtype, Vertex* v, List_of_p<Vertex>& vlist, GApredicate(Vertex)* funcaddr, Graph& g, Vis_v_ticket vt) {
	Vertex* next_v;
	Vertex* adj;
	Set_of_p<Edge> e_pset;

	v->set_visited(vt);
	if (g.contains(v)) //test needed for initial v param
		vlist.put(v);

	List_of_piter<Vertex> vlisti(vlist);

	while (vlisti.next(next_v))  //more v's to check 
		if (funcaddr!=0 && !(*funcaddr)(next_v)) {//user func stops trav
			while (vlisti.remove_next()) ; //chops off end of list
			break;
		}
		else {
			switch (gtype) {
				case 'd': e_pset = next_v->out_edges_g(g);
					  break;
				case 'u': e_pset = next_v->edges_g(g);
					  break;
			}
			Set_of_piter<Edge> epset_iter(e_pset);
			Edge* ep;
			while (ep = epset_iter.next()) {
				switch (gtype) {
					case 'd': adj = ep->dst(); 
						  break;
					case 'u': adj = ((ep->src())==next_v) ? ep->dst() : ep->src(); 
						  break;
				}
				if (!adj->visited(vt)) {
					adj->set_visited(vt);
					vlist.put(adj);/*put it on the list*/
				}
			}
		}
}
