/*ident	"@(#)Time:ctime/time_comm.c	3.1" */
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

// Functions common to ctime(3C) and cftime(3C)

#include <ctype.h>
#include <sys/types.h>
#include "mytime.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SEC_IN_DAY	(24 * 60 * 60)
#define MAXTZNAME	3
/* The year_size macro is good for the years [1901-2099]. */
#define	year_size(A)	(((A) % 4) ? 365 : 366)

//  global ctime(3) variables

time_t timezone_ATTLC = 0;
time_t altzone_ATTLC = 0;
int daylight_ATTLC = 0;
static char gmt[4] = "GMT";
static char bbb[4] = "   ";
char *tzname_ATTLC[] = { gmt, bbb };

//  In the original, the following were static variables.
//  We make the external so that we can simulate multiple 
//  concurrent timezones:

time_t start_dst_ATTLC ;   // Start date of alternate time zone
time_t end_dst_ATTLC;      // End date of alternate time zone

//  local functions

static char* getzname(char* p,char* tz);
static char* gettime(char* p,time_t* timez,int f);
static char* getdigit(char* ptr,int* d);
static int getdst(char* p,time_t* s,time_t* e);
static int sunday(register struct tm* t, long d);
static void set_TZ(void);

//  The following functions used to be local to this file:

//static void getusa(long* s,long* e,struct tm* t);
//static tm * gmtime(time_t*);

static short	month_size[12] = { 
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 
};

struct tm * localtime_ATTLC(time_t* tim){
    register struct tm* ct;
    long daybegin, dayend;
    time_t copyt, curr;

//  We are faking multiple timezones by setting the
//  global ctime variables directly prior to calling
//  localtime(); we must therefore prevent localtime() 
//  from calling tzset(), which reads the TZ variable
//  and sets up the global ctime variables based on
//  its contents.
//
//  tzset();
/*
    cout 
	<< "enter localtime with *tim, timezone="
	<< *tim
	<< ", "
	<< timezone_ATTLC
	<< "\n"
    ;
*/
    copyt = *tim - timezone_ATTLC;
    ct = gmtime_ATTLC(&copyt);
/*
    cout 
	<< "gmtime(&copyt) returns\n"
	<< "    ct->tm_sec="
	<< ct->tm_sec
	<< "\n"
	<< "    ct->tm_min="
	<< ct->tm_min
	<< "\n"
	<< "    ct->tm_hour="
	<< ct->tm_hour
	<< "\n"
	<< "    ct->tm_mday="
	<< ct->tm_mday
	<< "\n"
	<< "    ct->tm_mon="
	<< ct->tm_mon
	<< "\n"
	<< "    ct->tm_year="
	<< ct->tm_year
	<< "\n"
	<< "    ct->tm_wday="
	<< ct->tm_wday
	<< "\n"
	<< "    ct->tm_yday="
	<< ct->tm_yday
	<< "\n"
	<< "    ct->tm_isdst="
	<< ct->tm_isdst
	<< "\n"
    ;
*/
    if( !daylight_ATTLC ){
	return(ct);
    }
    curr = (
	ct->tm_yday*SEC_IN_DAY + 
	ct->tm_hour*3600 + 
	ct->tm_min*60 + 
	ct->tm_sec
    );
//  cout << "in localtime, curr=" << curr << "\n";
    if ( start_dst_ATTLC == 0 && end_dst_ATTLC == 0){
	getusa_ATTLC(&daybegin, &dayend, ct);
/*
	cout 
	    << "in localtime, getusa returns daybegin="
	    << daybegin
	    << ", dayend="
	    << dayend
	    << "\n"
	;
*/
    }else{
	daybegin = start_dst_ATTLC;
	dayend  = end_dst_ATTLC;
    }
    if( curr >= daybegin && curr < dayend ){
	copyt = *tim - altzone_ATTLC;
/*
	cout 
	    << "in localtime, recompute copyt=" 
	    << copyt 
	    << "\n"
	;
*/
	ct = gmtime_ATTLC(&copyt);
/*
	cout 
	    << "gmtime(&copyt) returns\n"
	    << "    ct->tm_sec="
	    << ct->tm_sec
	    << "\n"
	    << "    ct->tm_min="
	    << ct->tm_min
	    << "\n"
	    << "    ct->tm_hour="
	    << ct->tm_hour
	    << "\n"
	    << "    ct->tm_mday="
	    << ct->tm_mday
	    << "\n"
	    << "    ct->tm_mon="
	    << ct->tm_mon
	    << "\n"
	    << "    ct->tm_year="
	    << ct->tm_year
	    << "\n"
	    << "    ct->tm_wday="
	    << ct->tm_wday
	    << "\n"
	    << "    ct->tm_yday="
	    << ct->tm_yday
	    << "\n"
	    << "    ct->tm_isdst="
	    << ct->tm_isdst
	    << "\n"
	;
*/
	ct->tm_isdst = 1;
    }
//fprintf(stderr, "localtime 2 returns %d\n", (int)ct->tm_hour);
    return(ct);
}

struct tm * gmtime_ATTLC(time_t* tim){
    register int d0, d1;
    long hms, day;
    static struct tm xtime;

//  Break initial number into days 

    hms = *tim % SEC_IN_DAY;
    day = *tim / SEC_IN_DAY;

    if( hms < 0 ){
	hms += SEC_IN_DAY;
	day -= 1;
    }

//  Generate hours:minutes:seconds

    xtime.tm_sec = (int)(hms % 60);
    d1 = (int)(hms / 60);
    xtime.tm_min = d1 % 60;
    d1 /= 60;
    xtime.tm_hour = d1;

//  Generate day of the week. 
//  The addend is 4 mod 7 (1/1/1970 was Thursday) 

    xtime.tm_wday = (int)((day + 7340036L) % 7);

// Generate year number

    if( day >= 0 ){

	for(d1=70; day >= year_size(d1); d1++){
	    day -= year_size(d1);
	}
    }else{

	for(d1=70; day < 0; d1--){
	    day += year_size(d1-1);
	}
    }
    xtime.tm_year = d1;
    xtime.tm_yday = d0 = (int)day;

// Generate month

    if(year_size(d1) == 366){
	month_size[1] = 29;
    }
    for(d1 = 0; d0 >= month_size[d1]; d1++){
	d0 -= month_size[d1];
    }
    month_size[1] = 28;
    xtime.tm_mday = d0 + 1;
    xtime.tm_mon = d1;
    xtime.tm_isdst = 0;
    return(&xtime);
}

void tzset_ATTLC(){
    set_TZ();
    char *ptr;

    if ( (ptr = getenv("TZ")) == NULL){
	return;
    }
    
// Get main time zone name and difference from GMT

    if( 
	((ptr = getzname(ptr,tzname_ATTLC[0])) == 0) || 
	((ptr = gettime(ptr,&timezone_ATTLC,1)) == 0)
    ){
	return;
    }
    altzone_ATTLC = timezone_ATTLC - 3600;
    start_dst_ATTLC = end_dst_ATTLC = 0;
    daylight_ATTLC = 0;
     
// Get alternate time zone name

    if( (ptr = getzname(ptr,tzname_ATTLC[1])) == 0 ){
	return;
    }
    daylight_ATTLC = 1;

//  If the difference between alternate time zone and
//  GMT is not given, use one hour as default.

    if( *ptr == '\0' ){
	return;
    }
    if( *ptr != ';' ){

	if( 
	    (ptr = gettime(ptr,&altzone_ATTLC,1)) == 0 || 
	    *ptr != ';' 
	){
	    return;
	}
    }
    getdst(ptr + 1,&start_dst_ATTLC, &end_dst_ATTLC);
}

static char * getzname(char* p,char* tz){
    int n = MAXTZNAME;

    if( !isalpha(*p) ){
	return(0);
    }
    do{
	*tz++ = *p ;
    }while( --n > 0 && isalpha(*++p) );

    while( isalpha(*p) ){
	p++;
    }
    while( --n >= 0 ){
	*tz++ = ' ';	// Pad with blanks
    }
    return(p);	
}

static char* gettime(char* p,time_t* timez,int f){
    register time_t t = 0;
    int d, sign = 0;

    d = 0;

    if( f ){

	if( (sign = (*p == '-')) || (*p == '+') ){
	    p++;
	}
    }
    if( (p = getdigit(p,&d)) != 0 ){
	t = d * 60 * 60;

	if( *p == ':' ){

	    if( (p = getdigit(p+1,&d)) != 0 ){
		t += d * 60;

		if( *p == ':' ){

		    if( (p = getdigit(p+1,&d)) != 0 ){
			t += d;
		    }
		}
	    }
	}
    }
    if( sign ){
	*timez = -t;
    }else{
	*timez = t;
    }
    return(p);
}

static char* getdigit(char* ptr,int* d){

    if( !isdigit(*ptr) ){
	return(0);
    }
    *d = 0;

    do{
	*d *= 10;
	*d += *ptr - '0';
    }while( (isdigit(*++ptr)) );
    return(ptr);
}

static int getdst(char* p,time_t* s,time_t* e){
    int lsd,led;
    time_t st,et;
    st = et = 0;  // Default for start and end time is 00:00:00

    if( (p = getdigit(p,&lsd)) == 0 ){
	return(0);
    }
    lsd -= 1; 	// keep julian count in sync with date  1-366

    if( 
	(*p == '/') &&  
	((p = gettime(p+1,&st,0)) == 0) 
    ){
	return(0);
    }
    if( *p == ',' ){

	if( (p = getdigit(p+1,&led)) == 0 ){
	    return(0);
	}
	led -= 1;  // keep julian count in sync with date  1-366

	if( 
	    (*p == '/') &&  
	    ((p = gettime(p+1,&et,0)) == 0) 
	){
	    return(0);
	}
    }

// Convert the time into seconds

    *s = (long)(lsd * SEC_IN_DAY + st);
    *e = (long)(led * SEC_IN_DAY + et - (timezone_ATTLC - altzone_ATTLC));
    return(1);
}

void getusa_ATTLC(long* s,long* e,struct tm* t){
    
    static struct {
	int	yrbgn;
	int	daylb;
	int	dayle;
    } daytab[] = {
	87,	96,	303,	// new legislation - 1st Sun in April
	76,	119,	303,
	75,	58,	303,	// 1975: Last Sun in Feb - last Sun in Oct
	74,	5,	333,	// 1974: Jan 6 - last Sun. in Nov
	69,	119,	303,	// some timezones have legal times in 1969
	-10000,	119,	303,	// treat the distant past under current rules
    };
    int i = 0;

    while( t->tm_year < daytab[i].yrbgn ){ // can't be less than 69
	i++;
    }
    *s = daytab[i].daylb; // fall through the loop when in correct interval
    *e = daytab[i].dayle;

    *s = sunday(t, *s);
    *e = sunday(t, *e);
    *s = (long)(*s * SEC_IN_DAY +2*60*60);
    *e = (long)(*e * SEC_IN_DAY +60*60);
}

static int sunday(register struct tm* t, long d){
    if( d >= 58 ){
	d += year_size(t->tm_year) - 365;
    }
    return (int)(d - (d - t->tm_yday + t->tm_wday + 700) % 7);
}

static void set_TZ(void)
{
 char *p;
 if ((p = getenv("TZ")) == NULL) { return; }
 if (strcmp(p, "US/Eastern") == 0) { putenv("TZ=EST5EDT"); return; }
 if (strcmp(p, "US/Central") == 0) { putenv("TZ=CST6CDT"); return; }
 if (strcmp(p, "US/Mountain") == 0) { putenv("TZ=MST7MDT"); return; }
 if (strcmp(p, "US/Pacific") == 0) { putenv("TZ=PST8PDT"); return; }
 if (strcmp(p, "CA/Yukon") == 0) { putenv("TZ=YST9YDT"); return; }
 if (strcmp(p, "US/Hawaii") == 0) { putenv("TZ=HST10"); return; }
 return;
}
