/* @(#) stat.h 1.3 1/27/86 17:47:11 */
/*ident	"@(#)cfront:incl/sys/stat.h	1.3"*/
/*
 * Structure of the result of stat
 */

struct	stat
{
	dev_t	st_dev;
	ino_t	st_ino;
	ushort 	st_mode;
	short  	st_nlink;
	ushort 	st_uid;
	ushort 	st_gid;
	dev_t	st_rdev;
	off_t	st_size;
	time_t	st_atime;
	time_t	st_mtime;
	time_t	st_ctime;
};

#define	S_IFMT	0170000		/* type of file */
#define		S_IFDIR	0040000	/* directory */
#define		S_IFCHR	0020000	/* character special */
#define		S_IFBLK	0060000	/* block special */
#define		S_IFREG	0100000	/* regular */
#define		S_IFIFO	0010000	/* fifo */
#define	S_ISUID	04000		/* set user id on execution */
#define	S_ISGID	02000		/* set group id on execution */
#define	S_ISVTX	01000		/* save swapped text even after use */
#define	S_IREAD	00400		/* read permission, owner */
#define	S_IWRITE	00200		/* write permission, owner */
#define	S_IEXEC	00100		/* execute/search permission, owner */

extern int stat (const char*, stat*),
           fstat (int, stat*);
