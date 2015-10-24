/*ident	"@(#)Time:tm/tmform.c	3.1" */
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
#include <stdio.h>
#include "mytime.h"

#define elements(x)	(sizeof(x)/sizeof(x[0]))
#define warped(t,n)	((t)<((n)-6L*30L*24L*60L*60L)||(t)>((n)+60L*60L))

static char*		number(char*, long, int);

/*
 * format date given clock
 * end of buf is returned
 */

char* tmform_ATTLC(char* buf, char* format, time_t* clock) {
	register char*	cp;
	register char*	p;
	register int	n;
	int		c;
	int		i;
	int		flags;
	struct tm*	tp;
	char*		stack[4];
	char**		sp;
	time_t		now;

	tp = tmmake_ATTLC(clock);
/*
	cout 
	    << "in tmform...\n"
	    << "    tp->tm_sec="
	    << tp->tm_sec
	    << "\n"
	    << "    tp->tm_min="
	    << tp->tm_min
	    << "\n"
	    << "    tp->tm_hour="
	    << tp->tm_hour
	    << "\n"
	    << "    tp->tm_mday="
	    << tp->tm_mday
	    << "\n"
	    << "    tp->tm_mon="
	    << tp->tm_mon
	    << "\n"
	    << "    tp->tm_year="
	    << tp->tm_year
	    << "\n"
	    << "    tp->tm_wday="
	    << tp->tm_wday
	    << "\n"
	    << "    tp->tm_yday="
	    << tp->tm_yday
	    << "\n"
	    << "    tp->tm_isdst="
	    << tp->tm_isdst
	    << "\n"
	;
*/
	if (!format || !*format) format = tm_form_ATTLC[TM_DEFAULT];
	flags = tm_flags_ATTLC;
	sp = &stack[0];
	cp = buf;
	for (;;)
	{
		if (!(c = *format++))
		{
			if (sp <= &stack[0]) break;
			format = *--sp;
			continue;
		}
		if (c != '%')
		{
			*cp++ = c;
			continue;
		}
		switch (*format++)
		{
		case 0:
			format--;
			continue;
		case '%':
			*cp++ = '%';
			continue;
		case 'a':	/* abbreviated day of week name */
			n = TM_DAY_3 + tp->tm_wday;
			goto index;
		case 'A':	/* day of week name */
			n = TM_DAY + tp->tm_wday;
			goto index;
		case 'b':	/* abbreviated month name */
		case 'h':
			n = TM_MONTH_3 + tp->tm_mon;
			goto index;
		case 'B':	/* month name */
			n = TM_MONTH + tp->tm_mon;
			goto index;
		case 'c':	/* `ctime(3)' date sans newline */
			p = "%a %b %e %T %Y";
			goto push;
		case 'C':	/* `date(1)' date */
			p = "%a %b %e %T %Z %Y";
			goto push;
		case 'd':	/* day of month */
			cp = number(cp, (long)tp->tm_mday, 2);
			continue;
		case 'D':	/* date */
			p = "%m/%d/%y";
			goto push;
		case 'E':       /* no pad day of month */
			cp = number(cp, (long)tp->tm_mday, 0);
			continue;
		case 'e':       /* blank padded day of month */
			cp = number(cp, (long)tp->tm_mday, -2);
			continue;
		case 'H':	/* hour (0 - 23) */
			cp = number(cp, (long)tp->tm_hour, 2);
			continue;
		case 'i':	/* international `date(1)' date */
			p = "%a %b %e %T %z %Z %Y";
			goto push;
		case 'I':	/* hour (0 - 12) */
			if ((n = tp->tm_hour) > 12) n -= 12;
			else if (n == 0) n = 12;
			cp = number(cp, (long)n, 2);
			continue;
		case 'J':	/* Julian date (0 offset) */
			cp = number(cp, (long)tp->tm_yday, 3);
			continue;
		case 'j':	/* Julian date (1 offset) */
			cp = number(cp, (long)(tp->tm_yday + 1), 3);
			continue;
		case 'l':	/* `ls -l' date */
			p = "%b %e %H:%M";
			if (clock)
			{
				(void)time(&now);
				if (warped(*clock, now)) p = "%b %e  %Y";
			}
			goto push;
		case 'm':	/* month number */
			cp = number(cp, (long)(tp->tm_mon + 1), 2);
			continue;
		case 'M':	/* minutes */
			cp = number(cp, (long)tp->tm_min, 2);
			continue;
		case 'n':
			*cp++ = '\n';
			continue;
		case 'p':	/* meridian */
			n = TM_MERIDIAN + (tp->tm_hour >= 12);
			goto index;
		case 'r':
			p = "%I:%M:%S %p";
			goto push;
		case 'R':
			p = "%H:%M";
			goto push;
		case 'S':	/* seconds */
			cp = number(cp, (long)tp->tm_sec, 2);
			continue;
		case 't':
			*cp++ = '\t';
			continue;
		case 'T':
			p = "%H:%M:%S";
			goto push;
		case 'U':	/* week number, Sunday as first day */
			i = tp->tm_yday - tp->tm_wday;
		week:
			if (i >= -3 )
			{
				n = (i + 1) / 7 + 1;
				if (i % 7 >= 4) n++;
			}
			else n = 52;
			cp = number(cp, (long)n, 2);
			continue;
		case 'w':	/* weekday number */
			cp = number(cp, (long)tp->tm_wday, 0);
			continue;
		case 'W':	/* week number, Monday as first day */
			if (tp->tm_wday == 0) i = tp->tm_yday - 6;
			else i = tp->tm_yday - tp->tm_wday + 1;
			goto week;
		case 'x':
			p = tm_form_ATTLC[TM_DATE];
			goto push;
		case 'X':
			p = tm_form_ATTLC[TM_TIME];
			goto push;
		case 'y':	/* year in the form yy */
			cp = number(cp, (long)(tp->tm_year % 100), 2);
			continue;
		case 'Y':	/* year in the form ccyy */
			cp = number(cp, (long)(1900 + tp->tm_year), 4);
			continue;
		case 'z':	/* time zone type */
			if (!(tm_flags_ATTLC & TM_UTC) && (p = tm_local_ATTLC->type)) goto string;
			continue;
		case 'Z':	/* time zone */
			p = ( 
			    (tm_flags_ATTLC & TM_UTC) ?( 
			        tm_form_ATTLC[TM_UT] 
			    ):(
				tp->tm_isdst ?( 
				    tm_local_ATTLC->daylight 
				):( 
				    tm_local_ATTLC->standard
				)
			    )
			);
			goto string;
		case '-':
		case '+':
			switch (*format++)
			{
			case 0:
				format--;
				continue;
			case 'l':
				n = TM_LEAP;
				break;
			case 'u':
				n = TM_UTC;
				break;
			}
			if (c == '+')
			{
				if (!(tm_flags_ATTLC & n))
				{
					tm_flags_ATTLC |= n;
					tp = tmmake_ATTLC(clock);
				}
			}
			else if (tm_flags_ATTLC & n)
			{
				tm_flags_ATTLC &= ~n;
				tp = tmmake_ATTLC(clock);
			}
			continue;
		case '#':	/* seconds since the epoch */
			if (clock) now = *clock;
			else (void)time(&now);
			cp = number(cp, (long)now, 0);
			continue;
		default:
			*cp++ = c;
			*cp++ = *(format - 1);
			continue;
		}
	index:
		p = tm_form_ATTLC[n];
	string:
		while (*cp = *p++) cp++;
		continue;
	push:
		if (sp < &stack[elements(stack)])
		{
			*sp++ = format;
			format = p;
		}
		continue;
	}
	tm_flags_ATTLC = flags;
	*cp = 0;
	return(cp);
}

/*
 * format n with padding p into s
 * return end of s
 *
 * p:	<0	blank padding
 *	 0	no padding
 *	>0	0 padding
 */

static char* number(register char* s, register long n, register int p) {
	if (p > 0) (void)sprintf(s, "%0*lu", p, n);
	else if (p < 0) (void)sprintf(s, "%*lu", -p, n);
	else (void)sprintf(s, "%lu", n);
	while (*s) s++;
	return(s);
}
