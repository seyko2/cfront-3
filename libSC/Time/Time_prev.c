/*ident	"@(#)Time:Time_prev.c	3.1" */
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

#include "Timelib.h"

Time Time::previous( Weekday w,const Place& z )const{
    int offset=(7+week_day(z)-w)%7;
    if(offset==0)offset=7;
    Time retval = *this-days(offset);
    Duration time_diff = clock_part(z) - retval.clock_part(z);
    if (time_diff != Duration(0)) {
        Duration dst_diff = seconds(z.west() - z.dst());
        retval += dst_diff;
        if (clock_part(z) - retval.clock_part(z) != Duration(0)) {
            retval -= dst_diff;
            retval -= dst_diff;
        }
    }
    return retval;
}

Time Time::next( Weekday w,const Place& z )const{
    int offset=(7+w-week_day(z))%7;
    if(offset==0)offset=7;
    Time retval = *this+days(offset);
    Duration time_diff = clock_part(z) - retval.clock_part(z);
    if (time_diff != Duration(0)) {
        Duration dst_diff = seconds(z.west() - z.dst());
        retval += dst_diff;
        if (clock_part(z) - retval.clock_part(z) != Duration(0)) {
            retval -= dst_diff;
            retval -= dst_diff;
        }
    }
    return retval;
}
