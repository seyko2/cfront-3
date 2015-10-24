/*ident	"@(#)Set:demos/Spool.c	3.1" */
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

#include <Set.h>
#include <Objection.h>
#include <stddef.h>
#include <stream.h>
#include <stdlib.h>

class Spool
{
    size_t eltsize;
    Set_of_p<void> allocated;
    Set_of_p<void> available;
    void check() const;
public:
    static Objection bad_pointer;
    Spool(size_t n);
    void* alloc();
    void free(void* p);
    ~Spool();
};


void Spool::check() const
{
    if(allocated & available)abort();
}

Spool::Spool(size_t n) : eltsize(n)
{ 
    for ( int i=0 ; i<100 ; i++ )
    {
        available.insert(new char[eltsize]);
    }
    check();
}

void* Spool::alloc()
{
    check();
    void* result;

    if ( available )
    {
        result = available.select();
        available.remove(result);
        allocated.insert(result);
    }
    else
    {
        for ( int i=0 ; i<100 ; i++ )
	{
            available.insert(result = new char[eltsize]);
        }
        result = alloc();
    }
    check();
    return result;
}

void Spool::free(void* p)
{
    check();

    if ( !allocated.contains(p) )
    {
        if ( bad_pointer.raise() == 0 )
	{
	    cerr << "Spool: bad pointer: " << p << endl;
	    exit(0);
	}
    }
    else
    {
        allocated.remove(p);
        available.insert(p);
    }
    check();
}

Spool::~Spool()
{
    check();
    Set_of_piter<void> it(available);
    void* p;
    while ( p = it.next() )
    {
        delete p;
    }
    check();
}

Objection Spool::bad_pointer;

main()
{
	Spool spool(5);
	void *p = spool.alloc();
	spool.free(p);
	spool.free((void*)0x7);
	return 0;
}
