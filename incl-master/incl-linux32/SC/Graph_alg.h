/*ident	"@(#)Graph_alg:incl/Graph_alg.h	3.1" */
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

#ifndef GRAPHALGS_DOT_H
#define GRAPHALGS_DOT_H

#include <Graph.h>
#include <List.h>
#include <Set.h>

enum tcwbool_ATTLC { FALS, TRU };

class temp_set_dummy_ATTLC;

#include <generic.h>
#define SRCH_ATTLC(V) name2(V,_SRCH_ATTLC)
#define SRCH2_ATTLC(V) name2(V,_SRCH2_ATTLC)
#define CYCLEG_ATTLC(G) name2(G,_CYCLE_ATTLC)
#define CYCLEE_ATTLC(E) name2(E,_CYCLE_ATTLC)
#define CYCLEV_ATTLC(V) name2(V,_CYCLE_ATTLC)
#define ECYCLE_ATTLC(E) name2(E,_ECYCLE_ATTLC)
#define VCYCLE_ATTLC(V) name2(V,_VCYCLE_ATTLC)
#define PSETGA_ATTLC(X) name2(X,_PSETGA_ATTLC)
#define PSETGA2_ATTLC(X) name2(X,_PSETGA2_ATTLC)
#define TEMPSET_ATTLC(X) name2(X,_TEMPSET_ATTLC)
#define GApredicate(V) name2(V,_GApredicate)

typedef int (*GAFUNCPTR_ATTLC)(Vertex*);

extern List_of_p<Vertex> intermediate_dfs_p_ATTLC(char, Graph&, Vertex*, GAFUNCPTR_ATTLC);
extern List_of_p<Vertex> intermediate_bfs_p_ATTLC(char, Graph&, Vertex*, GAFUNCPTR_ATTLC);
extern Vertex* intermediate_cycle_ATTLC(char, const Graph&);
extern int intermediate_cycle_v_ATTLC(char, const Graph&, const Vertex*);
extern int intermediate_cycle_e_ATTLC(char, const Graph&, const Edge*);
extern List_of_p<Edge> internal_cycle_list_v_ATTLC(const Graph&, const Vertex*);
extern List_of_p<Edge> internal_u_cycle_list_v_ATTLC(const Graph&, const Vertex*);
extern List_of_p<Edge> intermediate_cycle_list_e_ATTLC(char,const Graph&,const Edge*);
extern Set_of_p<Vertex> internal_artic_pts2_ATTLC(const Graph&);
extern Set_of_p<Vertex> internal_artic_pts_ATTLC(const Graph&, const Set_of_p<Vertex>&);

#define Graph_algdeclare(G,V,E) \
 \
typedef int GApredicate(V)(V*); \
typedef List_of_p<V> SRCH_ATTLC(V)(char, G&, V*, GApredicate(V)*); \
typedef V* CYCLEG_ATTLC(G)(char, const G&); \
typedef int CYCLEE_ATTLC(E)(char, const G&, const E*); \
typedef int CYCLEV_ATTLC(V)(char, const G&, const V*); \
 \
typedef List_of_p<E> VCYCLE_ATTLC(V)(const G&, const V*); \
typedef List_of_p<E> ECYCLE_ATTLC(E)(char, const G&, const E*); \
typedef Set_of_p<V> PSETGA_ATTLC(V)(const G&, const Set_of_p<V>&); \
typedef Set_of_p<V> PSETGA2_ATTLC(V)(const G&); \
 \
inline List_of_p<V>	dfs(G& g, V* v, GApredicate(V)* f = 0) { \
	return (*((SRCH_ATTLC(V)*)intermediate_dfs_p_ATTLC))('d', g, v, f); \
} \
 \
inline List_of_p<V>	bfs(G& g, V* v, GApredicate(V)* f = 0) { \
	return (*((SRCH_ATTLC(V)*)intermediate_bfs_p_ATTLC))('d', g, v, f); \
} \
 \
inline List_of_p<V>	dfs_u(G& g, V* v, GApredicate(V)* f = 0) { \
	return (*((SRCH_ATTLC(V)*)intermediate_dfs_p_ATTLC))('u', g, v, f); \
} \
 \
inline List_of_p<V>	bfs_u(G& g, V* v, GApredicate(V)* f = 0) { \
	return (*((SRCH_ATTLC(V)*)intermediate_bfs_p_ATTLC))('u', g, v, f); \
} \
 \
inline V*	cycle(const G& g) { \
	return (*((CYCLEG_ATTLC(G)*)intermediate_cycle_ATTLC))('d', g); \
} \
 \
inline int	cycle(const G& g, const V* v) { \
	return (*((CYCLEV_ATTLC(V)*)intermediate_cycle_v_ATTLC))('d', g, v); \
} \
 \
inline int	cycle(const G& g, const E* e) { \
	return (*((CYCLEE_ATTLC(E)*)intermediate_cycle_e_ATTLC))('d', g, e); \
} \
 \
inline List_of_p<E>	cycle_list(const G& g, const V* v) { \
	return (*((VCYCLE_ATTLC(V)*)internal_cycle_list_v_ATTLC))(g, v); \
} \
 \
inline List_of_p<E>	cycle_list(const G& g, const E* e) { \
	return (*((ECYCLE_ATTLC(E)*)intermediate_cycle_list_e_ATTLC))('d', g, e); \
} \
 \
inline V*	cycle_u(const G& g) { \
	return (*((CYCLEG_ATTLC(G)*)intermediate_cycle_ATTLC))('u', g); \
} \
 \
inline int	cycle_u(const G& g, const V* v) { \
	return (*((CYCLEV_ATTLC(V)*)intermediate_cycle_v_ATTLC))('u', g, v); \
} \
 \
inline int	cycle_u(const G& g, const E* e) { \
	return (*((CYCLEE_ATTLC(E)*)intermediate_cycle_e_ATTLC))('u', g, e); \
} \
 \
inline List_of_p<E>	cycle_list_u(const G& g, const V* v) { \
	return (*((VCYCLE_ATTLC(V)*)internal_u_cycle_list_v_ATTLC))(g, v); \
} \
 \
inline List_of_p<E>	cycle_list_u(const G& g, const E* e) { \
	return (*((ECYCLE_ATTLC(E)*)intermediate_cycle_list_e_ATTLC))('u', g, e); \
} \
 \
inline Set_of_p<V>	artic_pts(const G& g) { \
	return (*((PSETGA2_ATTLC(V)*)internal_artic_pts2_ATTLC))(g); \
} \
 \
inline Set_of_p<V>	artic_pts(const G& g, const Set_of_p<V>& vs) { \
	return (*((PSETGA_ATTLC(V)*)internal_artic_pts_ATTLC))(g, vs); \
} \
 \
Set<G> conn_comps_u(const G&); \
Set<G> strong_conn_comps(const G&); \
 \



Graph_algdeclare(Graph,Vertex,Edge)


#define Graph_algimplement(G,V,E) \
 \
int \
scc_sub_ATTLC( \
	V* v, \
	int* i, \
	Set_of_p<Set_of_p<V> >& scc, \
	List_of_p<V>& v_list, \
	int vsize, \
	const G& g, \
	Val_v_ticket& vt) \
{ \
	int min, ret_val; \
	V* e_dst; \
	V* stacked_v; \
	min = ++(*i); \
	v->set_val(vt, min); \
	v_list.put(v); \
	Set_of_p<E> spE = v->out_edges_g(g); \
	Set_of_piter<E> eset_iter(spE); \
	E* e; \
	while (e = eset_iter.next()) { \
		e_dst = e->dst(); \
		ret_val = (!e_dst->val(vt)) ? scc_sub_ATTLC(e_dst,i,scc,v_list,vsize,g,vt) : e_dst->val(vt); \
		if (ret_val < min) \
			min = ret_val; \
	} \
	if (min == v->val(vt)) { \
		Set_of_p<V>* v_pset = new Set_of_p<V>; \
		while (v_list.unput(stacked_v) && (stacked_v != v)) { \
			v_pset->insert(stacked_v); \
			stacked_v->set_val(vt, vsize+1); \
		} \
		v_pset->insert(stacked_v); \
		stacked_v->set_val(vt, vsize+1); \
		scc.insert(v_pset); \
	} \
	return(min); \
} \
 \
void \
fill_graph_ATTLC( \
	const G& orig_g, \
	G* dest_g, \
	Set_of_p<V>& v_pset) \
{ \
	Set_of_piter<V> vset_iter(v_pset); \
	V* vp; \
	while (vp = vset_iter.next()) { \
		dest_g->insert(vp); \
		Set_of_p<E> spE1 = vp->in_edges_g(orig_g); \
		Set_of_piter<E> e1set_iter(spE1); \
		E* ep; \
		while (ep = e1set_iter.next()) { \
			if (v_pset.contains(ep->src())) \
				dest_g->insert(ep); \
		} \
		Set_of_p<E> spE2 = vp->out_edges_g(orig_g); \
		Set_of_piter<E> e2set_iter(spE2); \
		while (ep = e2set_iter.next()) { \
			if (v_pset.contains(ep->dst())) \
				dest_g->insert(ep); \
		} \
		Set_of_p<E> spE3 = vp->loop_edges_g(orig_g); \
		Set_of_piter<E> e3set_iter(spE3); \
		while (ep = e3set_iter.next()) \
			dest_g->insert(ep); \
		v_pset.remove(vp); \
	} \
} \
 \
class TEMPSET_ATTLC(G): public Set<G> { \
public: \
	TEMPSET_ATTLC(G)(const G&, temp_set_dummy_ATTLC*); \
	TEMPSET_ATTLC(G)(const G&, temp_set_dummy_ATTLC*, temp_set_dummy_ATTLC*); \
}; \
 \
TEMPSET_ATTLC(G)::TEMPSET_ATTLC(G)(const G& g, temp_set_dummy_ATTLC*) { \
	const G* g2_ptr; \
	Set_of_p<Set_of_p<V> > cc; \
	List_of_p<V> vlist; \
	V* stacked_v; \
	Set_of_p<V> v_set = g.vertices(); \
	Set_of_piter<V> vset_iter(v_set); \
	V* v; \
	while (v = vset_iter.next()) { \
		vlist = dfs_u((G&)g, v, 0); \
		Set_of_p<V>* vpset = new Set_of_p<V>; \
		while (vlist.unput(stacked_v)) { \
			vpset->insert(stacked_v); \
			v_set.remove(stacked_v); \
		} \
		cc.insert(vpset); \
	} \
	Set_of_piter<Set_of_p<V> > pvset_iter(cc); \
	Set_of_p<V>* vset; \
	while (vset = pvset_iter.next()) { \
		G g2; \
		insert(g2); \
		g2_ptr = contains(g2); \
		fill_graph_ATTLC(g, (G*)g2_ptr, *vset); \
		cc.remove(vset); \
		delete vset; \
	} \
} \
 \
TEMPSET_ATTLC(G)::TEMPSET_ATTLC(G)(const G& g, temp_set_dummy_ATTLC*, temp_set_dummy_ATTLC*) { \
	Val_v_ticket vt = Vertex::get_val_v_ticket(); \
	const G* g2_ptr; \
	Set_of_p<Set_of_p<V> > scc; \
	int j = 0; \
	int* i = &j; \
	List_of_p<V> v_list; \
	int vsize = g.vertices().size(); \
	Set_of_p<V> spV = g.vertices(); \
	Set_of_piter<V> vset_iter(spV); \
	V* v; \
	while (v = vset_iter.next()) \
		if (!v->val(vt)) \
			scc_sub_ATTLC(v, i, scc, v_list, vsize, g, vt); \
	Set_of_piter<Set_of_p<V> > pvset_iter(scc); \
	Set_of_p<V>* vset; \
	while (vset = pvset_iter.next()) { \
		G g2; \
		insert(g2); \
		g2_ptr = contains(g2); \
		fill_graph_ATTLC(g, (G*)g2_ptr, *vset); \
		scc.remove(vset); \
		delete vset; \
	} \
	reset_val(vt, (Set_of_p<V>&)g.vertices()); \
	Vertex::free_val_v_ticket(vt); \
} \
 \
TEMPSET_ATTLC(G) 	list_cc_make_ATTLC(const G& g) { \
	return TEMPSET_ATTLC(G)(g, (temp_set_dummy_ATTLC*)0); \
} \
 \
TEMPSET_ATTLC(G) 	list_scc_make_ATTLC(const G& g) { \
	return TEMPSET_ATTLC(G)(g, (temp_set_dummy_ATTLC*)0, (temp_set_dummy_ATTLC*)0); \
} \
 \
Set<G>	conn_comps_u(const G& g) { return list_cc_make_ATTLC(g); } \
Set<G>	strong_conn_comps(const G& g) { return list_scc_make_ATTLC(g); } \

#ifdef __edg_att_40
#pragma can_instantiate Set<Graph>
#pragma can_instantiate Setiter<Graph>
#pragma can_instantiate List_of_piter<Vertex>
#endif

#endif
