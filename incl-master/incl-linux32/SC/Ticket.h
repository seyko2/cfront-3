/*ident	"@(#)Graph:incl/Ticket.h	3.1" */
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

#ifndef TICKET_DOT_H
#define TICKET_DOT_H

class Vertex;
class Edge;
class Bits;

class Ticket_ATTLC {
	friend class Vertex;
	friend class Edge;

	static const int bits_increment;
	int num;
protected:
	void Validate(Bits&);
	void Invalidate(Bits&);
public:
	Ticket_ATTLC() : num(-1) {}
	Ticket_ATTLC& operator=(const Ticket_ATTLC& vt) {
		num = vt.num;
		return *this;
	}
	virtual void validate() = 0;
	virtual void invalidate() = 0;
	~Ticket_ATTLC() {}
};

class Vis_v_ticket : public Ticket_ATTLC {
	friend class Vertex;

	static Bits b;
public:
	Vis_v_ticket() {}
	~Vis_v_ticket() {}
	void validate();
	void invalidate();
};

class Vis_e_ticket : public Ticket_ATTLC {
	friend class Edge;

	static Bits b;
public:
	Vis_e_ticket() {}
	~Vis_e_ticket() {}
	void validate();
	void invalidate();
};

class Val_v_ticket : public Ticket_ATTLC {
	friend class Vertex;

	static Bits b;
public:
	Val_v_ticket() {}
	~Val_v_ticket() {}
	void validate();
	void invalidate();
};

class Val_e_ticket : public Ticket_ATTLC {
	friend class Edge;

	static Bits b;
public:
	Val_e_ticket() {}
	~Val_e_ticket() {}
	void validate();
	void invalidate();
};

#endif
