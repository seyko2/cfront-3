/* @(#) mon.h 1.2 1/27/86 17:46:50 */
/*ident	"@(#)cfront:incl/mon.h	1.2"*/
struct hdr {
	char	*lpc;
	char	*hpc;
	int	nfns;
};

struct cnt {
	char	*fnpc;
	long	mcnt;
};

typedef unsigned short WORD;

#define MON_OUT	"mon.out"
#define MPROGS0	(150 * sizeof(WORD))	/* 300 for pdp11, 600 for 32-bits */
#define MSCALE0	4
#define NULL	0

typedef int (*PF) ();
extern  void monitor (PF, PF, WORD*, int, int);
