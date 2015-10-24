#include <string.h>
#include "Memspace.h"

extern "C" char* strerror(int);

#ifdef IDEBUG
#define DB_PRINT(a,b) {if(a){b;}}
#else
#define DB_PRINT(a,b) /**/
#endif

const int MINSEG = 1; // minimum size of mem segment (avoid fragmentation)

class Memseg : public Link { // chunk of memory within a contiguous area
private:
	friend class Memspace;
	long m_offset;
	long m_size;
	enum { m_chunk = 16 };
	static Memseg* m_free;
	void* operator new(size_t);
	void operator delete(void*,size_t);
public:
	Memseg( long o, long sz ) : m_offset(o), m_size(sz) {}
	void display(); // virtual
};

Memseg* Memseg::m_free;

void*
Memseg::operator new(size_t sz)
{
	register Memseg* p;

	if ( (p=m_free) == 0 ) {
		register Memseg* q = (Memseg*) new char[m_chunk*sz];
		for (p=m_free=&q[m_chunk-1]; q<p; p--)
			p->next = p-1;
		(p+1)->next = 0;
	}
	else {
		m_free = (Memseg*)p->next;
	}
	return p;
}

void
Memseg::operator delete(void* vp, size_t)
{
	Memseg* p = (Memseg*)vp;
	p->next = m_free;
	m_free = p;
	vp = 0;
}

Memspace::Memspace( long sz, char* n, unsigned long flags )
{
	// initial space contains 1 free seg covering the whole area
	M_name = n;
	M_size = sz;
	M_seg >>= new Memseg( 0, sz );
	M_flags = flags;
}

Memspace::~Memspace() {}

#ifdef IDEBUG
void
Memspace::set_debug( int i )
{
	M_debug = i;
	DB_PRINT(M_debug>=1,fprintf(dbfile,
		"\ndebugging Memspace '%s', size 0x%.8lx\n",M_name,M_size));
}
#endif

long
Memspace::allocseg( long sz )
{
	// use first-fit
	DB_PRINT( M_debug>=1,
		fprintf(dbfile,"\n'%s'->allocseg( 0x%.8lx )\n",M_name,sz);
		if( M_debug>=2 ) print();
	);
	Memseg *m = (Memseg*)M_seg.head(), *l=0;
	for ( ; m; l=m, m=(Memseg*)m->next ) {
		if ( m->m_size >= sz ) {
			long rr;
			if ( (m->m_size-sz) >= MINSEG ) {
				m->m_offset += sz;
				m->m_size -= sz;
				rr = m->m_offset - sz;
			} else {
				// delete this chunk
				M_seg.delnext(l);
				rr = m->m_offset;
				delete m;
			}
			DB_PRINT( M_debug>=2,
				fprintf(dbfile, " -- returning offset 0x%.8lx\n",rr);
				print();
			);
			return rr;
		}
	}
	if ( (M_flags & M_dynamic) && grow(sz) ) {
		static int r = 0;
		if ( r++ > 0 ) { // sanity check
			fprintf(stderr, "OOPS! -- %d recursions of Memspace::allocseg()\n",r-1);
			print();
			DB_PRINT(M_debug>=1, fprintf(dbfile,"OOPS! -- more than one recursion of Memspace::allocseg()\n");print(););
			exit(1);
		} else {
			--r;
			return allocseg(sz);
		}
	} else {
		fprintf(stderr,"can't allocate 0x%.8lx units in Memspace '%s', sz 0x%.8lx\n",sz,M_name,M_size);
		DB_PRINT(M_debug>=1,fprintf(dbfile,"can't allocate 0x%.8lx units in Memspace '%s', sz 0x%.8lx\n",sz,M_name,M_size););
	}
	return -1;
}

#if 0
long
Memspace::placeseg( long off, long sz )
{
	// use first-fit
	DB_PRINT( M_debug>=1,
		fprintf(dbfile,"\n'%s'->placeseg( 0x%.8lx, 0x%.8lx )\n",M_name,off,sz);
		if( M_debug>=2 ) print();
	);
	Memseg *m = (Memseg*)M_seg.head(), *l=0;
	for ( ; m; l=m, m=(Memseg*)m->next ) {
		if ( m->m_offset + m->m_size < off ) continue;
		if ( m->m_offset > off ) break;
		if ( (m->m_offset + m->m_size - off) < sz ) break;
		long d1 = off - m->m_offset;
		long d2 = m->m_offset + m->m_size - (off + sz);
		if ( d1 == 0 ) {
			if ( d2 == 0 ) { M_seg.delnext(l); delete m; }
			else { m->m_offset = off + sz; m->m_size = d2; }
		} else if ( d2 == 0 ) {
			if ( d1 == 0 ) { M_seg.delnext(l); delete m; }
			else m->m_size = d1;
		} else { // fragment
			Memseg *n = new Memseg(off+sz,d2);
			m->m_size = d1;
			M_seg.addnext(m,n);
		}
		return off;
	}
	fprintf(stderr,"can't place 0x%.8lx units at offset 0x%.8lx in Memspace '%s', sz 0x%.8lx\n",sz,off,M_name,M_size);
	DB_PRINT(M_debug>=1,fprintf(dbfile,"can't place 0x%.8lx units at offset 0x%.8lx in Memspace '%s', sz 0x%.8lx\n",sz,off,M_name,M_size););
	return -1;
}
#endif

void
Memspace::freeseg( long o, long sz )
{
	DB_PRINT( M_debug>=1,
		fprintf(dbfile,"\n'%s'->freeseg( 0x%.8lx, 0x%.8lx )\n",M_name,o,sz);
		if( M_debug>=2 ) { print(); newline(); }
	);
	// deallocate and merge adjacent free space
	if ( o < 0 || (o+sz) >= M_size ) {
		fprintf(stderr,"bad args to freeseg( 0x%.8lx, 0x%.8lx ) for Memspace '%s'",o,sz,M_name);
		DB_PRINT(M_debug>=1,fprintf(dbfile,"bad args to freeseg( 0x%.8lx, 0x%.8lx ) for Memspace '%s'",o,sz,M_name););
		return;
	}
	if ( sz == 0 ) return;
	Memseg *m = (Memseg*)M_seg.head(), *l=0;
	for ( ; m; l=m, m=(Memseg*)m->next )
		if ( o < m->m_offset ) break;
	// m is first free mem seg with offset > o
	if ( m ) {
		// sanity check
		if ( (o+sz) > m->m_offset ) {
			fprintf(stderr,"deallocating unallocated memory in memspace '%s' (o==0x%.8lx,sz==0x%.8lx)",M_name,o,sz);
			DB_PRINT(M_debug>=1,fprintf(dbfile,"deallocating unallocated memory in memspace '%s' (o==0x%.8lx,sz==0x%.8lx)",M_name,o,sz););
			return;
		}
		// merge if adjacent
		if ( (o+sz) == m->m_offset ) {
			sz += m->m_size;
			M_seg.delnext(l); // m
			delete m;
			m = 0;
		}
	}
	// try to merge with previous seg
	if ( l ) {
		// sanity check
		if ( (l->m_offset+l->m_size) > o ) {
			fprintf(stderr,"deallocating unallocated memory in memspace '%s' (o==0x%.8lx,sz==0x%.8lx)",M_name,o,sz);
			DB_PRINT(M_debug>=1,fprintf(dbfile,"deallocating unallocated memory in memspace '%s' (o==0x%.8lx,sz==0x%.8lx)",M_name,o,sz););
			return;
		}
		// merge if adjacent
		if ( (l->m_offset+l->m_size) == o ) {
			l->m_size += sz;
			DB_PRINT( M_debug>=2,
				fprintf(dbfile,"merged segment "); l->display();
				newline(); print(); newline();
			);
			return;
		}
	}
	// add new free mem seg
	m = new Memseg(o,sz);
	M_seg.addnext(l,m);
	DB_PRINT( M_debug>=2, fprintf(dbfile,"adding ");
		m->display(); newline(); print(); newline());
}

int
Memspace::grow( long sz )
{
	Memseg* m = (Memseg*)M_seg.tail();
	DB_PRINT( M_debug>=1,
		fprintf(dbfile,"\n'%s'->grow( 0x%.8lx )\n",M_name,sz);
		if( M_debug>=2 ) { print(); fprintf(dbfile,"\ntail== "); m->display(); }
	);
	if ( sz < MINSEG ) sz = MINSEG;
	if ( (m->m_offset + m->m_size) == M_size )
		m->m_size += sz; // increase size of last free segment
	else
		M_seg >>= new Memseg(M_size,sz); // add new free segment at end
	M_size += sz;
	DB_PRINT( M_debug>=1,
		fprintf(dbfile,"\nafter '%s'->grow( 0x%.8lx )\n",M_name,sz);
		if( M_debug>=2 ) print();
	);
	return 1;
}

#if 0
long
Memspace::nextfree( long o, long& sz )
{
	if ( o < 0 || o >= M_size ) {
		fprintf(stderr, "bad args to nextfree( 0x%.8lx, ... ) for Memspace '%s'",o,M_name);
		DB_PRINT(M_debug>=1,fprintf(dbfile, "bad args to nextfree( 0x%.8lx, ... ) for Memspace '%s'",o,M_name););
		sz = 0;
		return o;
	}
	Memseg *m = (Memseg*)M_seg.head();
	for ( ; m; m = (Memseg*)m->next )
		if ( o < m->m_offset + m->m_size ) break;
	if ( m == 0 ) {
		sz = 0;
		return M_size;
	}
	if ( o > m->m_offset ) {
		fprintf(stderr,"'%s'->nextfree( 0x%.8lx ): offset in middle of free segment <0x%.8lx 0x%.8lx>",o,m->m_offset,m->m_size);
		DB_PRINT(M_debug>=1,fprintf(stderr,"'%s'->nextfree( 0x%.8lx ): offset in middle of free segment <0x%.8lx 0x%.8lx>",o,m->m_offset,m->m_size););
		sz = 0;
		return o;
	}
	sz = m->m_size;
	return m->m_offset;
}

void
Memspace::save( FILE* ofd )
{
	long x = M_seg.length();
	DB_PRINT(M_debug>=1,
		fprintf(dbfile,"saving Memseg '%s', nsegs==%d\n",
			M_name, x));
	MUSTFWRITE( &x, sizeof(long), ofd );
	Memseg *m = (Memseg*)M_seg.head();
	for ( ; m; m = (Memseg*)m->next ) {
		DB_PRINT(M_debug>=1,
			fprintf(dbfile,"  o==0x%.8lx s==0x%.8lx\n",
				m->m_offset, m->m_size));
		MUSTFWRITE( &m->m_offset, sizeof(long), ofd );
		MUSTFWRITE( &m->m_size, sizeof(long), ofd );
	}
}

void
Memspace::restore( long* buf )
{
	register long i = 0;
	long x = buf[i++]; // nsegs
	Memseg *h = (Memseg*)M_seg.head();
	M_seg.delnext(0); // remove initial segment; list should now be empty
	delete h;
	DB_PRINT(M_debug>=1,
		fprintf(dbfile,"restoring Memseg '%s', nsegs==%d\n", M_name, x));
	for ( ; x>0; --x ) {
		long o = buf[i++];
		long s = buf[i++];
		DB_PRINT(M_debug>=1,
			fprintf(dbfile,"  o==0x%.8lx s==0x%.8lx\n",o,s));
		M_seg >>= new Memseg( o, s );
	}
}
#endif

void
Memseg::display()
{
	fprintf(dbfile,"MEMSEG: offset==0x%.8lx, size==0x%.8lx",m_offset,m_size);
}

void
Memspace::print()
{
	fprintf(dbfile,"Memspace '%s': size==0x%.8lx  free list: ",M_name,M_size);
	M_seg.display();
}
