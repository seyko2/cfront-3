/*ident	"@(#)Time:Time.c	3.1" */
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

void setup(const Place& z){     

//  Set up tm_local and the UNIX SVR3 ctime(3) 
//  environment variables appropriately for Place z.
//
//  We should probably define a Time class variable 
//  that 'remembers' which Place is currently in effect;
//  this would avoid repeated assignment when the
//  Place remains the same, as will be the case in
//  applications that work strictly in local time.

    tm_local_ATTLC=z.tzp;
    ::timezone_ATTLC = tm_local_ATTLC->west*60;
    ::altzone_ATTLC = ::timezone_ATTLC+tm_local_ATTLC->dst*60;
    ::daylight_ATTLC = tm_local_ATTLC->observes;
    strcpy(::tzname_ATTLC[0], tm_local_ATTLC->standard);
    strcpy(::tzname_ATTLC[1], tm_local_ATTLC->daylight);
    ::start_dst_ATTLC = tm_local_ATTLC->spring_ahead;
    ::end_dst_ATTLC = tm_local_ATTLC->fall_back;
}
unsigned month_length_ATTLC[]={
    31,28,31,30,31,30,
    31,31,30,31,30,31
};
unsigned first_day_of_month_ATTLC[]={
      1, 32, 60, 91,121,152,
    182,213,244,274,305,335
};

Time::Time(unsigned y,Month m,unsigned d){
    *this = Time(y,m,d,Place::here());
}
Time::Time(unsigned y,Time::Month m,unsigned d,const Place& z){

//  The arguments represent midnight in 
//  Place z on the given date.

    tm x;

    x.tm_sec=0;
    x.tm_min=0;
    x.tm_hour=0;
    x.tm_mday=d;
    x.tm_mon=m;
    x.tm_year=y-1900;

    tmfix_ATTLC(&x);
    setup(z);

//  Convert to seconds since 0h GMT on 1/1/1970.  
//  (tmtime will add the difference between local 
//  time and Greenwich).

    t=tmtime_ATTLC(&x,TM_LOCALZONE);
}
Time Time::julian( unsigned y,int d,const Place& z ){
    tm x;
    x.tm_sec=0;
    x.tm_min=0;
    x.tm_hour=0;
    x.tm_mday=d;
    x.tm_mon=0;
    x.tm_year=y-1900;

    tmfix_ATTLC(&x);
    setup(z);
    time_t temp=tmtime_ATTLC(&x,TM_LOCALZONE);
    Time result(temp);
    return result;
}
unsigned Time::julian_day_no(const Place& z)const{
    tm* tmp;
    setup(z);
    tmp=tmmake_ATTLC(&((Time *)this)->t);
    return tmp->tm_yday+1;
}
unsigned Time::year_part(const Place& z)const{
    setup(z);
    tm* tmp=tmmake_ATTLC(&(((Time *)this)->t));
    return tmp->tm_year+1900;
}
Time::Month Time::month_part(const Place& z)const{
    setup(z);
    tm* tmp=tmmake_ATTLC(&(((Time *)this)->t));
    return (Time::Month)tmp->tm_mon;
}
unsigned Time::day_part(const Place& z)const{
    setup(z);
    tm* tmp=tmmake_ATTLC(&(((Time *)this)->t));
    return tmp->tm_mday;
}
Duration Time::clock_part(const Place& z)const{
    setup(z);
    tm* tmp=tmmake_ATTLC(&(((Time *)this)->t));
    return Duration( 0,tmp->tm_hour,tmp->tm_min,tmp->tm_sec );
}
Time::Weekday Time::week_day(const Place& z)const{
    setup(z);
    tm* tmp=tmmake_ATTLC(&(((Time *)this)->t));
    return (Time::Weekday)tmp->tm_wday;
}
Time make_time( const char* s,const Place& z ){
    char* end;
    setup(z);
    time_t t=tmdate_ATTLC(s,&end,0);

    if(*end){
	Time::string_objection.raise("Time::string_objection raised");
	t=0;
    }
    Time result(t);
    return result;
}
String Time::make_string( const char* s,const Place& z )const{
    char buf[512];
    setup(z);
    tmform_ATTLC(buf,(char*)s,&(((Time *)this)->t));
//write(2, "buf is <", 8);
//write(2, buf, strlen(buf));
//write(2, ">\n", 2);
    return String(buf);
}
unsigned leap_adjust_ATTLC( unsigned y ){
    return (unsigned)( y%4==0 && ( y%100!=0 || y%400==0) );
}
static int default_action(const char* msg){
    write(2, msg, strlen(msg));
    write(2, "\n", 1);
    abort();
    return 0;  // to avoid compiler message
}
const Time& Time::operator-=(const Duration& right){
    *this+=(-right);
    return *this;
}
Time operator-(const Time& left,const Duration& right){
    Time temp(left);
    temp-=right;
    return temp;
}
Objection Time::string_objection(default_action);
Objection Time::environment_objection;
