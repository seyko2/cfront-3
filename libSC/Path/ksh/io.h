/*ident "@(#)Path:ksh/io.h	3.1" */
/*
 *	UNIX shell
 *	S. R. Bourne
 *	rewritten by David Korn
 *
 */

#include	"sh_config.h"
#include	<sys/stat.h>
#ifdef _unistd_
#   include	<unistd.h>
#   ifdef R_OK_fcntl_h /* also defined in <fcntl.h> */
#		undef F_OK
#		undef R_OK
#		undef W_OK
#		undef X_OK
#   endif /* R_OK_fcntl_h */
#endif /* _unistd_ */
#ifdef _fcntl_
#   include	<fcntl.h>
#endif /* _fcntl_ */
#ifndef	O_CREAT
#   ifdef _sys_file_
#	ifdef R_OK_file_h /* also defined in <sys/file.h> */
#		undef F_OK
#		undef R_OK
#		undef W_OK
#		undef X_OK
#	endif /* R_OK_file_h */
#	include	<sys/file.h>
#   endif /* _sys_file_ */
#endif	/* O_CREAT */

#ifndef NFILE
#   define NFILE	20
#endif /* NFILE */
#ifndef IOBSIZE
#   define  IOBSIZE	1024
#endif /* IOBSIZE */
#ifndef EOF
#   define EOF		(-1)
#endif
#define MAXTRY		12
#ifdef SEVENBIT
#   define STRIP	0177
#else
#   define STRIP	0377
#endif /* SEVENBIT */

/* used for input and output of shell */
#define TMPSIZ		20
#define ERRIO		2
#define USERIO		10
#define FCIO		(NFILE-1) /* history file */
#ifdef KSHELL
#   define INIO		(NFILE-2) /* saved standard ioput */
#   define TMPIO	(NFILE-3) /* used for command substitution */
#   define CINPIPE	(NFILE-4) /* default inpipe for co-process */
#   define CINPIPE2	(NFILE-5) /* other end of inpipe for co-process */
#   define COTPIPE	(NFILE-6) /* default output pipe for co-process */
#   define MAXFILES	(NFILE-USERIO) /* maximum number of saved open files */

#   define F_STRING	((unsigned char)NFILE)	/* file number for incore files */
#   define F_INFINITE	0x7fff			/* effectively infinite */
#endif /* KSHELL */

/* SHELL file I/O structure */
struct fileblk
{
	char		*ptr;
	char		*base;
	char		*last;
	off_t		fseek;
	int		flag;
	unsigned char	fdes;
#ifdef KSHELL
	char		ftype;
	int		flast;
	char		**feval;
	struct fileblk	*fstak;
	unsigned	flin;
#endif /* KSHELL */
};

#define filenum(fp)	((int)(fp->fdes))	/* file number */
#define fnobuff(fp)	((fp)->flag&IONBF)	/* file is unbuffered */

#define IOREAD	0001
#define IOWRT	0002
#define IONBF	0004
#define IOFREE	0010
#define IOEOF	0020
#define IOERR	0040
#define IORW	0100
#define IOSLOW	0200
#define IOEDIT	0400

extern struct fileblk *io_ftable[NFILE];

#ifdef	FNDELAY
#   undef EAGAIN
#   define EAGAIN	EWOULDBLOCK
#   undef O_NONBLOCK
#   undef O_NDELAY
#   define O_NONBLOCK	FNDELAY
#endif	/* FNDELAY */
#ifndef	O_CREAT
#   define	O_CREAT		0400
#   define	O_TRUNC		01000
#   define	O_APPEND	010
#endif	/* O_CREAT */
#ifndef O_RDWR
#   define	O_RDONLY	0
#   define	O_WRONLY	01
#   define	O_RDWR		02
#endif /* O_RDWR */
#ifdef NOFCNTL
#   define	open		myopen
#   define	F_DUPFD		0
#   define	F_GETFD		1
#   define	F_SETFD		2
#endif /* F_DUPFD */
#ifndef R_OK
#   define F_OK 	   0	   /* does file exist */
#   define X_OK 	   1	   /* is it executable by caller */
#   define W_OK 	   2	   /* writable by caller */
#   define R_OK 	   4	   /* readable by caller */
#endif /* R_OK */
#ifndef SEEK_SET
#   define SEEK_SET	   0	   /* absolute offset */
#   define SEEK_CUR	   1	   /* relative offset */
#   define SEEK_END	   2	   /* EOF offset */
#endif /* SEEK_SET */
#ifdef S_IRUSR
#   define RW_ALL	(S_IRUSR|S_IRGRP|S_IROTH|S_IWUSR|S_IWGRP|S_IWOTH)
#else
#   define RW_ALL	0666
#endif /* S_IRUSR */


/*io nodes*/
#define INPIPE	0
#define OTPIPE	1
#define DUPFLG	0100

/*
 * The remainder of this file is only used when compiled with shell
 */

#ifdef KSHELL
/* possible values for ftype */
#define	F_ISSTRING	1
#define F_ISFILE	2
#define F_ISALIAS	3
#define F_ISEVAL	4


/* The following union is used for argument to sh_eval */
union io_eval
{
	int 	fd;
	char	**com;
};

#define io_unreadc(c)	(st.peekn |= (c)|MARK)
#define input		(st.curin)
#define output		(sh.curout)
#define newline()	p_char(NL)
#define fisopen(fd)	(io_access(fd,F_OK)==0)
#define fiswrite(fd)	(io_access(fd,W_OK)==0)
#define fisread(fd)	(io_access(fd,R_OK)==0)
#define fiseof(fp)	((fp)->flag&IOEOF)
#define fiserror(fp)	((fp)->flag&IOERR)
#define nextchar(fp)	(*((fp)->ptr))
#define finbuff(fp)	((fp)->last - (fp)->ptr)
#ifndef clearerr
#   define clearerr(fp)	((fp)->flag &= ~(IOERR|IOEOF))
#endif

struct filesave
{
	short	org_fd;
	short	dup_fd;
};


#ifdef PROTO
    extern off_t	lseek(int,off_t,int);
    extern void		io_clear(struct fileblk*);
    extern void 	io_fclose(int);
    extern int		io_getc(int);
    extern void 	io_init(int,struct fileblk*,char*);
    extern int		io_intr(struct fileblk*);
    extern void 	io_push(struct fileblk*);
    extern int		io_pop(int);
    extern int		io_mktmp(char*);
    extern off_t	io_seek(int,off_t,int);
    extern int		io_readbuff(struct fileblk*);
    extern int		io_readc(void);
    extern int		io_renumber(int,int);
    extern void 	io_sync(void);
    extern void 	io_popen(int[]);
    extern void 	io_pclose(int[]);
    extern void 	io_restore(int);
   struct ionod;
    extern int		io_redirect(struct ionod*,int);
    extern void 	io_save(int,int);
    extern void 	io_rmtemp(struct ionod*);
    extern void 	io_linkdoc(struct ionod*);
    extern void 	io_swapdoc(struct ionod*);
    extern int		io_fopen(const char*);
    extern void 	io_sopen(char*);
    extern int		io_access(int,int);
    extern int		io_nextc(void);
    extern int		ispipe(int);
#else
    extern off_t	lseek();
    extern void 	io_clear();
    extern void 	io_fclose();
    extern int		io_getc();
    extern void 	io_init();
    extern int		io_intr();
    extern void 	io_push();
    extern int		io_pop();
    extern int		io_mktmp();
    extern off_t	io_seek();
    extern int		io_readbuff();
    extern int		io_readc();
    extern int		io_renumber();
    extern void 	io_sync();
    extern void 	io_popen();
    extern void 	io_pclose();
    extern void 	io_restore();
    extern int		io_redirect();
    extern void 	io_save();
    extern void 	io_rmtemp();
    extern void 	io_linkdoc();
    extern void 	io_swapdoc();
    extern int		io_fopen();
    extern void 	io_sopen();
    extern int		io_access();
    extern int		io_nextc();
    extern int		ispipe();
#endif /* PROTO */
extern void	io_settemp();

extern char _sibuf[];
extern char	_sobuf[];
extern struct fileblk	io_stdin;
extern struct fileblk	io_stdout;
extern char	io_tmpname[];

/* the following are readonly */
extern const char	e_create[];
extern const char	e_file[];
extern const char	e_open[];
extern const char	e_pipe[];
extern const char	e_flimit[];
extern const char	e_fexists[];
extern const char	e_unknown[];
extern const char	e_endoffile[];
extern const char	e_devnull[];
extern const char	e_profile[];
extern const char	e_suidprofile[];
extern const char	e_sysprofile[];
extern const char	e_devfdNN[];
#ifdef SUID_EXEC
    extern const char	e_suidexec[];
#endif /* SUID_EXEC */
#endif /* KSHELL */
