/* @(#) times.h 1.2 1/27/86 17:47:13 */
/*ident	"@(#)cfront:incl/sys/times.h	1.2"*/
/*
 * Structure returned by times()
 */
struct tms {
	time_t	tms_utime;		/* user time */
	time_t	tms_stime;		/* system time */
	time_t	tms_cutime;		/* user time, children */
	time_t	tms_cstime;		/* system time, children */
};

extern  long times (tms*);
