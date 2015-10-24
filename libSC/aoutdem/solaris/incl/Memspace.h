#include "Link.h"

/*
 *  memory management utility
 *    class Memspace manages a contiguous space of M_size units
 */

class Memspace { // contiguous memory divided into variable-sized chunks
	Chain M_seg;
	char *M_name;
	long M_size;
	unsigned long M_flags;
#ifdef IDEBUG
	int M_debug;
#endif
public:
	enum { M_dynamic=01 };
	Memspace( long sz, char* n = "", unsigned long f = 0 );
	~Memspace();
	long allocseg( long sz ); // returns offset; allocate by first-fit
	int grow( long sz ); // append new free space at end of this Memspace
#if 0
	long placeseg( long off, long sz ); // place segment at specified offset
#endif
	void freeseg( long off, long sz );  // free allocated space
#if 0
	long nextfree( long, long& );
	void save( FILE* ); // save representation in file
	void restore( long* ); // build Memspace from data in buf
#endif
	long msize() { return M_size; }
	long svsize() // number of longs needed to save this Memspace
		{ return M_seg.length()*2 + 1; }
	void print();
	const char* name() { return M_name; }
#ifdef IDEBUG
	void set_debug( int );
#endif
};

// expansion of output sections and internal buffers to handle
//    incremental updates
#define MINMEM 1024
#define MEMADJUST(s,a) (scratch=align( (s)+(long)((s)*((double)section_scale/100.0)), a ), scratch<MINMEM?MINMEM:scratch )
extern int section_scale;
extern long scratch;

inline long
align( long s, int a )
{
	return s%a ? (s+a-s%a) : s;
}

inline long
align2( long off, long a )
{
	return ( off & (a-1) ) ? off + a - ( off & (a-1) ) : off;
}

extern int debugMem;
extern int debugInternal;
