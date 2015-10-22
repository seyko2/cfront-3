/* @(#) shm.h 1.2 1/27/86 17:47:10 */
/*ident	"@(#)cfront:incl/sys/shm.h	1.2"*/
/*
**	IPC Shared Memory Facility.
*/

/*
**	Implementation Constants.
*/

#define	SHMLBA	stob(1)	/* segment low boundary address multiple */
			/* (SHMLBA must be a power of 2) */

/*
**	Permission Definitions.
*/

#define	SHM_R	0400	/* read permission */
#define	SHM_W	0200	/* write permission */

/*
**	ipc_perm Mode Definitions.
*/

#define	SHM_INIT	01000	/* grow segment on next attach */
#define	SHM_DEST	02000	/* destroy segment when # attached = 0 */

/*
**	Message Operation Flags.
*/

#define	SHM_RDONLY	010000	/* attach read-only (else read-write) */
#define	SHM_RND		020000	/* round attach address to SHMLBA */

/*
**	Structure Definitions.
*/

/*
**	There is a shared mem id data structure for each segment in the system.
*/

struct shmid_ds {
	struct ipc_perm	shm_perm;	/* operation permission struct */
	int		shm_segsz;	/* size of segment in bytes */
	struct region	*shm_reg;	/* ptr to region structure */
	char		pad[4];		/* for swap compatibility */
	ushort		shm_lpid;	/* pid of last shmop */
	ushort		shm_cpid;	/* pid of creator */
	ushort		shm_nattch;	/* used only for shminfo */
	ushort		shm_cnattch;	/* used only for shminfo */
	time_t		shm_atime;	/* last shmat time */
	time_t		shm_dtime;	/* last shmdt time */
	time_t		shm_ctime;	/* last change time */
};

struct	shminfo {
	int	shmmax,		/* max shared memory segment size */
		shmmin,		/* min shared memory segment size */
		shmmni,		/* # of shared memory identifiers */
		shmseg,		/* max attached shared memory	  */
				/* segments per process		  */
		shmall;		/* max total shared memory system */
				/* wide (in clicks)		  */
};


/*
 * Shared memory control operations
 */

#define SHM_LOCK	3	/* Lock segment in core */
#define SHM_UNLOCK	4	/* Unlock segment */

//
extern char* shmat (int, char*, int);
extern int  shmctl (int, int, shmid_ds*),
            shmdt  (char*),
            shmget (key_t, int, int);
