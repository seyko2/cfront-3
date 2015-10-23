/* @(#) ipc.h 1.4 1/27/86 17:47:05 */
/*ident	"@(#)cfront:incl/sys/ipc.h	1.4"*/
/* Common IPC Access Structure */
// <sys/types.h> must be included.

struct ipc_perm {
	ushort	uid;	/* owner's user id */
	ushort	gid;	/* owner's group id */
	ushort	cuid;	/* creator's user id */
	ushort	cgid;	/* creator's group id */
	ushort	mode;	/* access modes */
	ushort	seq;	/* slot usage sequence number */
	key_t	key;	/* key */
};

/* Common IPC Definitions. */
/* Mode bits. */
#define	IPC_ALLOC	0100000		/* entry currently allocated */
#define	IPC_CREAT	0001000		/* create entry if key doesn't exist */
#define	IPC_EXCL	0002000		/* fail if key exists */
#define	IPC_NOWAIT	0004000		/* error if request must wait */

/* Keys. */
#define	IPC_PRIVATE	(key_t)0	/* private key */

/* Control Commands. */
#define	IPC_RMID	0	/* remove identifier */
#define	IPC_SET		1	/* set options */
#define	IPC_STAT	2	/* get options */


extern key_t ftok(const char*, char);
