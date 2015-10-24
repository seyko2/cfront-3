#include "Link.h"

Link::~Link() {}

void Link::display() { fprintf(dbfile, "Link"); }

static int indentation = 0;
void
newline()
{
	fprintf(dbfile, "\n");
	for ( int i = 0;  i < indentation;  ++i ) fprintf(dbfile, "  ");
}
void
indent( int i )
{
	indentation += i;
}

void
undent( int i )
{
	indentation -= i;
}

void
Chain::display()
{
	fprintf(dbfile, "Chain[%d] {", len);
	++indentation;
	for ( Link* o = h;  o;  o = o->next ) {
		newline();
		o->display();
	}
	--indentation;
	newline();
	fprintf(dbfile, "}");
}
