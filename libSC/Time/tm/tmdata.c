/*ident	"@(#)Time:tm/tmdata.c	3.1" */
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

#include <tm.h>

/*
 * default format strings -- must agree with TM_* indices
 */

char*		tm_dform_ATTLC[] =
{

//  TM_MONTH_3

/* 00 */	"Jan",		
/* 01 */	"Feb",		
/* 02 */	"Mar",		
/* 03 */	"Apr",
/* 04 */	"May",		
/* 05 */	"Jun",		
/* 06 */	"Jul",		
/* 07 */	"Aug",
/* 08 */	"Sep",		
/* 09 */	"Oct",		
/* 00 */	"Nov",		
/* 11 */	"Dec",

//  TM_MONTH

/* 12 */	"January",	
/* 13 */	"February",	
/* 14 */	"March",	
/* 15 */	"April",
/* 16 */	"May",		
/* 17 */	"June",		
/* 18 */	"July",		
/* 19 */	"August",
/* 20 */	"September",	
/* 21 */	"October",	
/* 22 */	"November",	
/* 23 */	"December",

//  TM_DAY_3

/* 24 */	"Sun",		
/* 25 */	"Mon",		
/* 26 */	"Tue",		
/* 27 */	"Wed",
/* 28 */	"Thu",		
/* 29 */	"Fri",		
/* 30 */	"Sat",

//  TM_DAY

/* 31 */	"Sunday",	
/* 32 */	"Monday",	
/* 33 */	"Tuesday",	
/* 34 */	"Wednesday",
/* 35 */	"Thursday",	
/* 36 */	"Friday",	
/* 37 */	"Saturday",

//  TM_TIME

/* 38 */	"%H:%M:%S",	

//  TM_DATE

/* 39 */	"%m/%d/%y",	

//  TM_DEFAULT

/* 40 */	"%a %b %e %T %Z %Y",

//  TM_MERIDIAN

/* 41 */	"AM",		
/* 42 */	"PM",

//  TM_UT

/* 43 */	"GMT",		
/* 44 */	"UTC",		
/* 45 */	"UCT",		
/* 46 */	"CUT",

//  TM_DT

/* 47 */	"DST",		
/* 48 */	"",		
/* 49 */	"",		
/* 50 */	"",

//  TM_SUFFIXES

/* 51 */	"s",		
/* 52 */	"es",		
/* 53 */	"",		
/* 54 */	"",

//  TM_PARTS

/* 55 */	"second",	
/* 56 */	"minute",	
/* 57 */	"hour",		
/* 58 */	"day",
/* 59 */	"week",		
/* 60 */	"month",	
/* 61 */	"year",

//  TM_HOURS

/* 62 */	"midnight",	
/* 63 */	"morning",	
/* 64 */	"noon",		
/* 65 */	"evening",

//  TM_DAYS

/* 66 */	"yesterday",	
/* 67 */	"today",	
/* 68 */	"tomorrow",

//  TM_LAST

/* 69 */	"last",		
/* 70 */	"ago",		
/* 71 */	"past",

//  TM_THIS

/* 72 */	"this",		
/* 73 */	"now",		
/* 74 */	"current",

//  TM_NEXT

/* 75 */	"next",		
/* 76 */	"hence",	
/* 77 */	"in",		

//  TM_NOISE

/* 78 */	"at",		
/* 79 */	"on",		
/* 80 */	"",
/* 81 */	"",

//  TM_NFORM (guard)

};

/*
 * tm_dform[] lex type classes
 */

char		tm_lex_ATTLC[] =
{
	TM_MONTH_3,	TM_MONTH_3,	TM_MONTH_3,	TM_MONTH_3,
	TM_MONTH_3,	TM_MONTH_3,	TM_MONTH_3,	TM_MONTH_3,
	TM_MONTH_3,	TM_MONTH_3,	TM_MONTH_3,	TM_MONTH_3,

	TM_MONTH,	TM_MONTH,	TM_MONTH,	TM_MONTH,
	TM_MONTH,	TM_MONTH,	TM_MONTH,	TM_MONTH,
	TM_MONTH,	TM_MONTH,	TM_MONTH,	TM_MONTH,

	TM_DAY_3,	TM_DAY_3,	TM_DAY_3,	TM_DAY_3,
	TM_DAY_3,	TM_DAY_3,	TM_DAY_3,

	TM_DAY,		TM_DAY,		TM_DAY,		TM_DAY,
	TM_DAY,		TM_DAY,		TM_DAY,

	TM_TIME,	TM_DATE,	TM_DEFAULT,

	TM_MERIDIAN,	TM_MERIDIAN,

	TM_UT,		TM_UT,		TM_UT,		TM_UT,
	TM_DT,		TM_DT,		TM_DT,		TM_DT,

	TM_SUFFIXES,	TM_SUFFIXES,	TM_SUFFIXES,	TM_SUFFIXES,

	TM_PARTS,	TM_PARTS,	TM_PARTS,	TM_PARTS,
	TM_PARTS,	TM_PARTS,	TM_PARTS,

	TM_HOURS,	TM_HOURS,	TM_HOURS,	TM_HOURS,

	TM_DAYS,	TM_DAYS,	TM_DAYS,

	TM_LAST,	TM_LAST,	TM_LAST,
	TM_THIS,	TM_THIS,	TM_THIS,
	TM_NEXT,	TM_NEXT,	TM_NEXT,

	TM_NOISE,	TM_NOISE,	TM_NOISE,	TM_NOISE,
};

/*
 * output format digits
 */

char		tm_digit_ATTLC[] = "0123456789";

/*
 * count of days in month i
 */

short		tm_mcnt_ATTLC[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

/*
 * sum of days in months before month i
 */

short		tm_msum_ATTLC[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };

/*
 * leap second time_t and accumulated adjustments
 * (reverse order -- biased for recent dates)
 *
 * tl.time is the seconds since the epoch for the leap event
 *
 *	adding:		the first additional second
 *	subtracting:	the first dissappearing second
 */

struct tl	tm_leap_ATTLC[] =
{
	  567993613,   14,		/* Dec 31 23:59:60 GMT 1987 */
	  489024012,   13,		/* Jun 30 23:59:60 GMT 1985 */
	  425865611,   12,		/* Jun 30 23:59:60 GMT 1983 */
	  394329610,   11,		/* Jun 30 23:59:60 GMT 1982 */
	  362793609,   10,		/* Jun 30 23:59:60 GMT 1981 */
	  315532808,    9,		/* Dec 31 23:59:60 GMT 1979 */
	  283996807,    8,		/* Dec 31 23:59:60 GMT 1978 */
	  252460806,    7,		/* Dec 31 23:59:60 GMT 1977 */
	  220924805,    6,		/* Dec 31 23:59:60 GMT 1976 */
	  189302404,    5,		/* Dec 31 23:59:60 GMT 1975 */
	  157766403,    4,		/* Dec 31 23:59:60 GMT 1974 */
	  126230402,    3,		/* Dec 31 23:59:60 GMT 1973 */
	   94694401,    2,		/* Dec 31 23:59:60 GMT 1972 */
	   78796800,    1,		/* Jun 30 23:59:60 GMT 1972 */
		  0,    0,		/* can reference (tl+1)     */
		  0,    0
};

/*
 * time zones
 *
 * the UTC entries must be first
 *
 * zones with the same type are contiguous with all but the
 * first entry for the type having a null type
 *
 * tz.standard is the sentinel
 */

struct tz	tm_zone_ATTLC[1];

