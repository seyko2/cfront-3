/*ident	"@(#)Time:demos/cal.c	3.1" */
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

#include "cal.h"
#include <Args.h>
#include <Regex.h>

Time midnight_before(const Time& t) {
    return t - t.clock_part();
}

Date :: Date(Time t) : midnight(midnight_before(t)) {}

ostream& operator<<(ostream& os, Date d) {
    os << d.midnight.make_string("%x");
    return os;
}

int operator<(Date a, Date b) {
    return a.midnight < b.midnight;
}

// set by handler(), tells read_appts to skip
// this input line
int skip = 0;
int lineno = 0;

void show_clock_part(ostream& os, const Time& t) {
    os << t.clock_part().make_string("%X");
}

ostream& operator<<(ostream& os, const Appointment& a) {
    show_clock_part(os, a.time);
    os << "\t" << a.desc;
    return os;
}

// called when a time in the input is malformed
int handler(const char*) {
    cerr << "Badly formatted date on line "
         << lineno
         << " (appointment ignored)"
         << endl;
    skip = 1;
    return 1;
}

// read all the Appointments in the input
// into the Calendar
void read_appts(Calendar& cal) {
    char buf[100];
    while (cin.getline(buf, 100, '\t')) {
        ++lineno;
        Appointment a;
        a.time = make_time(buf);
            // make_time() may raise handler() 
        cin.getline(buf, 100, '\n');
        if (skip)
            skip = 0;
        else {		   
            a.desc = buf;
            //cout << "got appt: " << a.time;
            //cout << " -- " << a.desc << endl;
            cal.add(a);
        }
    }
}

// show the Appointments in appts matching r
void show_appts(const Appts& appts,const Regex& r){
    for (int i = 0; i < appts.num(); ++i)
        if (r.match(appts[i].desc))
            cout << "\t" << appts[i] << endl;
}

// show today's Appointments matching r
void show_todays_appts(const Calendar& cal, const Regex& r) {
    Time now = make_time("now");
    Date today(now);

    if (!cal.element(today))
        cout << "cal: no appointments for today"
             << endl;
    else {
        cout << "your appointments for " << today
             << ":" << endl;
        show_appts(cal[today], r);
    }
}

// search in appts for first Appointment
// after now matching r
int search_in_appts(const Appts& appts, Time now, const Regex& r) {
    for (int i = 0; i < appts.num(); ++i) {
        const Appointment& a = appts[i];
        if (a.time > now && r.match(a.desc)) {
            cout << "Your next appointment is on "
                 << a.time.make_string("%x:")
                 << endl;
            cout << "\t" << a << endl;
            return 1;
        }
    }
    return 0;
}

// starting at i, search for first Appointment
// after now matching r
void search_forward_in_cal(Calendariter& i, Time now, const Regex& r) {
    for ( ; i; i.next())
        if (search_in_appts(i.curr()->value, now, r))
            return;
}

// show first Appointment in Calendar matching r
void show_next_appt(const Calendar& cal, const Regex& r) {
    Time now = make_time("now");
    Date today(now);

    Calendariter i = cal.element(today);
    if (!i) i = cal.first();

    search_forward_in_cal(i, now, r);
}

// process the -t and -n queries
void process_queries(const Calendar& cal, int argc, const char*const* argv) {
    Args args(argc, argv, "tnr:");

    Regex r(""); // matches everything
    if (args.isset('r'))
        r.assign(args.value('r'),
            Regex::case_insensitive);

    if (args.isset('t'))
        show_todays_appts(cal, r);
    if (args.isset('n'))
        show_next_appt(cal, r);
}

// initialize the handler, read in the Calendar,
// then process the queries
main(int argc, const char*const* argv) {
    Time::string_objection.appoint(handler);
    Calendar cal;
    read_appts(cal);
    process_queries(cal, argc, argv);
    return 0;
}

