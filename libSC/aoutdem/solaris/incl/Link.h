#include "idebug.h"

/*
 *  Link base classes and Chain utilities
 */

// simple object and one-way linked list
struct Link {
	Link *next;
	Link() : next(0) {}
	virtual ~Link();
	virtual void display();
};

class Chain : public Link {
	Link *h, *t;
	int len;
public:
	Chain() : h(0), t(0), len(0) {}
	void operator>>=( Link *o )
	{
		if ( h == 0 ) h = o; else t->next = o;
		o->next = 0;  t = o;  ++len;
	}
	void operator<<=( Link *o )
	{
		if ( h == 0 ) t = o;
		o->next = h;  h = o;  ++len;
	}
	void display();
	int   length() { return len; }
	Link* head() { return h; }
	Link* tail() { return t; }
	// add/delete element after l; l must be on list
	// if l==0, insert/delete at head of list
	void addnext( Link *l, Link *o ) {
		if ( l==0 ) *this <<= o;
		else {
			if ( l == t ) t = o;
			o->next = l->next; l->next = o; ++len;
		}
	}
	void delnext( Link* l ) {
		if ( l==0 ) {
			h = h->next; --len;
			if ( len == 0 ) t = 0;
		}
		else if ( l != t ) {
			if ( l->next == t ) t = l;
			l->next = l->next->next;
			--len;
		}
	}
};

extern void newline();
extern void indent( int = 1 );
extern void undent( int = 1 );
