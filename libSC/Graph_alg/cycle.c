/*ident	"@(#)Graph_alg:cycle.c	3.1" */
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

//DETECT CYCLES 

/* 
roadmap through cycles

cycle -> intermediate_cycle -> cycle_v -> cycle_e ->cycle0_sub
                                  ^
cycle(v) -> intermediate_cycle_v  |
                                              ^
cycle(e) -> intermediate_cycle_e              |


cycle_list(e) -> intermediate_cycle_list_e -> cycle_sub
                         ^
                         |  <-  <-  <-  <-
                                          ^
cycle_list(v) -> internal_cycle_list_v -> | 

*/

#include "Graph_alg.h"

static Vertex* orig_source;
static Set_of_p<Vertex> stat_v_pset;

//NOTE: cycle0_sub and cycle_sub are similar except that
//cycle_sub builds a list

static tcwbool_ATTLC cycle0_sub(char, const Edge*, int, Vertex**, const Graph&, Vis_v_ticket); 
static tcwbool_ATTLC cycle_sub(char, const Edge*, List_of_p<Edge>&, const Graph&, Vis_v_ticket);
static tcwbool_ATTLC cycle_e(char, const Edge*, int, Vertex**, const Graph&, Vis_v_ticket);
static tcwbool_ATTLC cycle_v(char, const Vertex*, int, Vertex**, const Graph&, Vis_v_ticket);

static int cycle1_test(Edge* e) {  // tests if the current edge's destination
				   // is in the original source
	return(e->dst() == orig_source);
}

static int cycle2_test(Edge* e) {  // tests if current edge's destination
				   // is in the current set being inspected
	return(stat_v_pset.contains(e->dst()) != NULL);
}

static int cycle3_test(Edge* e, Vertex* last_v, Vertex* curr_v) {
	// this is like cycle1_test, except it's for undirected graphs
	if ((last_v != e->src()) && (last_v != e->dst())) {
		Vertex* next_v = (e->src() == curr_v) ? e->dst(): e->src();
		return (next_v == orig_source);  
	}
	else return 0; //this edge has already been traversed
}

static int cycle4_test(Edge* e, Vertex* last_v, Vertex* curr_v) {
	// this is like cycle2_test, except it's for undirected graphs
	if ((last_v != e->src()) && (last_v != e->dst())) {
		Vertex* next_v = (e->src() == curr_v) ? e->dst(): e->src();
		return (stat_v_pset.contains(next_v) != NULL);
	}
	else return 0; //this edge has already been traversed
}

/* cycle() pseudocode:
	if a marked vertex is encountered while in dfs, a 
	cycle exists; the marked vertex is returned
*/


Vertex*
intermediate_cycle_ATTLC(char gtype, const Graph& g) {
	tcwbool_ATTLC found_cycle = FALS;
	Vertex* v;

	Vis_v_ticket vt = Vertex::get_vis_v_ticket();
	Set_of_p<Vertex> spv = g.vertices();
	Set_of_piter<Vertex> vset_iter(spv);
	Vertex* next_v;
	while (next_v = vset_iter.next()) {
		if (!next_v->visited(vt)) {
			next_v->set_visited(vt);
			stat_v_pset.insert(next_v); 
			if (cycle_v(gtype, next_v, 2, &v, g, vt)) {
				found_cycle = TRU;
				break;
			}
			else
				stat_v_pset.remove_all(); //clear for next try
		}
	}
	stat_v_pset.remove_all(); //clear 
	reset_visited(vt, (Set_of_p<Vertex>&)g.vertices());
	Vertex::free_vis_v_ticket(vt);
	return(found_cycle ? v : 0);
}


/* cycle(v) pseudocode:
	if v is encountered twice while in a dfs chain, a cycle exists
*/

int
intermediate_cycle_v_ATTLC(char gtype, const Graph& g, const Vertex* v) {
	tcwbool_ATTLC found_cycle = FALS;
	Vertex* vp;

	if (v) {
		Vis_v_ticket vt = Vertex::get_vis_v_ticket();
		((Vertex*)v)->set_visited(vt);
		stat_v_pset.insert((Vertex*)v);
		found_cycle = cycle_v(gtype, v, 1, &vp, g, vt);
		reset_visited(vt, (Set_of_p<Vertex>&)g.vertices());
		Vertex::free_vis_v_ticket(vt);
		stat_v_pset.remove_all(); //clear
	}
	return(found_cycle);
}


tcwbool_ATTLC
cycle_v(char gtype, const Vertex* v, int test_type, Vertex** vp, const Graph& g, Vis_v_ticket vt) {
	tcwbool_ATTLC found_cycle = FALS;

	if (gtype == 'd') {  /*can't && these 2 expr's together, not implemented*/
		if (v->loop_edges_g(g).size()) { //a loop is a cycle
			*vp = (Vertex*)v;
			found_cycle = TRU;
		}
	}
	Set_of_p<Edge> epset;
	switch (gtype) { /*can't use ternary operator: not implemented for this*/
		case 'd': epset = v->out_edges_g(g);
			  break;
		case 'u': epset = v->edges_g(g);
			  break;
	}
	Set_of_piter<Edge> eset_iter(epset);
	Edge* next_e;  //first in a set is the same as any in a set
	while (next_e = eset_iter.next()) {
		if (cycle_e(gtype, next_e, test_type, vp, g, vt)) {
			found_cycle = TRU;
			break;
		}
	}
	return(found_cycle);
}

/* cycle(e) pseudocode:
	if e is encountered twice while in dfs, a cycle exists.
	This is the same as evaluating whether e's source is visited twice.
*/
int
intermediate_cycle_e_ATTLC(char gtype, const Graph& g, const Edge* e) {
	tcwbool_ATTLC found_cycle = FALS;
	Vertex* v;
	
	if (e) {
		Vis_v_ticket vt = Vertex::get_vis_v_ticket();
		(e->src())->set_visited(vt); //arbitrary for 'u' but consistent
					// with cycle_e
		stat_v_pset.insert(e->src());
		found_cycle = cycle_e(gtype, e, 1, &v, g, vt);
		reset_visited(vt, (Set_of_p<Vertex>&)g.vertices());
		Vertex::free_vis_v_ticket(vt);
		stat_v_pset.remove_all(); //clear
	}
	return(found_cycle);
}


tcwbool_ATTLC
cycle_e(char gtype, const Edge* e, int test_type, Vertex** vp, const Graph& g, Vis_v_ticket vt) {
	tcwbool_ATTLC found_cycle = FALS;
	if (e->src()->visited(vt))  
		orig_source = e->src(); //handles most cases: arbitrary choice 
			//for cycle_u(e) but consistent with it 
	else
		orig_source = e->dst(); //handles a case of cycle_u(v)

	found_cycle = cycle0_sub(gtype, e, test_type, vp, g, vt);
	return(found_cycle);
}

	

/* in this function, test_type == 1 means to look for a cycle that
   contains the node "orig_source", test_type == 2 means to look for
   any old cycle */
tcwbool_ATTLC
cycle0_sub(char gtype, const Edge* e, int test_type, Vertex** vp, const Graph& g, Vis_v_ticket vt) {
	Vertex* v;
	Vertex* last_v;
	Set_of_p<Edge> e_pset;
	int i = e->src()->visited(vt); //better to place these in block below,
	int j = e->dst()->visited(vt); //but get cc warning stmt not reached
	switch (gtype) {
		case 'd':
			v = e->dst();
			e_pset = v->out_edges_g(g);
			break;
		case 'u':
			if (i && j) /*temp ints nec: && not implemented*/
				return(FALS);  
				    //this edge is going back the way it came
			else {
				if (e->src()->visited(vt)) {
					v = e->dst();
					last_v = e->src();
					}
				else {
					v = e->src();
					last_v = e->dst();
					}
				e_pset = v->edges_g(g);
			}
			break;
	}
	Edge* next_e;
	tcwbool_ATTLC found_cycle = FALS;
	v->set_visited(vt);
	stat_v_pset.insert(v);

	Set_of_piter<Edge> e_iter(e_pset);
	if ((test_type == 1) && (gtype == 'd')) {
		while (next_e = e_iter.next()) {
			if (cycle1_test(next_e))
				found_cycle = TRU;
		}
	}
	else if ((test_type == 1) && (gtype == 'u') &&
		(stat_v_pset.size() >= 3)) {
			while (next_e = e_iter.next()) {
			    if (cycle3_test(next_e, last_v, v))
				found_cycle = TRU;
			}
	}
	else if ((test_type == 2) && (gtype == 'd')) {
		while (next_e = e_iter.next()) {
			if (cycle2_test(next_e)) {
				*vp = next_e->dst();
				found_cycle = TRU;
			}
		}
	}
	else if ((test_type == 2) && (gtype == 'u') &&
		(stat_v_pset.size() >= 3)) {
		while (next_e = e_iter.next()) {
			if (cycle4_test(next_e, last_v, v)) {
				*vp = (next_e->src() == v) ? next_e->dst() : next_e->src();
				found_cycle = TRU;
			}
		}
	}

	if (!found_cycle) {      	//keep looking			 
		Set_of_piter<Edge> eset_iter(e_pset);
		Vertex* next_v;
		while (next_e = eset_iter.next()) { 
			if (gtype == 'd')
				next_v = next_e->dst();
			else  //gtype == 'u'
				next_v = (next_e->src() == v) ? next_e->dst() : next_e->src(); 
			if (!next_v->visited(vt)) {
				if (cycle0_sub(gtype, next_e, test_type, vp, g, vt)) {
					found_cycle = TRU;
					break;
				}
			}
		}
	}
	if (!found_cycle) {
		stat_v_pset.remove(v);  //undo insert
	}
	return(found_cycle);
}	

/**************************** CYCLE LISTS ************************************/
/* cycle_list(v) pseudocode:
	if a marked vertex is encountered while in cycle(e), and it is v,
	a cycle exists that includes v; return the cycle.
*/
List_of_p<Edge>
internal_cycle_list_v_ATTLC(const Graph& g, const Vertex* v) {
	List_of_p<Edge> e_list;

	if (v) {
		Set_of_p<Edge> e0_pset = v->loop_edges_g(g);
		if (e0_pset.size() != 0) { 	//loops are cycles
			Set_of_piter<Edge> eset_iter(e0_pset);
			e_list.put(eset_iter.next());
		//	e_list.reset(0);
		}
		else {
			Set_of_p<Edge> spe = v->out_edges_g(g);
			Set_of_piter<Edge> eset2_iter(spe);
			Edge* next_e;  //first in a set is the same as any in a set
			while (next_e = eset2_iter.next()) {
				e_list = intermediate_cycle_list_e_ATTLC('d', g, next_e);
				if (e_list.length())
					break;
			}
		}
	}
	return(e_list);
}

List_of_p<Edge>
internal_u_cycle_list_v_ATTLC(const Graph& g, const Vertex* v) {
	List_of_p<Edge> e_list;

	if (v) {
		Set_of_p<Edge> spe = v->edges_g(g);
		Set_of_piter<Edge> eset_iter(spe);
		Edge* next_e;  //first in a set is the same as any in a set
		while (next_e = eset_iter.next()) {
			e_list = intermediate_cycle_list_e_ATTLC('u', g, next_e);
			if (e_list.length())
				break;
		}
	}
	return(e_list);
}

/* cycle_list(e) pseudocode:
	if the current edge's destination has a back edge to the source 
		then    -- put the back edge on the cycle list and stop
			   (terminating gracefully not forced)
		else 
			-- mark the destination so that don't revisit;
			-- find all edges out of this destination;
			-- try each edge in turn, temporarily putting an
                           edge on the cycle list, calling cycle, and
                           removing the edge later if necessary. 
*/

/* returns list of edges in cycle, including e if other edges in the list*/
List_of_p<Edge>
intermediate_cycle_list_e_ATTLC(char gtype, const Graph& g, const Edge* e) {
	List_of_p<Edge> e_list; 		/*e_list collects the cycle */

	if (e) {
		orig_source = e->src(); //doesn't matter which end we choose for 'u'
		Vis_v_ticket vt = Vertex::get_vis_v_ticket();
		orig_source->set_visited(vt);
		cycle_sub(gtype, e, e_list, g, vt);
		reset_visited(vt, (Set_of_p<Vertex>&)g.vertices());
		Vertex::free_vis_v_ticket(vt);
		if (e_list.length() != 0) {
			// e_list.reset(0);  /*position ptr to head of list for insert */
			// e_list.insert_next(e); //insert start e
			e_list.unget(e);
		}
		// e_list.reset(0);
	}
	return(e_list);
}


tcwbool_ATTLC
cycle_sub(char gtype, const Edge* e, List_of_p<Edge>& e_list, const Graph& g, Vis_v_ticket vt) {
	Vertex* v;
	Vertex* last_v;
	Set_of_p<Edge> e_pset;
	int i = e->src()->visited(vt); //better to place these in block below,
	int j = e->dst()->visited(vt); //but get cc warning stmt not reached
	switch (gtype) {
		case 'd':
			v = e->dst();
			e_pset = v->out_edges_g(g);
			break;
		case 'u':
			if (i && j) /*use of temp ints nec: && not implemented*/
				return(FALS);
				    //this edge is going back the way it came
			else {
				if (e->src()->visited(vt)) {
					v = e->dst();
					last_v = e->src();
				}
				else {
					v = e->src();
					last_v = e->dst();
				}
				e_pset = v->edges_g(g);
			}
			break;
	}
	Edge* next_e;
	tcwbool_ATTLC found_cycle = FALS;
	v->set_visited(vt);

	Set_of_piter<Edge> e_iter(e_pset);
	if ((gtype == 'd')) {
		while (next_e = e_iter.next()) {
			if (cycle1_test(next_e)) {
				e_list.put(next_e);
				found_cycle = TRU;	// cycle was found
			}
		}
	}
	else if ((gtype == 'u') && e_list.length()) {
				//3rd next to see, 1st put on at end
		while (next_e = e_iter.next()) {
			if (cycle3_test(next_e, last_v, v)) {
				e_list.put(next_e);
				found_cycle = TRU;	// cycle was found
			}
		}
	}

	if (!found_cycle) {			//this vertex doesn't have an 
						//edge back to e, keep looking
		Set_of_piter<Edge> eset_iter(e_pset);
		Vertex* next_v;
		while (next_e = eset_iter.next()) { 
			if (gtype == 'd')
				next_v = next_e->dst();
			else  //gtype == 'u'
				next_v = (next_e->src() == v) ? next_e->dst() : next_e->src(); 
			if (!next_v->visited(vt)) {
				e_list.put(next_e);  //temporary put perhaps
				if (cycle_sub(gtype, next_e, e_list, g, vt)) {
					found_cycle = TRU;
					break;
				}
				else
					e_list.unput(next_e);  //edge panned out
			}
		}
	}
	return(found_cycle);
}	

