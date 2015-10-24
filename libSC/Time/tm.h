/*ident	"@(#)Time:tm.h	3.1" */
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

#ifndef TMH
#define TMH

#ifndef TM_DEFAULT

#include <sys/types.h>
#include "mytime.h"

#define tmset()	/*do{if(!tm_local->daylight)tminit();}while(0)*/

#ifndef VOID
#define VOID	void
#endif

#define TM_ADJUST      (1<<0)	 /* local doesn't do leap secs	*/
#define TM_LEAP	       (1<<1)	 /* do leap seconds		*/
#define TM_UTC	       (1<<2)	 /* universal coordinated ref	*/

#define TM_DST	       (-60)	 /* default minutes for DST	*/
#define TM_LOCALZONE   (25 * 60) /* use local time zone offset	*/
#define TM_MAXLEAP     1	 /* max leap secs per leap	*/

/*
 * these indices must agree with tm_dform[]
 */

#define TM_MONTH_3	0
#define TM_MONTH	12
#define TM_DAY_3	24
#define TM_DAY		31
#define TM_TIME		38
#define TM_DATE		39
#define TM_DEFAULT	40
#define TM_MERIDIAN	41

#define TM_UT		43
#define TM_DT		47
#define TM_SUFFIXES	51
#define TM_PARTS	55
#define TM_HOURS	62
#define TM_DAYS		66
#define TM_LAST		69
#define TM_THIS		72
#define TM_NEXT		75
#define TM_NOISE	78

#define TM_NFORM	82

struct tl{ // leap second info

    time_t  time;	/* the leap second event	*/
    int	    total;	/* inclusive total since epoch	*/
};

struct tz{   // timezone information

    char*	type;		/* type name (not used)		*/
    char*	standard;	/* standard time name		*/
    char*	daylight;	/* daylight or summertime name	*/
    short	west;		/* minutes west of GMT		*/
    short	dst;		/* add to tz.west for DST	*/

//  jfi extensions

    long        spring_ahead;   /* offset in seconds of dst start */
    long        fall_back;      /* offset in seconds of dst end */
    int         observes;       /* 1 iff dst is observed        */
};

extern int	  tm_flags_ATTLC;	/* tm library global flags	*/
extern char*	  tm_dform_ATTLC[];	/* default tmformat() strings	*/
extern char	  tm_digit_ATTLC[];	/* output format digits		*/
extern char**	  tm_form_ATTLC;	/* current tmformat() strings	*/
extern struct tl  tm_leap_ATTLC[];	/* leap second info		*/
extern char	  tm_lex_ATTLC[];	/* tm_form[] lex type classes	*/
extern struct tz* tm_local_ATTLC;	// local time zone info
extern short	  tm_mcnt_ATTLC[];	/* number of days in month i	*/
extern short	  tm_msum_ATTLC[];	/* total days in months before i*/
extern struct tz  tm_zone_ATTLC[];	/* alternate time zone table	*/

extern char*      mystrdup_Time_ATTLC(const char*);

extern "C"{
	time_t time(time_t* clock);
	long strtol(const char*, char**, int);
}

extern time_t	  tmdate_ATTLC(const char* date, char** end, time_t* clock);
extern struct tm* tmfix_ATTLC(struct tm* tm);
extern char*	  tmform_ATTLC(char* buf, char* format, time_t* clock);
extern int	  tmgoff_ATTLC(char* buf, char** end, int d);
//extern void	  tminit_ATTLC();
extern time_t	  tmleap_ATTLC(time_t* clock);
extern int	  tmlex_ATTLC(char* buf, char** end, char** tab, int n, char** suf, int n1);
extern struct tm* tmmake_ATTLC(time_t* clock);
extern char*	  tmpoff_ATTLC(char* buf, char* p, int m, int d);
extern time_t	  tmtime_ATTLC(struct tm* tm, int west);
extern char*	  tmtype_ATTLC(char* buf, char** end);
extern int	  tmword_ATTLC(char* buf, char** end, char* str, char** suf, int n);
extern int	  tmzone_ATTLC(char* buf, char** end, char* type, int* dst);

#endif
#endif
