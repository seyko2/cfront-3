/*ident	"@(#)Graph_alg:component.c	3.1" */
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

//COMPONENTS

#include "Graph_alg.h"

static int artic_sub(Vertex*, int*, Vertex*, int*, Set_of_p<Vertex>&, const Graph&, Val_v_ticket&);

/* pseudocode for artic_pts
Sedgewick, Algorithms, 2nd ed., 1988, p.440

	call artic_sub for all vertices in the parameter set (which constitutes
		a connected component)
	if the root v has only 1 child, remove it from the found pts
*/

Set_of_p<Vertex>
internal_artic_pts_ATTLC(const Graph& g, const Set_of_p<Vertex>& v1_pset) {
    //parameter is the vertex set in this connected component being inspected
	Set_of_p<Vertex> v_pset2;
	int j = 0;
	int* i = &j; //"id" in Sedgewick
	int k = 0;
	int* childcount = &k;
	Vertex* root_v;

	Set_of_piter<Vertex> vpset_iter(v1_pset);
	root_v = vpset_iter.next();  //choose any v in the set as the root 
	if (root_v) {
		Val_v_ticket vt = Vertex::get_val_v_ticket();
		artic_sub(root_v, childcount, root_v, i, v_pset2, g, vt);
		if ((*childcount) < 2)
			v_pset2.remove(root_v);
		reset_val(vt, (Set_of_p<Vertex>&)v1_pset);
		Vertex::free_val_v_ticket(vt);
	}
	return(v_pset2);
}

Set_of_p<Vertex>
internal_artic_pts2_ATTLC(const Graph& g) {
	Set_of_p<Vertex> vpset;
	Set<Graph> sg = conn_comps_u(g);
	Setiter<Graph> si(sg);
	const Graph* g2;
	while (g2 = si.next()) 
		vpset |= internal_artic_pts_ATTLC(g, g2->vertices());
	return (vpset);
}

/* pseudocode for artic_sub
	assign the next value to the current v and to "this min"	
	for each v2 as adjacent vertex to v
		if v2 has no assigned value
			if v is the root, increment "childcount"
			repeat artic_sub with v2, returning "returned min" 
			if the returned min < this min, change this min
			if the returned min >= v's val, v is an artic pt
		else if v2's val < this min, change this min  
	return(this min)
*/

int
artic_sub(Vertex* root_v, int* childcount, Vertex* v, int* i, Set_of_p<Vertex>& pset, const Graph& g, Val_v_ticket& vt) {
	int min, ret_val;
	Vertex* adj_v;
	
	min = ++(*i);
	v->set_val(vt, min); 	
	Set_of_p<Edge> spe = v->edges_g(g);
	Set_of_piter<Edge> eset_iter(spe);
	Edge* e;
	while (e = eset_iter.next()) {
		adj_v = ((e->src()) == v) ? e->dst() : e->src();
		if (!adj_v->val(vt)) { //adjacent v has no val 
			if (v == root_v)  
				(*childcount)++; 
			ret_val = artic_sub(root_v, childcount, adj_v, i, pset, g, vt);
			if (ret_val < min)
				min = ret_val;
			if (ret_val >= v->val(vt)) 
				pset.insert(v); //this is an articulation point
		}
		else if (adj_v->val(vt) < min)
			min = adj_v->val(vt);
	} 
	return(min);
}

