/*ident	"@(#)Time:Duration.c	3.1" */
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


#ifndef __GNUG__
static inline long abs(long i){
    return i>=0 ?i :-i;
}
#endif
Duration abs(const Duration& d){
    if(d<Duration(0)){
        return (-d);
    }else{
        return d;
    }
}
Duration operator+(
    const Duration& left,
    const Duration& right
){
    Duration temp(left);
    temp+=right;
    return temp;
}
const Duration& Duration::operator-=(
    const Duration& right
){
    *this+=(-right);
    return *this;
}
Duration operator-(
    const Duration& left,
    const Duration& right
){
    Duration temp(left);
    temp-=right;
    return temp;
}
Duration operator*(
    const Duration& left,
    long right
){
    Duration temp(left);
    temp*=right;
    return temp;
}
Duration operator/(
    const Duration& left,
    long right
){
    Duration temp(left);
    temp/=right;
    return temp;
}
String Duration::make_string( const char* fmt )const{
    Time t=Time()+*this;
    return t.make_string(fmt,Place::greenwich());
}
Duration::Duration( long d, long h, long m, long s ){

//  Extract whole days from s, m, and h, adding them 
//  to temp, simultaneously converting remainders to
//  seconds and accumulating them in s.  Precautions
//  against overflow are unnecessary because (1) s
//  can grow no larger than 4*24*60*60-1 (which is
//  less than MAXLONG) and (2) temp can grow no larger
//  than approximately MAXLONG/24.

    long temp=0;

    if( s!=0 ){
	temp += s/(24*60*60);
	s %= (24*60*60);
    }
    if( m!=0 ){
	temp += m/(24*60);
	s += (60)*(m%(24*60));
    }
    if( h!=0 ){
	temp += h/24;
	s += (60*60)*(h%24);
    }

//  Since s may now be greater than one day, 
//  extract whole days and add to temp.

    if( s!=0 ){
	temp += s/(24*60*60);
	s %= (24*60*60);
    }
    d += temp;
    s += d*24*60*60;
    ss=s;
}
const Duration& Duration::operator+=(
    const Duration& right
){
    ss += right.ss;
    return *this;
}
const Duration& Duration::operator*=(
    long right
){
    ss*=right;
    return *this;
}
const Duration& Duration::operator/=(
    long right
){
    ss/=right;
    return *this;
}
String Duration::make_string()const{
    String sign="";
    char buf[24];

    long d = day_part();
    long h = hour_part();
    long m = minute_part();
    long s = second_part();

    if(ss<0){
	sign = "-";
    }
    sprintf(
	buf,
	"%ldd %02ldh %02ldm %02lds",
	abs(d),
	abs(h),
	abs(m),
	abs(s)
    );
    return sign+buf;
}

static int handler(const char*){
    return 1;
}

Duration make_duration(const char* s){
    Duration result;
    
    time_t temp_t;
    time_t zero = 0;
    char* end;
    setup(Place::greenwich());
    temp_t = tmdate_ATTLC(s, &end, &zero);

    Objection_action* old_action=Time::string_objection.appoint(handler);
    if(*end || temp_t >= 24*60*60){
	Duration::string_objection.raise(
	    "Duration::string_objection raised"
	);
	result.ss=0;
    }else{
	result.ss=temp_t;
    }
    Time::string_objection.appoint(old_action);
    return result;
}
double operator/(
    const Duration& left,
    const Duration& right
){
    double result;
    result=double(left.ss)/double(right.ss);
    return result;
}
static int default_action(const char* msg){
    write(2, msg, strlen(msg));
    write(2, "\n", 1);
    abort();
    return 0;  // to avoid compiler message
}
Objection Duration::string_objection(default_action);
