/*ident	"@(#)Graph:incl/Graph.h	3.1" */
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

#ifndef GRAPH_DOT_H
#define GRAPH_DOT_H

#include <Set.h>
#include <Bits.h>
#include <Block.h>
#include <Ticket.h>

class Edge;
class Vertex;

class Graph {
	friend class Vertex;
	friend class Edge;

	Set_of_p<Edge>	 edgs;
	Set_of_p<Vertex> vrtices;

protected:
	Graph(const Graph&, const Set_of_p<Vertex>&);
	void 	fill_graph(const Graph&, Set_of_p<Vertex>&);

public:
	Graph();
	Graph(const Graph&);
	~Graph();

	Graph&	operator=(const Graph&);
	int	operator==(const Graph&) const;
	int	operator!=(const Graph&) const;
	const Set_of_p<Edge>&	edges() const { return edgs; }
	const Set_of_p<Vertex>&	vertices() const { return vrtices; }
	int	insert(Vertex*);
	int	insert(Edge*);
	int	remove(const Vertex*);
	int	remove(const Edge*);
	int	contains(const Vertex* x) const {
			return (vrtices.contains(x) != NULL);
		}
	int	contains(const Edge* x) const {
			return (edgs.contains(x) != NULL);
		}
	Graph	induced_graph(const Set_of_p<Vertex>& vs) const {
			return Graph(*this, vs);
		}

	void	check();
};


class Vertex {
	friend class Graph;
	friend class Edge;

	Set_of_p<Graph>	grphs;
	Set_of_p<Edge>	in_edgs;
	Set_of_p<Edge>	out_edgs;
	Set_of_p<Edge>	loop_edgs;

	Bits		visit;
	Block<int>	value;

	Vertex(const Vertex&);
	Vertex&	operator=(const Vertex&);

public:
	Vertex();
	~Vertex();

	Set_of_p<Graph>	graphs() const;
	Set_of_p<Edge>	edges() const;
	Set_of_p<Edge>	edges_g(const Graph&) const;
	Set_of_p<Edge>	in_edges() const;
	Set_of_p<Edge>	in_edges_g(const Graph&) const;
	Set_of_p<Edge>	out_edges() const;
	Set_of_p<Edge>	out_edges_g(const Graph&) const;
	Set_of_p<Edge>	loop_edges() const;
	Set_of_p<Edge>	loop_edges_g(const Graph&) const;

	static Vis_v_ticket	get_vis_v_ticket();
	static Val_v_ticket	get_val_v_ticket();
	static void		free_vis_v_ticket(Vis_v_ticket&);
	static void		free_val_v_ticket(Val_v_ticket&);

	int	set_visited();
	int	set_visited(const Vis_v_ticket&);
	int	reset_visited();
	int	reset_visited(const Vis_v_ticket&);
	int	set_val(int);
	int	set_val(const Val_v_ticket&, int);
	int	visited();
	int	visited(const Vis_v_ticket&);
	int	val();
	int	val(const Val_v_ticket&);
	int	in_graph(const Graph& g) const {
			return (grphs.contains(&g) != NULL);
		}

	void	check();
};

void	reset_visited(Set_of_p<Vertex>&);
void 	reset_visited(const Vis_v_ticket&, Set_of_p<Vertex>&);
void	reset_val(Set_of_p<Vertex>&);
void	reset_val(const Val_v_ticket&, Set_of_p<Vertex>&);


class Edge {
	friend class Graph;
	friend class Vertex;

	Set_of_p<Graph>	grphs;
	Vertex*		frnt;
	Vertex*		bck;

	Bits		visit;
	Block<int>	value;

	static void	clobber_it(Edge* ep);	// when a vertex is destroyed
	void		clobber() { clobber_it(this); }

public:
	Edge(Vertex* src, Vertex* dst);
	Edge(const Edge&);
	~Edge(); 

	Edge&	operator=(const Edge&);

	Vertex*	src() const { return bck; }
	Vertex*	dst() const { return frnt; }
	const Set_of_p<Graph>&	graphs() const { return grphs; }
	int	in_graph(const Graph& g) const {
			return (grphs.contains(&g) != NULL);
		}

	static Vis_e_ticket	get_vis_e_ticket();
	static Val_e_ticket	get_val_e_ticket();
	static void		free_vis_e_ticket(Vis_e_ticket&);
	static void		free_val_e_ticket(Val_e_ticket&);

	int	set_visited();
	int	set_visited(const Vis_e_ticket&);
	int	reset_visited();
	int	reset_visited(const Vis_e_ticket&);
	int	set_val(int);
	int	set_val(const Val_e_ticket&, int);
	int	visited();
	int	visited(const Vis_e_ticket&);
	int	val();
	int	val(const Val_e_ticket&);

	void	check();
};

void		reset_visited(Set_of_p<Edge>&);
void 		reset_visited(const Vis_e_ticket&, Set_of_p<Edge>&);
void		reset_val(Set_of_p<Edge>&);
void		reset_val(const Val_e_ticket&, Set_of_p<Edge>&);

enum InOutLoop_ATTLC { IN, OUT, LOOP, ALL };

#include <generic.h>
#define PSETF_ATTLC(X) name2(X,_PSETF_ATTLC)
#define PSETFG_ATTLC(X) name2(X,_PSETFG_ATTLC)


#define graphtypedefs_ATTLCdeclare(G,V,E) \
 \
typedef Set_of_p<G> (Vertex::*PSETF_ATTLC(G))() const; \
typedef Set_of_p<E> (Vertex::*PSETF_ATTLC(E))() const; \
typedef Set_of_p<E> (Vertex::*PSETFG_ATTLC(E))(const Graph&) const; \
 \


#define Graphdeclare1(G,V,E) \
 \
class G; \
class V; \
class E; \
graphtypedefs_ATTLCdeclare(G,V,E) \
 \


graphtypedefs_ATTLCdeclare(Graph,Vertex,Edge)

extern const PSETF_ATTLC(Graph)		Vertex_graphs_ATTLC;
extern const PSETF_ATTLC(Edge)		Vertex_edges_ATTLC;
extern const PSETFG_ATTLC(Edge)		Vertex_edges_g_ATTLC;
extern const PSETF_ATTLC(Edge)		Vertex_in_edges_ATTLC;
extern const PSETFG_ATTLC(Edge)		Vertex_in_edges_g_ATTLC;
extern const PSETF_ATTLC(Edge)		Vertex_out_edges_ATTLC;
extern const PSETFG_ATTLC(Edge)		Vertex_out_edges_g_ATTLC;
extern const PSETF_ATTLC(Edge)		Vertex_loop_edges_ATTLC;
extern const PSETFG_ATTLC(Edge)		Vertex_loop_edges_g_ATTLC;

#define Graphimplement(G,V,E)

#define derivedGraph(G,V,E) \
 \
protected: \
	inline G(const G&, const Set_of_p<V>&); \
	inline void	 fill_graph(const G&, Set_of_p<V>&); \
public: \
	inline const Set_of_p<E>&	edges() const; \
	inline const Set_of_p<V>&	vertices() const; \
	inline int	insert(V*); \
	inline int	insert(E*); \
	inline int	remove(V*); \
	inline int	remove(E*); \
	inline G	induced_graph(const Set_of_p<V>& vp); \
 \


#define derivedVertex(G,V,E) \
 \
public: \
	inline Set_of_p<G>	graphs() const; \
	inline Set_of_p<E>	edges() const; \
	inline Set_of_p<E>	edges_g(const G& g) const; \
	inline Set_of_p<E>	in_edges() const; \
	inline Set_of_p<E>	in_edges_g(const G& g) const; \
	inline Set_of_p<E>	out_edges() const; \
	inline Set_of_p<E>	out_edges_g(const G& g) const; \
	inline Set_of_p<E>	loop_edges() const; \
	inline Set_of_p<E>	loop_edges_g(const G& g) const; \
 \


#define derivedEdge(G,V,E) \
 \
	V*	src() const { return (V*)Edge::src(); } \
	V*	dst() const { return (V*)Edge::dst(); } \
	const Set_of_p<G>&	graphs() const { \
		return (const Set_of_p<G>&)Edge::graphs(); \
	} \
 \


#define Graphdeclare2(G,V,E) \
 \
inline void	reset_visited(Set_of_p<V>& vpset) { \
		    reset_visited((Set_of_p<Vertex>&)vpset); \
		} \
inline void	reset_val(Set_of_p<V>& vpset) { \
		    reset_val((Set_of_p<Vertex>&)vpset); \
		} \
inline void	reset_visited(Set_of_p<E>& epset) { \
		    reset_visited((Set_of_p<Edge>&)epset); \
		} \
inline void	reset_val(Set_of_p<E>& epset) { \
		    reset_val((Set_of_p<Edge>&)epset); \
		} \
inline void	reset_visited(const Vis_v_ticket& vt, Set_of_p<V>& vpset) { \
		    reset_visited(vt, (Set_of_p<Vertex>&)vpset); \
		} \
inline void	reset_val(const Val_v_ticket& vt, Set_of_p<V>& vpset) { \
		    reset_val(vt, (Set_of_p<Vertex>&)vpset); \
		} \
inline void	reset_visited(const Vis_e_ticket& et, Set_of_p<E>& epset) { \
		    reset_visited(et, (Set_of_p<Edge>&)epset); \
		} \
inline void	reset_val(const Val_e_ticket& et, Set_of_p<E>& epset) { \
		    reset_val(et, (Set_of_p<Edge>&)epset); \
		} \
inline void	G::fill_graph(const G& g, Set_of_p<V>& vpset) { \
		    Graph::fill_graph((const Graph&)g, (Set_of_p<Vertex>&)vpset); \
		} \
inline  	G::G(const G& g, const Set_of_p<V>& vs) \
		: Graph((const Graph&)g, (const Set_of_p<Vertex>&)vs) {} \
inline G	G::induced_graph(const Set_of_p<V>& vs) { \
		    return G(*this, vs); \
		} \
 \
inline const Set_of_p<E>&	G::edges() const { \
		    return (const Set_of_p<E>&)Graph::edges(); \
		} \
inline const Set_of_p<V>&	G::vertices() const { \
		    return (Set_of_p<V>&)Graph::vertices(); \
		} \
inline int	G::insert(V* x) { return Graph::insert(x); } \
inline int	G::insert(E* x) { return Graph::insert(x); } \
inline int	G::remove(V* x) { return Graph::remove(x); } \
inline int	G::remove(E* x) { return Graph::remove(x); } \
inline Set_of_p<G>	V::graphs() const { \
		    return (this->*((PSETF_ATTLC(G))Vertex_graphs_ATTLC))(); \
		} \
inline Set_of_p<E>	V::edges() const { \
		    return (this->*((PSETF_ATTLC(E))Vertex_edges_ATTLC))(); \
		} \
inline Set_of_p<E>	V::edges_g(const G& g) const { \
		    return (this->*((PSETFG_ATTLC(E))Vertex_edges_g_ATTLC))(g); \
		} \
inline Set_of_p<E>	V::in_edges() const { \
		    return (this->*((PSETF_ATTLC(E))Vertex_in_edges_ATTLC))(); \
		} \
inline Set_of_p<E>	V::in_edges_g(const G& g) const { \
		    return (this->*((PSETFG_ATTLC(E))Vertex_in_edges_g_ATTLC))(g); \
		} \
inline Set_of_p<E>	V::out_edges() const { \
		    return (this->*((PSETF_ATTLC(E))Vertex_out_edges_ATTLC))(); \
		} \
inline Set_of_p<E>	V::out_edges_g(const G& g) const { \
		    return (this->*((PSETFG_ATTLC(E))Vertex_out_edges_g_ATTLC))(g); \
		} \
inline Set_of_p<E>	V::loop_edges() const { \
		    return (this->*((PSETF_ATTLC(E))Vertex_loop_edges_ATTLC))(); \
		} \
inline Set_of_p<E>	V::loop_edges_g(const G& g) const { \
		    return (this->*((PSETFG_ATTLC(E))Vertex_loop_edges_g_ATTLC))(g); \
		} \
 \




#endif
