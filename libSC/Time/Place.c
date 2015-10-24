/*ident	"@(#)Time:Place.c	3.1" */
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

String* Place::lsp;
tz* Place::lzp;

Place Place::here(){
    Place::init();
    return Place();
}

//  Definitions corresponding to declarations in tm.h

int		tm_flags_ATTLC;
tz*		tm_local_ATTLC;
char**          tm_form_ATTLC = &tm_dform_ATTLC[0];

void Time::set_table(char** tm_form){
    ::tm_form_ATTLC=tm_form;
}
char** Time::get_table(){
    return ::tm_form_ATTLC;
}

/*
 * The following function is correct for the years 1898-2400.
 *
 * (It is also correct for a smattering of years in the interval [1209-1897].)
 */

static inline int first_day_of(int y){
    int z=y-1;
    return (
	(
	    4+
	    (z-1900)+
	    (z-1900)/4-
	    (z-1900)/100+
	    (z>=2000)
	)%7+
	5
    )%7;
}
Place::Place(const Place& p){
    tsp=new String(*p.tsp);
    tzp=new tz;
    *tzp=*p.tzp;
    if(tzp->standard)tzp->standard = mystrdup_Time_ATTLC(tzp->standard);
    if(tzp->daylight)tzp->daylight = mystrdup_Time_ATTLC(tzp->daylight);
}
const Place& Place::operator=(const Place& p){
    if (this != &p) {
	*tsp=*p.tsp;
	if(tzp->standard)delete tzp->standard;
	if(tzp->daylight)delete tzp->daylight;
	*tzp=*p.tzp;
	if(tzp->standard)tzp->standard = mystrdup_Time_ATTLC(tzp->standard);
	if(tzp->daylight)tzp->daylight = mystrdup_Time_ATTLC(tzp->daylight);
    }
    return *this;
}
String Place::standard_zone()const{
    return tzp->standard;
}
int Place::west()const{
    return 60*tzp->west;
}
int Place::observes()const{
    return tzp->observes;
}
Place::Place( const char* TZ,int observes_dst ){

    tsp=new String(TZ);
    tzp=new tz;

//  Initialize ctime(3) globals to their default 
//  values.  Otherwise, tzset() won't work as 
//  advertized for valid TZ strings like "GMT0".

    ::timezone_ATTLC=0;
    strcpy(::tzname_ATTLC[0], "GMT");
    strcpy(::tzname_ATTLC[1], "   ");

//  The following save/restore sequence makes this code 
//  independent of the syntax of the TZ environment variable. 

    char* oldtz = getenv("TZ");
    char* newtz = new char[4 + strlen(TZ)];
    strcpy(newtz,"TZ=");
    strcat(newtz,TZ);
    putenv(newtz);

    tzset_ATTLC();
    if(oldtz)putenv(oldtz-3);
    delete newtz;

//  Assign values to the tz components

    tzp->type="";
    tzp->standard=mystrdup_Time_ATTLC(tzname_ATTLC[0]);
    tzp->west=(short)(::timezone_ATTLC/60);
    tzp->observes= observes_dst && ::daylight_ATTLC;

//  The following are meaningful only if tzp->observes

    tzp->dst=(short)((::altzone_ATTLC-::timezone_ATTLC)/60);
    tzp->daylight=mystrdup_Time_ATTLC(tzname_ATTLC[1]);
    tzp->spring_ahead=start_dst_ATTLC;
    tzp->fall_back=end_dst_ATTLC;
}
Place::Place(){
    Place::init();
    tsp=new String(*lsp);
    tzp = new tz;
    *tzp = *lzp;
    if(tzp->standard)tzp->standard = mystrdup_Time_ATTLC(tzp->standard);
    if(tzp->daylight)tzp->daylight = mystrdup_Time_ATTLC(tzp->daylight);
}
String Place::daylight_zone()const{
    return tzp->daylight;
}
int Place::dst()const{
    return 60*((tzp->west)+(tzp->dst));
}
Time Place::spring_ahead(unsigned int y)const{
    if( tzp->spring_ahead || tzp->fall_back ){  // same for all years
	return Time(y,Time::january,1,*this)+seconds(tzp->spring_ahead);
    }
    tm temp;
    long s;
    long e;

//  Set up a tm for January 1 of year y

    temp.tm_year=y-1900;
    temp.tm_yday=0;
    temp.tm_wday=first_day_of(y);
    getusa_ATTLC(&s,&e,&temp);
    return Time(y,Time::january,1,*this)+seconds(s);
}
Time Place::fall_back(unsigned int y)const{
    if( tzp->spring_ahead || tzp->fall_back ){  // fixed for all years
	return Time(y,Time::january,1,*this)+seconds(tzp->fall_back);
    }
    tm temp;
    long s;
    long e;

//  Set up a tm for January 1 of year y

    temp.tm_year=y-1900;
    temp.tm_yday=0;
    temp.tm_wday=first_day_of(y);
    getusa_ATTLC(&s,&e,&temp);
    return Time(y,Time::january,1,*this)+seconds(e);
}
Place::~Place(){
    delete tzp->standard;
    delete tzp->daylight;
    delete tzp;
    delete tsp;
}
static int default_action(const char* msg){
    write(2, msg, strlen(msg));
    write(2, "\n", 1);
    abort();
    return 0;  // to avoid compiler message
}
void Place::do_init(){

//  Get information about the host machine timezone
//  from the TZ environment variable

    char* temp=getenv("TZ");

//  If TZ is undefined, raise Time::environment_objection

    if( !temp ){

	if( !Time::environment_objection.raise() ){
	    default_action("TZ environment variable not set!");
	}else{

//  Recovery action: try again (in case the client
//  set TZ) and if failure occurs a second time, punt

	    temp=getenv("TZ");

	    if(!temp){
		putenv("TZ=GMT0");
		temp=getenv("TZ");
	    }
	}
    }

//  Initialize static members

    Place::lsp=new String(temp);
    Place::lzp=new tz;

//  Set up *Place::lzp;
//
//  The following code was lifted from Glenn's tminit(), 
//  which not used by Time(3C++).

    tzset_ATTLC();

    tm_local_ATTLC=Place::lzp;

    tm_local_ATTLC->type="";  // we don't use it
    tm_local_ATTLC->standard=mystrdup_Time_ATTLC(tzname_ATTLC[0]);
    tm_local_ATTLC->west=short(::timezone_ATTLC/60);  // Glen uses minutes
    tm_local_ATTLC->observes=::daylight_ATTLC;
    tm_local_ATTLC->dst=(short)((::altzone_ATTLC-::timezone_ATTLC)/60);
    tm_local_ATTLC->daylight=mystrdup_Time_ATTLC(tzname_ATTLC[1]);
    tm_local_ATTLC->spring_ahead=start_dst_ATTLC;
    tm_local_ATTLC->fall_back=end_dst_ATTLC;

    if(!(tm_flags_ATTLC & TM_ADJUST)){
	tm*        tp;
	time_t	   now;

	now = (time_t)78811200;   /* Jun 30 1972 23:59:60 */
	tp = (tm*)localtime_ATTLC(&now);
	if (tp->tm_sec != 60) tm_flags_ATTLC |= TM_ADJUST;
    }
}
String Place::make_string()const{
    return *tsp;
}

