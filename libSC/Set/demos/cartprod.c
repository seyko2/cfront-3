/*ident	"@(#)Set:demos/cartprod.c	3.1" */
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
#include <stream.h>

struct Pair
{
        int i;
        int j;
};

Set_or_Bag_hashval Set<int>::hash(const int& i)
{
        return (Set_or_Bag_hashval)i;
}
Set_or_Bag_hashval Set<Pair>::hash(const Pair& p)
{
        return (Set_or_Bag_hashval)(p.i+p.j);
}

int operator==(const Pair& p1,const Pair& p2)
{
        return p1.i==p2.i && p1.j==p2.j;
}

ostream& operator<<(ostream& os,const Pair& p)
{
        os << '<' << p.i << ',' << p.j << '>';
        return os;
}

Set<Pair> operator*(const Set<int>& a,const Set<int>& b){
        Set<Pair> result;
        Setiter<int> a_iter(a);
        const int* ap;
    
        while ( ap = a_iter.next() )
	{
            Pair p;
            p.i = *ap;
            Setiter<int> b_iter(b);
            const int* bp;
            
            while( bp = b_iter.next() )
	    {
                p.j = *bp;
                result.insert(p);
            }
        }
        return result;
}

main()
{
        Set<int> s(1,2,3,4), t(4,5,6);
        cout << s*t << endl;
	return 0;
}

