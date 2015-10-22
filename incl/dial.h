/* @(#) dial.h 1.3 1/27/86 17:46:38 */
/*ident	"@(#)cfront:incl/dial.h	1.3"*/
#ifndef IUCLC
#include <sys/termio.h>
#endif

#define LDEVS	"/usr/lib/uucp/L-devices"
#define SYSFILE "/usr/lib/uucp/L.sys"
#define DIALFILE "/usr/lib/uucp/L-dialcodes"
#define DEVDIR	"/dev/"			/* device path */
#define LOCK	"/usr/spool/uucp/LCK.."	/* lock file semaphore */
#define DVC_LEN	30	/* max NO of chars in TTY-device path name */

		/* error mnemonics */

#define	TRUE	1
#define FALSE	0
#define INTRPT	(-1)	/* interrupt occured */
#define D_HUNG	(-2)	/* dialer hung (no return from write) */
#define NO_ANS	(-3)	/* no answer within 10 seconds */
#define ILL_BD	(-4)	/* illegal baud-rate */
#define A_PROB	(-5)	/* acu problem (open() failure) */
#define L_PROB	(-6)	/* line problem (open() failure) */
#define NO_Ldv	(-7)	/* can't open LDEVS file */
#define DV_NT_A	(-8)	/* requested device not available */
#define DV_NT_K	(-9)	/* requested device not known */
#define NO_BD_A	(-10)	/* no device available at requested baud */
#define NO_BD_K	(-11)	/* no device known at requested baud */
#define DV_NT_E (-12)	/* requested speed does not match */

typedef struct {
	struct termio *attr;	/* ptr to termio attribute struct */
	int	baud;		/* transmission baud-rate */
	int	speed;		/* 212A modem: low=300, high=1200 */
	char	*line;		/* device name for out-going line */
	char	*telno;		/* ptr to tel-no digits string */
	int	modem;		/* allow modem control on direct lines */
	char	*device;	/* Will hold the name of the device used */
				/* to make a connection. */
	int	dev_len;	/* The length of the device used to make */
				/* a connection. */
} CALL;

extern int dial(CALL);
extern void undial(int);
