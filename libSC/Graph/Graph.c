/*ident	"@(#)Graph:Graph.c	3.1" */
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

#include "Graph.h"
#include <assert.h>

static Vis_v_ticket vis_v_default;
static int vis_v = 0;

static Val_v_ticket val_v_default;
static int val_v = 0;

static Vis_e_ticket vis_e_default;
static int vis_e = 0;

static Val_e_ticket val_e_default;
static int val_e = 0;

static int bits_increment = sizeof(unsigned long); //should be ok for all machines

const PSETF_ATTLC(Graph)	Vertex_graphs_ATTLC = &Vertex::graphs;
const PSETF_ATTLC(Edge)		Vertex_edges_ATTLC = &Vertex::edges;
const PSETFG_ATTLC(Edge)	Vertex_edges_g_ATTLC = &Vertex::edges_g;
const PSETF_ATTLC(Edge)		Vertex_in_edges_ATTLC = &Vertex::in_edges;
const PSETFG_ATTLC(Edge)	Vertex_in_edges_g_ATTLC = &Vertex::in_edges_g;
const PSETF_ATTLC(Edge)		Vertex_out_edges_ATTLC = &Vertex::out_edges;
const PSETFG_ATTLC(Edge)	Vertex_out_edges_g_ATTLC = &Vertex::out_edges_g;
const PSETF_ATTLC(Edge)		Vertex_loop_edges_ATTLC = &Vertex::loop_edges;
const PSETFG_ATTLC(Edge)	Vertex_loop_edges_g_ATTLC = &Vertex::loop_edges_g;
/*const GRAPH(Graph)*	Graph_induced_graph = &Graph::induced_graph; */

Graph::Graph() {}

Graph::Graph(const Graph& g)
{	// more efficiently?
	Set_of_piter<Vertex> v_iter(g.vrtices);
	Vertex*	vp;
	while (vp = v_iter.next())
		insert(vp);
	Set_of_piter<Edge> e_iter(g.edgs);
	Edge*	ep;
	while (ep = e_iter.next())
		insert(ep);
}

Graph::~Graph()
{
	Set_of_piter<Edge> e_iter(edgs);
	Edge*	ep;
	while (ep = e_iter.next())
		ep->grphs.remove(this);
	Set_of_piter<Vertex> v_iter(vrtices);
	Vertex*	vp;
	while (vp = v_iter.next())
		vp->grphs.remove(this);
}

Graph&
Graph::operator=(const Graph& g)
{
	if (this != &g) {
		Set_of_piter<Edge> e_iter(edgs);
		Edge*	ep;
		while (ep = e_iter.next())
			ep->grphs.remove(this);
		Set_of_piter<Vertex> v_iter(vrtices);
		Vertex*	vp;
		while (vp = v_iter.next())
			vp->grphs.remove(this);
		edgs.remove_all();
		vrtices.remove_all();
		Set_of_piter<Vertex> v1_iter(g.vrtices);
		while (vp = v1_iter.next())
			insert(vp);
		Set_of_piter<Edge> e1_iter(g.edgs);
		while (ep = e1_iter.next())
			insert(ep);
	}
	return *this;
}

int
Graph::operator==(const Graph& g) const
{
	return edgs == g.edgs && vrtices == g.vrtices;
}

int
Graph::operator!=(const Graph& g) const
{
	return !(*this == g);
}

int
Graph::insert(Vertex* v)
{
#ifdef DEBUG
	check();
#endif
	if (!vrtices.insert(v)) return 0;
	v->grphs.insert(this);
#ifdef DEBUG
	check();
#endif
	return 1;
}

void
Graph::check()
{
#ifndef NDEBUG
	edgs.check();
	vrtices.check();
	Set_of_piter<Vertex> vset_iter(vrtices);
	Vertex*	vp;
	while(vp = vset_iter.next()) {
		vp->check();
		assert(vp->grphs.contains(this));
	}			
	Set_of_piter<Edge> eset_iter(edgs);
	Edge*	ep;
	while(ep = eset_iter.next()) {
		ep->check();
		assert(ep->grphs.contains(this));
	}
#endif
}

void
Vertex::check()
{
#ifndef NDEBUG
	grphs.check();
	Set_of_piter<Graph> gset_iter(grphs);
	Graph*	gp;
	while(gp = gset_iter.next()) {
		assert(gp->vrtices.contains(this));
	}
#endif
}			
			
void
Edge::check()
{
#ifndef NDEBUG
	grphs.check();
	Set_of_piter<Graph> gset_iter(grphs);
	Graph*	gp;
	while(gp = gset_iter.next()) {
		assert(gp->edgs.contains(this));
	}
#endif
}			
			
int
Graph::insert(Edge* e)
{
#ifdef DEBUG
	check();
#endif
	if (!edgs.insert(e)) return 0;
	e->grphs.insert(this);
	insert(e->frnt);
	insert(e->bck);
#ifdef DEBUG
	check();
#endif
	return 1;
}

int
Graph::remove(const Vertex* v1)
{
#ifdef DEBUG
	check();
#endif
	Vertex* v = (Vertex*)v1;
	if (!vrtices.remove(v)) return 0;
	v->grphs.remove(this);	// remove the vertex from the graph
	
	Edge* ep;
	Set_of_piter<Edge> e_iter(v->in_edgs);
	while (ep = e_iter.next())
		remove(ep);
	Set_of_piter<Edge> e_iter2(v->out_edgs);
	while (ep = e_iter2.next())
		remove(ep);
	Set_of_piter<Edge> e_iter3(v->loop_edgs);
	while (ep = e_iter3.next())
		remove(ep);
#ifdef DEBUG
	check();
#endif
	return 1;
}

int
Graph::remove(const Edge* e)
{
#ifdef DEBUG
	check();
#endif
	if (!edgs.remove(e)) return 0;
	((Edge *) e)->grphs.remove(this);
#ifdef DEBUG
	check();
#endif
	return 1;
}

Vertex::Vertex() {} 

Vertex::~Vertex()
{
	Edge* ep;
	Set_of_piter<Edge> e_iter(in_edgs);
	while (ep = e_iter.next())
		ep->clobber_it(ep);
	Set_of_piter<Edge> e_iter2(out_edgs);
	while (ep = e_iter2.next())
		ep->clobber_it(ep);
	Set_of_piter<Edge> e_iter3(loop_edgs);
	while (ep = e_iter3.next())
		ep->clobber_it(ep);

	Graph* gp;
	Set_of_piter<Graph> g_iter(grphs);
	while (gp = g_iter.next())
		gp->remove(this);
}

Set_of_p<Graph>
Vertex::graphs() const
{
	return grphs;
}

Set_of_p<Edge>
Vertex::edges() const
{
	if (loop_edgs)
		return in_edgs | loop_edgs | out_edgs;
	else
		return in_edgs | out_edgs;
}

Set_of_p<Edge>
Vertex::edges_g(const Graph& g) const
{
	Set_of_p<Edge> result;

	Edge* ep;
	Set_of_piter<Edge> e_iter(in_edgs);
	while (ep = e_iter.next())
		if (g.contains(ep))
			result.insert(ep);

	Set_of_piter<Edge> e_iter2(out_edgs);
	while (ep = e_iter2.next())
		if (g.contains(ep))
			result.insert(ep);

	if (loop_edgs) {
		Set_of_piter<Edge> e_iter2(loop_edgs);
		while (ep = e_iter2.next())
			if (g.contains(ep))
				result.insert(ep);
		}
	return (result);
}

Set_of_p<Edge>
Vertex::in_edges() const
{
	if (loop_edgs)
		return in_edgs | loop_edgs;
	else
		return in_edgs;
}

Set_of_p<Edge>
Vertex::in_edges_g(const Graph& g) const
{
	Set_of_p<Edge> result;
	
	Edge* ep;
	Set_of_piter<Edge> e_iter(in_edgs);
	while (ep = e_iter.next())
		if (g.contains(ep))
			result.insert(ep);

	if (loop_edgs) {
		Set_of_piter<Edge> e_iter2(loop_edgs);
		while (ep = e_iter2.next())
			if (g.contains(ep))
				result.insert(ep);
	}
	return (result);
}

Set_of_p<Edge>
Vertex::out_edges() const
{
	if (loop_edgs)
		return out_edgs | loop_edgs;
	else
		return out_edgs;
}

Set_of_p<Edge>
Vertex::out_edges_g(const Graph& g) const
{
	Set_of_p<Edge> result;
	
	Edge* ep;
	Set_of_piter<Edge> e_iter(out_edgs);
	while (ep = e_iter.next())
		if (g.contains(ep))
			result.insert(ep);

	if (loop_edgs) {
		Set_of_piter<Edge> e_iter2(loop_edgs);
		while (ep = e_iter2.next())
			if (g.contains(ep))
				result.insert(ep);
	}
	return (result);
}

Set_of_p<Edge>
Vertex::loop_edges() const
{
	return loop_edgs;
}

Set_of_p<Edge>
Vertex::loop_edges_g(const Graph& g) const
{
	Set_of_p<Edge> result;
	
	Edge* ep;
	Set_of_piter<Edge> e_iter(loop_edgs);
	while (ep = e_iter.next())
		if (g.contains(ep))
			result.insert(ep);
	return (result);
}

Edge::~Edge()
{
	clobber();
}

void
Edge::clobber_it(Edge* ep)	// when one of the vertices is destroyed
{
	/* remove the edge from all graphs */
	Graph* gp;
	Set_of_piter<Graph> g_iter(ep->grphs);
	while (gp = g_iter.next())
		gp->remove(ep);

	if (ep->frnt) {
		if (ep->frnt == ep->bck) {
			ep->frnt->loop_edgs.remove(ep);
			ep->bck = 0;
		}
		else
			ep->frnt->in_edgs.remove(ep);
		ep->frnt = 0;
	}
	if (ep->bck)
	{
		ep->bck->out_edgs.remove(ep);
		ep->bck = 0;
	}
}

Edge::Edge(Vertex* src, Vertex* dst) : bck(src), frnt(dst)
{
	if (bck == frnt)
		bck->loop_edgs.insert(this);
	else {
		bck->out_edgs.insert(this);
		frnt->in_edgs.insert(this);
	}
}

Edge::Edge(const Edge& e) : bck(e.bck), frnt(e.frnt)
{
	if (bck == frnt)
		bck->loop_edgs.insert(this);
	else {
		bck->out_edgs.insert(this);
		frnt->in_edgs.insert(this);
	}

	Graph* gp;
	Set_of_piter<Graph> g_iter(e.grphs);
	while (gp = g_iter.next())
		gp->insert(this);
}

Edge&
Edge::operator=(const Edge& e)
{
	if (&e == this)
		return *this;
	clobber();
	bck = e.bck;
	frnt = e.frnt;
	if (bck == frnt)
		bck->loop_edgs.insert(this);
	else {
		bck->out_edgs.insert(this);
		frnt->in_edgs.insert(this);
	}

	Graph* gp;
	Set_of_piter<Graph> g_iter(e.grphs);
	while (gp = g_iter.next())
		gp->insert(this);
	return *this;
}

/*
Edge*
Edge_iterator::first()
{
	if (iterp)
		delete iterp;
	if (vp)
	{
		switch (pos = io)
		{
		case ALL:
		case IN:
			iterp = new Set_of_p_iterator<Edge>(vp->in_edgs);
			break;
		case OUT:
			iterp = new Set_of_p_iterator<Edge>(vp->out_edgs);
			break;
		case LOOP:
			iterp = new Set_of_p_iterator<Edge>(vp->loop_edgs);
			break;
		}
	}
	else
		iterp = new Set_of_p_iterator<Edge>(gp->edgs);
	return next();
}

Edge*
Edge_iterator::next()
{
	if (iterp == 0) return first();
	Edge*	ep;
	for (;;)
	{
		while (ep = iterp->next())
			if (vp == 0 || gp == 0 || gp->contains(ep))
				return ep;
		if (vp == 0 || pos == LOOP) return 0;
		delete iterp;
		if (pos == ALL ) {
			iterp = new Set_of_p_iterator<Edge>(vp->out_edgs);
			pos = OUT;
		}
		else {
			iterp = new Set_of_p_iterator<Edge>(vp->loop_edgs);
			pos = LOOP;
		}
	}
}
*/		
		

#ifdef DEBUG
	check();
#endif
/*
Graph 
Graph::induced_graph(const Set_of_p<Vertex>& v0_pset)
{
	Set_of_p<Vertex> v_pset = v0_pset;
	Graph g;
	Set_of_piter<Vertex> vset_iter(v_pset);
	Vertex* vp;							
	while (vp = vset_iter.next()) {
		if (contains(vp)) {
			g.insert(vp);
			Set_of_p<Edge> spe1 = vp->in_edges_g(*this);
			Set_of_piter<Edge> e1set_iter(spe1);
			Edge* ep;
			while (ep = e1set_iter.next())
				if (v_pset.contains(ep->src()))
					g.insert(ep);
					
			Set_of_p<Edge> spe2 = vp->out_edges_g(*this);
			Set_of_piter<Edge> e2set_iter(spe2);
			while (ep = e2set_iter.next())
				if (v_pset.contains(ep->dst()))
					g.insert(ep);
					
			Set_of_p<Edge> spe3 = vp->loop_edges_g(*this);
			Set_of_piter<Edge> e3set_iter(spe3);
			while (ep = e3set_iter.next())
				g.insert(ep);
			v_pset.remove(vp); 	//shrinks set over which to inspect	
		}
	}
	return(g);
}
*/

Graph::Graph(const Graph& g, const Set_of_p<Vertex>& v0_pset)
{
	Set_of_p<Vertex> v_pset = v0_pset;
	fill_graph(g, v_pset);
}

void
Graph::fill_graph(const Graph& orig_g, Set_of_p<Vertex>& v_pset)
{
	Set_of_piter<Vertex> vset_iter(v_pset);
	Vertex* vp;
	while (vp = vset_iter.next()) {
		if (orig_g.contains(vp)) {
			insert(vp);
			Set_of_p<Edge> spe1 = vp->in_edges_g(orig_g);
			Set_of_piter<Edge> e1set_iter(spe1);
			Edge* ep;
			while (ep = e1set_iter.next())
				if (v_pset.contains(ep->src()))
					insert(ep);
			Set_of_p<Edge> spe2 = vp->out_edges_g(orig_g);
			Set_of_piter<Edge> e2set_iter(spe2);
			while (ep = e2set_iter.next())
				if (v_pset.contains(ep->dst()))
					insert(ep);
			Set_of_p<Edge> spe3 = vp->loop_edges_g(orig_g);
			Set_of_piter<Edge> e3set_iter(spe3);
			while (ep = e3set_iter.next())
				insert(ep);
			v_pset.remove(vp); 	//shrinks set over which to inspect
		}
	}
}


Vis_v_ticket
Vertex::get_vis_v_ticket()
{
	Vis_v_ticket vt;
	vt.validate();
	return (vt);
}

Val_v_ticket
Vertex::get_val_v_ticket()
{
	Val_v_ticket vt;
	vt.validate();
	return (vt);
}

void
Vertex::free_vis_v_ticket(Vis_v_ticket& vt)
{
	vt.invalidate();
}

void
Vertex::free_val_v_ticket(Val_v_ticket& vt)
{
	vt.invalidate();
}

Vis_e_ticket
Edge::get_vis_e_ticket()
{
	Vis_e_ticket vt;
	vt.validate();
	return (vt);
}

Val_e_ticket
Edge::get_val_e_ticket()
{
	Val_e_ticket vt;
	vt.validate();
	return (vt);
}

void
Edge::free_vis_e_ticket(Vis_e_ticket& vt)
{
	vt.invalidate();
}

void
Edge::free_val_e_ticket(Val_e_ticket& vt)
{
	vt.invalidate();
}

int
Vertex::set_visited()
{
	if (!vis_v) {
		vis_v = 1;
		vis_v_default = get_vis_v_ticket();
	}
	return (set_visited(vis_v_default));
}

int
Vertex::set_visited(const Vis_v_ticket& vt)
{
	if (vt.num >= 0) {
		while (visit.size() <= vt.num)
			visit.size(visit.size() + bits_increment);
		int temp = visit[vt.num];
		visit.set(vt.num);
		return(temp);
	}
	else
		return(vt.num); //not validated
}

int
Edge::set_visited()
{
	if (!vis_e) {
		vis_e = 1;
		vis_e_default = get_vis_e_ticket();
	}
	return (set_visited(vis_e_default));
}

int
Edge::set_visited(const Vis_e_ticket& vt)
{
	if (vt.num >= 0) {
		while (visit.size() <= vt.num)
			visit.size(visit.size() + bits_increment);
		int temp = visit[vt.num];
		visit.set(vt.num);
		return(temp);
	}
	else
		return(vt.num); //not validated
}

int
Vertex::reset_visited()
{
	if (!vis_v) {
		vis_v = 1;
		vis_v_default = get_vis_v_ticket();
	}
	return (reset_visited(vis_v_default));
}

int
Vertex::reset_visited(const Vis_v_ticket& vt)
{
	if (vt.num >= 0) {
		while (visit.size() <= vt.num)
			visit.size(visit.size() + bits_increment);
		int temp = visit[vt.num];
		visit.reset(vt.num);
		return(temp);
	}
	else
		return(vt.num); //not validated
}

int
Edge::reset_visited()
{
	if (!vis_e) {
		vis_e = 1;
		vis_e_default = get_vis_e_ticket();
	}
	return (reset_visited(vis_e_default));
}

int
Edge::reset_visited(const Vis_e_ticket& vt)
{
	if (vt.num >= 0) {
		while (visit.size() <= vt.num)
			visit.size(visit.size() + bits_increment);
		int temp = visit[vt.num];
		visit.reset(vt.num);
		return(temp);
	}
	else
		return(vt.num); //not validated
}

int
Vertex::set_val(int i)
{
	if (!val_v) {
		val_v = 1;
		val_v_default = get_val_v_ticket();
	}
	return (set_val(val_v_default, i));
}

int
Vertex::set_val(const Val_v_ticket& vt, int i)
{
	if (vt.num >= 0) {
		if (value.size() <= vt.num)
			value.size(vt.num+1); //to allow 0 index
		int temp = value[vt.num];
		value[vt.num] = i;
		return(temp);
	}
	else
		return(vt.num); //not validated
}

int
Edge::set_val(int i)
{
	if (!val_e) {
		val_e = 1;
		val_e_default = get_val_e_ticket();
	}
	return(set_val(val_e_default, i));
}

int
Edge::set_val(const Val_e_ticket& vt, int i)
{
	if (vt.num >= 0) {
		if (value.size() <= vt.num)
			value.size(vt.num+1); //to allow 0 index
		int temp = value[vt.num];
		value[vt.num] = i;
		return(temp);
	}
	else
		return(vt.num); //not validated
}

int
Vertex::visited()
{
	if (!vis_v) {
		vis_v = 1;
		vis_v_default = get_vis_v_ticket();
	}
	return (visited(vis_v_default));
}

int
Vertex::visited(const Vis_v_ticket& vt)
{
	while (visit.size() <= vt.num)
		visit.size(visit.size() + bits_increment);
	return (visit[vt.num]);
}

int
Edge::visited()
{
	if (!vis_e) {
		vis_e = 1;
		vis_e_default = get_vis_e_ticket();
	}
	return (visited(vis_e_default));
}

int
Edge::visited(const Vis_e_ticket& vt)
{
	while (visit.size() <= vt.num)
		visit.size(visit.size() + bits_increment);
	return (visit[vt.num]);
}

int
Vertex::val()
{
	if (!val_v) {
		val_v = 1;
		val_v_default = get_val_v_ticket();
	}
	return (val(val_v_default));
}

int
Vertex::val(const Val_v_ticket& vt)
{
	if (value.size() <= vt.num)
		value.size(vt.num + 1); //to allow 0 index
	return (value[vt.num]);
}

int
Edge::val()
{
	if (!val_e) {
		val_e = 1;
		val_e_default = get_val_e_ticket();
	}
	return (val(val_e_default));
}

int
Edge::val(const Val_e_ticket& vt)
{
	if (value.size() <= vt.num)
		value.size(vt.num + 1); //to allow 0 index
	return (value[vt.num]);
}

void
reset_visited(Set_of_p<Vertex>& vpset)
{
	if (!vis_v) {
		vis_v = 1;
		vis_v_default = Vertex::get_vis_v_ticket();
	}
	reset_visited(vis_v_default, vpset);
}

void 
reset_visited(const Vis_v_ticket& vt, Set_of_p<Vertex>& vpset)
{
	Set_of_piter<Vertex> vset_iter(vpset);
	Vertex* vp;
	while (vp = vset_iter.next()) //reset visited bits
		vp->reset_visited(vt);
}

void
reset_visited(Set_of_p<Edge>& epset)
{
	if (!vis_e) {
		vis_e = 1;
		vis_e_default = Edge::get_vis_e_ticket();
	}
	reset_visited(vis_e_default, epset);
}


void 
reset_visited(const Vis_e_ticket& vt, Set_of_p<Edge>& epset)
{
	Set_of_piter<Edge> eset_iter(epset);
	Edge* ep;
	while (ep = eset_iter.next()) //reset visited bits
		ep->reset_visited(vt);
}

void
reset_val(Set_of_p<Vertex>& vpset)
{
	if (!val_v) {
		val_v = 1;
		val_v_default = Vertex::get_val_v_ticket();
	}
	reset_val(val_v_default, vpset);
}


void 
reset_val(const Val_v_ticket& vt, Set_of_p<Vertex>& vpset)
{
	Set_of_piter<Vertex> vset_iter(vpset);
	Vertex* vp;
	while (vp = vset_iter.next()) //reset val
		vp->set_val(vt, 0);
}

void
reset_val(Set_of_p<Edge>& epset)
{
	if (!val_e) {
		val_e = 1;
		val_e_default = Edge::get_val_e_ticket();
	}
	reset_val(val_e_default, epset);
}

void 
reset_val(const Val_e_ticket& vt, Set_of_p<Edge>& epset)
{
	Set_of_piter<Edge> eset_iter(epset);
	Edge* ep;
	while (ep = eset_iter.next()) //reset val
		ep->set_val(vt, 0);
}
