/*ident	"@(#)Set:Bag.c	3.1" */
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

#define A ((unsigned long)a)
#define B ((unsigned long)b)
#define CONST1 (BAG_MASK_BITS_ATTLC << BAG_INITIAL_SHIFT_ATTLC)
#define CONST2 (BAG_MASK_BITS_ATTLC << BAG_INITIAL_SHIFT_ATTLC + 1 * BAG_SHIFT_INCR_ATTLC)
#define CONST3 (BAG_MASK_BITS_ATTLC << BAG_INITIAL_SHIFT_ATTLC + 2 * BAG_SHIFT_INCR_ATTLC)
#define CONST4 (BAG_MASK_BITS_ATTLC << BAG_INITIAL_SHIFT_ATTLC + 3 * BAG_SHIFT_INCR_ATTLC)
#define CONST5 (BAG_MASK_BITS_ATTLC << BAG_INITIAL_SHIFT_ATTLC + 4 * BAG_SHIFT_INCR_ATTLC)
#define CONST6 (BAG_MASK_BITS_ATTLC << BAG_INITIAL_SHIFT_ATTLC + 5 * BAG_SHIFT_INCR_ATTLC)
#define CONST7 (BAG_MASK_BITS_ATTLC << BAG_INITIAL_SHIFT_ATTLC + 6 * BAG_SHIFT_INCR_ATTLC)
#define CONST8 (BAG_MASK_BITS_ATTLC << BAG_INITIAL_SHIFT_ATTLC + 7 * BAG_SHIFT_INCR_ATTLC)

int
BAG_LT_ATTLC(Set_or_Bag_hashval a, Set_or_Bag_hashval b)
{
    register unsigned long      twa;
    register unsigned long      twb;

    twa = (A & CONST1);
    twb = (B & CONST1);
    if (twa < twb) return 1;
    if (twa > twb) return 0;

    twa = (A & CONST2);
    twb = (B & CONST2);
    if (twa < twb) return 1;
    if (twa > twb) return 0;

    twa = (A & CONST3);
    twb = (B & CONST3);
    if (twa < twb) return 1;
    if (twa > twb) return 0;

    twa = (A & CONST4);
    twb = (B & CONST4);
    if (twa < twb) return 1;
    if (twa > twb) return 0;

    twa = (A & CONST5);
    twb = (B & CONST5);
    if (twa < twb) return 1;
    if (twa > twb) return 0;

    twa = (A & CONST6);
    twb = (B & CONST6);
    if (twa < twb) return 1;
    if (twa > twb) return 0;

    twa = (A & CONST7);
    twb = (B & CONST7);
    if (twa < twb) return 1;
    if (twa > twb) return 0;

    twa = (A & (unsigned long)CONST8);
    twb = (B & (unsigned long)CONST8);
    if (twa < twb) return 1;
    return 0;

/*    return      (twa = (A & CONST1)) < (twb = (B & CONST1)) ? 1 :
                            (twa > twb ? 0 :
                    (twa = (A & CONST2)) < (twb = (B & CONST2)) ? 1 :
                            (twa > twb ? 0 :
                    (twa = (A & CONST3)) < (twb = (B & CONST3)) ? 1 :
                            (twa > twb ? 0 :
                    (twa = (A & CONST4)) < (twb = (B & CONST4)) ? 1 :
                            (twa > twb ? 0 :
                    (twa = (A & CONST5)) < (twb = (B & CONST5)) ? 1 :
                            (twa > twb ? 0 :
                    (twa = (A & CONST6)) < (twb = (B & CONST6)) ? 1 :
                            (twa > twb ? 0 :
                    (twa = (A & CONST7)) < (twb = (B & CONST7)) ? 1 :
                            (twa > twb ? 0 :
                    (twa = (A & CONST8)) < (twb = (B & CONST8)) ? 1 :
                            0)))))));*/

#undef A
#undef B
#undef CONST1
#undef CONST2
#undef CONST3
#undef CONST4
#undef CONST5
#undef CONST6
#undef CONST7
#undef CONST8
}

void 
Bag_internal_node_ATTLC::initialize()
{
    if (bag_internal_node_pool_ptr_ATTLC == 0)
        bag_internal_node_pool_ptr_ATTLC = new Pool(sizeof(Bag_internal_node_ATTLC));
}

void*
Bag_internal_node_ATTLC::operator new(size_t i)
{
    initialize();
    if ( i == sizeof(Bag_internal_node_ATTLC) )
	return bag_internal_node_pool_ptr_ATTLC->alloc();
    else
	return new char[i];
}

Bag_internal_node_ATTLC::Bag_internal_node_ATTLC()
{
    register Bag_internal_item_ATTLC* itemp = &item[0];
    register int i = BAG_NODE_SIZE_ATTLC;

    while( i-- )
	(itemp++)->make_null();

    busy_count = 0;
}
