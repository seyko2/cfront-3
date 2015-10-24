/*ident	"@(#)Time:incl/Time.h	3.2" */
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

#ifndef TIMEH
#define TIMEH

#ifdef MIN
#undef MIN
#endif
#ifdef MAX
#undef MAX
#endif

#include <Objection.h>
#include <String.h>
#include <values.h>

class ostream;

//  Exports the following global names

class Duration;
class Place;
class Time;
struct tz;



class Duration{
public: 

//  Duration constants

    static inline Duration MAX();

//  Objections

    static Objection string_objection;

//  Constructors, destructor

    inline Duration();
    Duration(
	long d,            // days
	long h=0,          // hours
	long m=0,          // minutes
	long s=0           // seconds
    );
// If the following is uncommented and --long_lifetime_temps is used,
// then edgcpfe won't inline functions that return a Duration.
//  ~Duration() {}

//  Copy and assign

    inline Duration(const Duration& d);
    inline Duration& operator=(const Duration& d);

//  Component extractors

    inline long day_part()const;
    inline int hour_part()const;
    inline int minute_part()const;
    inline int second_part()const;

    friend inline long seconds(const Duration& d);
    inline operator void*()const;

//  Conversion to and from Strings

    String make_string(const char* fmt)const;
    String make_string()const;
    friend Duration make_duration(const char* s);

//  Relational operators

    friend inline int operator==(const Duration& left, const Duration& right);
    friend inline int operator!=(const Duration& left, const Duration& right);
    friend inline int operator>(const Duration& left, const Duration& right);
    friend inline int operator>=(const Duration& left, const Duration& right);
    friend inline int operator<(const Duration& left, const Duration& right);
    friend inline int operator<=(const Duration& left, const Duration& right);

//  Fixed-point arithmetic operators

    const Duration& operator+=(const Duration& d);

    friend Duration operator+(const Duration& left, const Duration& right);
    const Duration& operator-=(const Duration& d);
    friend Duration operator-(const Duration& left, const Duration& right);
    friend inline Duration operator-(const Duration& right);
    const Duration& operator*=(long d);
    friend Duration operator*(const Duration& left, long right);
    friend inline Duration operator*(long left, const Duration& right);
    friend Duration abs(const Duration& d);

//  Approximate arithmetic operators

    const Duration& operator/=(long right);
    friend Duration operator/(const Duration& left, long right);
    friend double operator/(const Duration& left, const Duration& right);

//  Functions for expressing Durations in "natural" units

    static inline Duration days(long d);
    static inline Duration hours(long h);
    static inline Duration minutes(long m);
    static inline Duration seconds(long s);

//  Stream insertion

    friend ostream& operator<<(ostream& os,const Duration& d);

private:

//  The representation is a long containing the
//  duration in seconds

    long ss;   

//  Representation invariant:
//
//      true

};

inline
Duration Duration::MAX(){
    return Duration(0,0,0,MAXLONG);
}
inline
Duration::Duration():ss(0){ }
inline
Duration::Duration(const Duration& d):ss(d.ss){ }
inline
Duration& Duration::operator=(const Duration& d){
    ss = d.ss;
    return *this;
}
inline
Duration operator-(
    const Duration& d
){
    Duration temp(d);
    temp.ss = -temp.ss;
    return temp;
}
inline
long Duration::day_part()const{
    return ss/(24*60*60);
}
inline
int Duration::hour_part()const{
    return (ss%(24*60*60))/(60*60);
}
inline
int Duration::minute_part()const{
    return (ss%(60*60))/60;
}
inline
int Duration::second_part()const{
    return ss%60;
}
inline
Duration::operator void*()const{
    return ( ss!=0 )?(void*)this:0;
}
inline
long seconds(const Duration& d){
    return d.ss;
}
inline
int operator==(
    const Duration& left,
    const Duration& right
){
    return left.ss==right.ss;
}
inline
int operator!=(
    const Duration& left,
    const Duration& right
){
    return left.ss!=right.ss;
}
inline
int operator>(
    const Duration& left,
    const Duration& right
){
    return left.ss>right.ss;
}
inline
int operator<(
    const Duration& left,
    const Duration& right
){
    return left.ss<right.ss;
}
inline
int operator<=(
    const Duration& left,
    const Duration& right
){
    return left.ss<=right.ss;
}
inline
int operator>=(
    const Duration& left,
    const Duration& right
){
    return left.ss>=right.ss;
}
inline
Duration operator*(
    long left,
    const Duration& right
){
    return right*left;
}
inline
Duration Duration::days(
    long d
){
    return Duration(d);
}
inline
Duration Duration::hours(
    long h
){
    return Duration(0,h);
}
inline
Duration Duration::minutes(
    long m
){
    return Duration(0,0,m);
}
inline
Duration Duration::seconds(
    long s
){
    return Duration(0,0,0,s);
}



class Place{
    friend class Time;
    friend void setup(const Place& z);
public:

//  Standard Places

    inline static Place eastern();
    inline static Place central();
    inline static Place mountain();
    inline static Place pacific();
    inline static Place yukon();
    inline static Place hawaii();
    inline static Place greenwich();
    static Place here();

//  Constructors, destructor

    Place();
    Place(const char* TZ,int observes_dst=1);
    ~Place();

//  Copy and assign

    Place(const Place& z);
    const Place& operator=(const Place& right);

//  Operations for extracting timezone information

    int west()const;
    int dst()const;
    int observes()const;
    String daylight_zone()const;
    String standard_zone()const;
    Time spring_ahead(unsigned y)const;
    Time fall_back(unsigned y)const;
    String make_string()const;

//  Stream insertion

    friend ostream& operator<<(ostream& os,const Place& p);

private:

//  lsp points to a copy of the TZ string obtained from getenv

    static String* lsp;

//  lzp points to timezone information parsed from *lsp.

    static tz* lzp;

//  tsp points to a copy of the TZ string obtained from 
//  the Place constructor

    String* tsp;

//  tzp points to timezone information (of type struct tz)
//  parsed from *tsp.

    tz* tzp;

//  The following are used for initialization of all
//  Place and Time statics

    inline static void init();
    static void do_init();
};

inline
void Place::init(){
    if( !Place::lsp ){
	Place::do_init();
    }
}
inline
Place Place::central(){
    return Place("CST6CDT");
}
inline
Place Place::greenwich(){
    return Place("GMT0");
}
inline
Place Place::hawaii(){
    return Place("HST10");
}
inline
Place Place::yukon(){
    return Place("YST9YDT");
}
inline
Place Place::pacific(){
    return Place("PST8PDT");
}
inline
Place Place::mountain(){
    return Place("MST7MDT");
}
inline
Place Place::eastern(){
    return Place("EST5EDT");
}



class Time{
public:

//  Time constants

    static inline Time MAX();
    static inline Time MIN();
    static inline Time REF();

//  Objections

    static Objection environment_objection;
    static Objection string_objection;

//  Enumerations

    enum Weekday{ 
	sunday    = 0, monday    = 1, tuesday   = 2,
	wednesday = 3, thursday  = 4, friday    = 5,
	saturday  = 6
    }; 
    enum Month{
	january   = 0, february  = 1, march     = 2, 
	april     = 3, may       = 4, june      = 5, 
	july      = 6, august    = 7, september = 8, 
	october   = 9, november  = 10, december  = 11
    };

//  Constructors, destructor

    inline Time();
    Time(unsigned y,Month m,unsigned d);
    Time(unsigned y,Month m,unsigned d,const Place& z);
// If the following is uncommented and --long_lifetime_temps is used,
// then edgcpfe won't inline functions that return a Time.
//  ~Time() {}

//  Copy and assign

    inline Time(const Time& t);
    inline Time& operator=(const Time& t);

//  Julian dates

    inline unsigned julian_day_no()const;
    unsigned julian_day_no(const Place& z)const;

//  Component extractors

    inline unsigned year_part()const;
    unsigned year_part(const Place& z)const;
    inline Month month_part()const;
    Month month_part(const Place& z)const;
    inline unsigned day_part()const;
    unsigned day_part(const Place& z)const;
    inline Duration clock_part()const;
    Duration clock_part(const Place& z)const;

//  Day-of-week operations

    inline Weekday week_day()const;
    Weekday week_day(const Place& z)const;
    inline Time previous(Weekday w)const;
    Time previous(Weekday w,const Place& z)const;
    inline Time next(Weekday w)const;
    Time next(Weekday w,const Place& z)const;

//  Relations

    friend inline int operator==(const Time& left,const Time& right);
    friend inline int operator!=(const Time& left,const Time& right);
    friend inline int operator<=(const Time& left,const Time& right);
    friend inline int operator>(const Time& left,const Time& right);
    friend inline int operator>=(const Time& left,const Time& right);
    friend inline int operator<(const Time& left,const Time& right);

//  Arithmetic operators

    inline const Time& operator+=(const Duration& right);
    friend inline Time operator+(const Time& left,const Duration& right);
    friend inline Time operator+(const Duration& left,const Time& right);
    const Time& operator-=(const Duration& right);
    friend Time operator-(const Time& left,const Duration& right);
    friend inline Duration operator-(const Time& left,const Time& right);

//  Conversion to and from Strings

    inline String make_string(const char* fmt)const;
    String make_string(const char* fmt,const Place& z)const;
    inline String make_string()const;
    inline String make_string(const Place& z)const;
    friend inline Time make_time(const char* p);
    friend Time make_time(const char* p,const Place& z);

    static void set_table(char** tm_form);
    static char** get_table();

//  Conversion to and from time_t 

    friend inline time_t make_time_t(const Time& t);
    friend inline Time make_time(time_t x);

//  Auxiliary Time Functions

    static inline unsigned days_in_year(unsigned y);
    static inline int is_leap(unsigned y);
    static inline unsigned first_day(Month m,unsigned y);
    static inline unsigned days_in_month(Month m,unsigned y);
    static inline int valid_date(unsigned y,Month m,unsigned d);
    static inline Time julian(unsigned y,int d);
    static Time julian(unsigned y,int d,const Place& z);

// Stream insertion

    friend ostream& operator<<(ostream& os,const Time& t);

//  Miscellaneous

    inline unsigned hash()const;

private:
    inline Time(time_t t);
    time_t t;        // seconds since 1/1/1970 at 0h GMT

};

//  Inline function definitions: Time

inline
Time::Time(time_t x):t(x){
}
inline
Time make_time(time_t t){
    return Time(t);
}
inline
Time Time::MAX(){
    return make_time(MAXLONG);
}
inline
Time Time::MIN(){
    return make_time(-MAXLONG-1);
}
inline
Time Time::REF(){
    return make_time((time_t)0);
}

extern unsigned first_day_of_month_ATTLC[];
extern unsigned month_length_ATTLC[];

inline
Time::Time():t(0){ 
}
inline
Time::Time(const Time& _t):t(_t.t){ 
}
inline
Time& Time::operator=(const Time& _t){
    this->t = _t.t;
    return *this;
}
inline
Time Time::julian(unsigned y,int d){
    return julian(y,d,Place::here());
}
inline
unsigned Time::julian_day_no()const{
    return julian_day_no(Place::here());
}
inline
unsigned Time::year_part()const{
    return year_part(Place::here());
}
inline
Time::Month Time::month_part()const{
    return month_part(Place::here());
}
inline
unsigned Time::day_part()const{
    return day_part(Place::here());
}
inline
Duration Time::clock_part()const{
    return clock_part(Place::here());
}
inline
Time::Weekday Time::week_day()const{
    return week_day(Place::here());
}
inline
Time Time::previous(Weekday w)const{
    return previous(w,Place::here());
}
inline
Time Time::next(Weekday w)const{
    return next(w,Place::here());
}
inline
String Time::make_string(const char* fmt)const{
    return make_string(fmt,Place::here());
}
inline
String Time::make_string(const Place& z)const{
    return make_string("%x %X",z);
}
inline
String Time::make_string()const{
    return make_string(Place::here());
}
inline
Time make_time(const char* p){
    return make_time(p,Place::here());
}

extern unsigned leap_adjust_ATTLC(unsigned y);

inline
unsigned Time::days_in_year(unsigned y){
    return 365+leap_adjust_ATTLC(y);
}
inline
int Time::is_leap(unsigned y){
    return leap_adjust_ATTLC(y)==1;
}
inline
unsigned Time::first_day(Month m,unsigned y){
    return first_day_of_month_ATTLC[m] + (m>february)*leap_adjust_ATTLC(y);
}
inline
unsigned Time::days_in_month(Month m,unsigned y){
    return (
	month_length_ATTLC[m] + (
	    m==february ?(
		leap_adjust_ATTLC(y)
	    ):(
		0
	    )
	)
    );
}
inline
int Time::valid_date(unsigned y,Month m,unsigned d){
    return (
	m>=january &&
	m<=december &&
	d>=1 &&
	d<=days_in_month(m,y)
    );
}
inline
int operator==(const Time& left,const Time& right){
    return left.t==right.t;
}
inline
int operator!=(const Time& left,const Time& right){
    return left.t!=right.t;
}
inline
int operator<=(const Time& left,const Time& right){
    return left.t<=right.t;
}
inline
int operator>(const Time& left,const Time& right){
    return left.t>right.t;
}
inline
int operator>=(const Time& left,const Time& right){
    return left.t>=right.t;
}
inline
int operator<(const Time& left,const Time& right){
    return left.t<right.t;
}
inline
const Time& Time::operator+=(const Duration& right){
    t += seconds(right);
    return *this;
}
inline
Time operator+(const Time& left,const Duration& right){
    Time temp(left);
    temp+=right;
    return temp;
}
inline
Time operator+(const Duration& left,const Time& right){
    Time temp(right);
    temp+=left;
    return temp;
}
inline
Duration operator-(const Time& left,const Time& right){
    time_t sum;
    sum = left.t - right.t; 
    return Duration(0,0,0,sum);
}
inline
time_t make_time_t(const Time& t){
    return t.t;
}
inline
unsigned Time::hash()const{
    return (unsigned)t;
}



//  The following definitions are provided for
//  compatibility with earlier releases.  If
//  you are not concerned with compatibility,
//  you can remove them.

#define EASTERN		Place::eastern()
#define CENTRAL		Place::central()
#define MOUNTAIN	Place::mountain()
#define PACIFIC		Place::pacific()
#define YUKON		Place::yukon()
#define HAWAII		Place::hawaii()
#define GREENWICH	Place::greenwich()
#define HERE		Place::here()

#define SUNDAY		Time::sunday
#define MONDAY		Time::monday
#define TUESDAY		Time::tuesday
#define WEDNESDAY	Time::wednesday
#define THURSDAY	Time::thursday
#define FRIDAY		Time::friday
#define SATURDAY	Time::saturday

#define JANUARY		Time::january
#define FEBRUARY	Time::february
#define MARCH 		Time::march
#define APRIL 		Time::april
#define MAY		Time::may
#define JUNE 		Time::june
#define JULY		Time::july
#define AUGUST 		Time::august
#define SEPTEMBER 	Time::september
#define OCTOBER 	Time::october
#define NOVEMBER 	Time::november
#define DECEMBER 	Time::december

//  Duration

inline Duration days(long d){
    return Duration::days(d);
}
inline Duration hours(long h){
    return Duration::hours(h);
}
inline Duration minutes(long m){
    return Duration::minutes(m);
}
inline Duration seconds(long s){
    return Duration::seconds(s);
}

//  Time 

inline Time julian(unsigned y,int d){
    return Time::julian(y,d);
}
inline Time julian(unsigned y,int d,const Place& z){
    return Time::julian(y,d,z);
}
inline unsigned days_in_year(unsigned y){
    return Time::days_in_year(y);
}
inline int is_leap(unsigned y){
    return Time::is_leap(y);
}
inline unsigned first_day(Time::Month m,unsigned y){
    return Time::first_day(m,y);
}
inline unsigned days_in_month(Time::Month m,unsigned y){
    return Time::days_in_month(m,y);
}
inline int valid_date(unsigned y,Time::Month m,unsigned d){
    return Time::valid_date(y,m,d);
}

//  End of compatibility definitions

#endif
