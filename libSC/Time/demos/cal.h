/*ident	"@(#)Time:demos/cal.h	3.1" */
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

#include "appoint.h"
#include <Regex.h>
#include <Block.h>
#include <Array_alg.h>
#include <Map.h>
#include <stream.h>

// an Appts contains all of the Appointments
// for a given day, in time order
class Appts {
    Block<Appointment> appts;
    int nappts;
public:
    Appts() : nappts(0) {}
    void add(const Appointment& a) {
        appts.reserve(nappts);
        insert(a, &appts[0], &appts[nappts]);
        ++nappts;
    }
    int num() const {
        return nappts;
    }
    const Appointment& operator[](int i) const {
        return appts[i];
    }
};

// represent Dates by the midnight of that date
class Date {
    friend ostream& operator<<(ostream& os, Date d);
    friend int operator<(Date a, Date b);
    Time midnight;
public:
    Date() {}
    Date(Time t);
};

typedef Mapiter<Date,Appts> Calendariter;

// a Calendar is basically a Map from Dates to Appts
class Calendar {
    Map<Date,Appts> m;
public:
    void add(const Appointment& a) {
        Date d(a.time);
        m[d].add(a);
    }
    Calendariter element(Date d) const { 
        return m.element(d);
    }
    Calendariter first() const { 
        return m.first();
    }
    const Appts& operator[](Date t) const {
        return ((Calendar*)this)->m[t];
    }
};
