/* @(#) utmp.h 1.2 1/27/86 17:47:25 */
/*ident	"@(#)cfront:incl/utmp.h	1.2"*/

/*	<sys/types.h> must be included.					*/

#define	UTMP_FILE	"/etc/utmp"
#define	WTMP_FILE	"/etc/wtmp"
#define	ut_name	ut_user

struct utmp
  {
	char ut_user[8] ;		/* User login name */
	char ut_id[4] ; 		/* /etc/lines id(usually line #) */
	char ut_line[12] ;		/* device name (console, lnxx) */
	short ut_pid ;			/* process id */
	short ut_type ; 		/* type of entry */
	struct exit_status
	  {
	    short e_termination ;	/* Process termination status */
	    short e_exit ;		/* Process exit status */
	  }
	ut_exit ;			/* The exit status of a process
					 * marked as DEAD_PROCESS.
					 */
	time_t ut_time ;		/* time entry was made */
  } ;

/*	Definitions for ut_type						*/

#define	EMPTY		0
#define	RUN_LVL		1
#define	BOOT_TIME	2
#define	OLD_TIME	3
#define	NEW_TIME	4
#define	INIT_PROCESS	5	/* Process spawned by "init" */
#define	LOGIN_PROCESS	6	/* A "getty" process waiting for login */
#define	USER_PROCESS	7	/* A user process */
#define	DEAD_PROCESS	8
#define	ACCOUNTING	9

#define	UTMAXTYPE	ACCOUNTING	/* Largest legal value of ut_type */

/*	Special strings or formats used in the "ut_line" field when	*/
/*	accounting for something other than a process.			*/
/*	No string for the ut_line field can be more than 11 chars +	*/
/*	a NULL in length.						*/

#define	RUNLVL_MSG	"run-level %c"
#define	BOOT_MSG	"system boot"
#define	OTIME_MSG	"old time"
#define	NTIME_MSG	"new time"
