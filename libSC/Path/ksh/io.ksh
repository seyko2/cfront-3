/*ident "@(#)Path:ksh/io.ksh	3.1" */
/*
 * UNIX shell
 *
 * S. R. Bourne
 * Rewritten by David Korn
 * AT&T Bell Laboratories
 *
 */

#include	<errno.h>
#include	"defs.h"
#include	"sym.h"
#include	"history.h"
#ifndef SIG_NORESTART
#   include	<signal.h>
    static jmp_buf readerr;
    static VOID interrupt();
#endif	/* SIG_NORESTART */

extern char *utos();

static struct filesave fdmap[MAXFILES];
static struct fileblk *iofree = &io_stdin;
static int nosave;
static int serial;
static char *temp_suffix;

/* ======== input output and file copying ======== */

/*
 * initialize temp file names
 */
void io_settemp(pid)
pid_t pid;
{
	register char *sp = sh_copy(utos((ulong)pid,10),io_tmpname+7);
	*sp++ = '.';
	temp_suffix = sp;
	serial = 0;
}

/*
 * set up a fileblk associated with the file descriptor fd using buffer buf
 */

void	io_init(fd,fp,buf)
register int	fd;
register struct fileblk *fp;
char *buf;
{
	register unsigned size = IOBSIZE+1;
	if(!fp)
	{
		if(iofree)
		{
			fp = iofree;
			iofree = 0;
			fp->flag = 0;
			buf = fp->base;
		}
		else
		{
			/* buffer the stream if possible */
			if(fd==0 && ispipe(0))
				size = 2;
			fp = new_of(struct fileblk,size);
			buf = (char*)(fp+1);
			fp->fstak = 0;
			fp->flag = IOFREE;
		}
	}
	else
		fp->flag = 0;
	fp->fdes = fd;
	fp->base = fp->ptr = fp->last = buf;
	*fp->ptr = 0;
	fp->flag |= IOREAD;
	fp->ftype=F_ISFILE;
	fp->flin=1;
	fp->fseek = 0;
	io_ftable[fd] = 0;
	if(io_access(fd,W_OK)==0)
		fp->flag |= IORW;
	io_ftable[fd] = fp;
	if(size==2 || ispipe(fd))
		fp->flag |= (IOSLOW|IONBF);
	else if( tty_check(fd))
		fp->flag |= IOSLOW;
}

/* 
 * push an iostream onto the input queue
 */

void io_push(fp)
struct fileblk *fp;
{
	fp->fstak=st.standin;
	st.standin=fp;
}

/*
 * pop an iostream from the input queue
 * close stream if flag==0, filenum>0 && filenum!=INIO and flag is a file.
 */

int io_pop(flag)
int flag;
{
	register struct fileblk *fp;
	register int fno;
	if((fp=st.standin) && fp->fstak)
	{
		fno = filenum(fp);
		if(flag==0 && fno>0 && fno!=F_STRING && fno!=INIO && fno!=sh.cpipe[INPIPE])
			io_fclose(fno);
		st.standin=fp->fstak;
		input = filenum(st.standin);
		if(fp->ftype == F_ISALIAS)
		{
			if(fp->feval)
				nam_offtype((struct namnod*)fp->feval,~M_FLAG);
			free((char*)fp);
		}
		return(1);
	}
	return(0);
}


/*
 * clear iostack up to <iop>
 */

void io_clear(iop)
register struct fileblk *iop;
{
	while((iop != st.standin) &&  io_pop(0));
}

/*
 * given a file descriptor f1, move it to a new file descriptor number
 * f2.  If f2 is open then it is closed first.
 * If the MARK bit not set on f2, then close on exec will be set for f2>2
 * The original stream is closed.
 * File numbers greater than 2 are marked close on exec if io_renumber is 
 *  invoked by a parent shell.
 *  The new file descriptor is returned;
 */

int io_renumber(f1,f2)
register int f1;
register int f2;
{
	register int flag = (f2&MARK);
	register int fs=0;
	f2 &= ~MARK;
	if(f2>2 && flag==0)
		fs = 1;
	if(f1!=f2)
	{
		if(fs==0)
			fs = fcntl(f2,F_GETFD,0);
		io_fclose(f2);
		f2 = fcntl(f1,F_DUPFD, f2);
		if(f2 < 0)
			sh_fail(e_file,NIL);
		if(io_ftable[f2] = io_ftable[f1])
			io_ftable[f2]->fdes = f2;
		io_ftable[f1] = 0;
		close(f1);
		if(f2==0)
			st.ioset = 1;
	}
	if(fs==1)
	/* set close on exec */
		fcntl(f2,F_SETFD,1);
	return(f2);
}

int io_mktmp(fname)
register char *fname;
{
	register int maxtry = MAXTRY;
	register char *tmp_name = io_tmpname;
	register int fd;
	do
	{
		sh_copy(sh_itos(++serial),temp_suffix);
	}
	while((fd=open(tmp_name,O_CREAT|O_RDWR,RW_ALL))<0 && maxtry--);
	if(fname)
	{
		sh_copy(tmp_name,fname);
		if(fd<=0)
			sh_fail(tmp_name,e_create);
	}
	io_ftable[fd] = 0;
	return(fd);
}
	 
/*
 * returns the next character from file <fd>
 */

io_getc(fd)
int fd;
{
	register struct fileblk *fp = io_ftable[fd];
	register int c;
	if(!fp)
		return(EOF);
	if(c= *fp->ptr++)
		return(c&STRIP);
	if(fp->ptr <= fp->last)
		return(0);
	return(io_readbuff(fp));
}

/*
 * This special version does not handle ptrname==1
 * It also saves a lot of real seeks on history file
 */

off_t io_seek(fd, offset, ptrname)
int fd;
off_t	offset;
register int	ptrname;
{
	register struct fileblk *fp;
	register int c;
	off_t	p;

	if(!(fp=io_ftable[fd]))
		return(lseek(fd,offset,ptrname));
	fp->flag &= ~IOEOF;
	if(!(fp->flag&IOREAD) && fp->ptr!=fp->base)
	{
		c = output;
		output = fd;
		p_flush();
		output = c;
	}
	c = 0;
	/* check history file to see if already in the buffer */
	if(ptrname==0 && (fp->flag&IOREAD) && offset<fp->fseek)
	{
		p = fp->fseek - (fp->last - fp->base);
		if(offset >= p)
		{
			fp->ptr = fp->base + (int)(offset-p);
			return(offset);
		}
		else
		{
			c = offset&(IOBSIZE-1);
			offset -= c;
		}
	}
	if(fp->flag&IORW)
	{
		fp->flag &= ~(IOWRT|IOREAD);
		fp->last = fp->ptr = fp->base;
		*fp->last = 0;
	}
	p = lseek(fd, offset, ptrname);
	if(!(fp->flag&IOSLOW))
	{
		if(ptrname==0)
			fp->fseek = p;
		else
			fp->fseek = -1;
		if(c)
		{
			io_readbuff(fp);
			fp->ptr += (c-1);
		}
	}
	return(p);
}

/*
 * Read from file into fp
 * Call the edit routines if necessary
 */

int io_readbuff(fp)
register struct fileblk *fp;
{
	register int n;
	register int fno;

	if (fp->flag & IORW)
		fp->flag |= IOREAD;

	if (!(fp->flag&IOREAD))
		return(EOF);

	/* The following line is needed in case an interrupt occurs */
	fp->ptr = fp->last;
#ifndef SIG_NORESTART
	if(fp->flag&IOSLOW)
	{
		st.intfn = interrupt;
		/* automatically restart with some systems */
		if(setjmp(readerr))
		{
			n = -1;
			errno = EINTR;
			goto slowsig;
		}
	}
#endif	/* SIG_NORESTART */
	fno = filenum(fp);
retry:
	/* don't attemp editing mode for pipes with allocated buffers */
	if(!(fp->flag&IOEDIT) || (fp->flag&IONBF))
		goto skip;
#   ifdef ESH
	if(is_option(EMACS|GMACS))
		n = emacs_read(fno,fp->base, (unsigned)(MAXLINE-2));
	else
#   endif	/* ESH */
#   ifdef VSH
	if(is_option(EDITVI))
		n = vi_read(fno,fp->base, (unsigned)(MAXLINE-2));
	else
#   endif	/* VSH */
	{
	skip:
		/* unbuffered reads needed for pipe on standard input */
		if(fnobuff(fp) && fno==0)
			n = 1;
		else
			n = IOBSIZE;
		if(fp->flag&(IOSLOW|IOEDIT))
		{
			job_wait((pid_t)0);
			p_flush();
		}
		n = read(fno,fp->base, (unsigned)n);
	}
#ifndef SIG_NORESTART
slowsig:
	if(fp->flag&IOSLOW)
		st.intfn = 0;
#endif	/* SIG_NORESTART */
	fp->ptr = fp->base;
	if(n > 0)
		fp->last = fp->base + n;
	else
		fp->last = fp->ptr;
	*fp->last = 0;
	if (n <= 0)
	{
		if (n == 0)
		{
#   ifdef O_NDELAY
			if((fp->flag&IOEDIT)&&(n=fcntl(fno,F_GETFL,0))&O_NDELAY)
			{
				n &= ~O_NDELAY;
				fcntl(fno, F_SETFL, n);
				goto retry;
			}
#   endif /* O_NDELAY */
			fp->flag |= IOEOF;
			if (fp->flag & IORW)
				fp->flag &= ~IOREAD;
		}
		else
		{
#   ifdef O_NONBLOCK
			if(errno == EAGAIN)
			{
				n = fcntl(fno,F_GETFL,0);
				n &= ~O_NONBLOCK;
				fcntl(fno, F_SETFL, n);
				goto retry;
			}
#   endif /* O_NONBLOCK */
			fp->flag |= IOERR;
		}
		return(EOF);
	}
	if(!(fp->flag&IOSLOW))
		fp->fseek += n;
	/* this is needed to handle read -s */
	if((st.states&RWAIT) && fno!=hist_ptr->fixfd)
	{
		n = output;
		p_setout(hist_ptr->fixfd);
		p_str(fp->base,0);
		output = n;
	}
	return(*fp->ptr++&STRIP);
}


/*
 * close file stream and reopen for reading and writing
 */

void io_fclose(fd)
register int fd;
{
	register struct fileblk *fp = io_ftable[fd];
	/* reposition seek pointer if necessary */
	if(fp && !(fp->flag&IOREAD))
		p_flush();
	else if(fp)
	{
		register int count;
		if(!(st.states&FORKED) && (count=fp->last-fp->ptr))
			lseek(filenum(fp),-((off_t)count),SEEK_CUR);
		*(fp->last = fp->ptr = fp->base) = 0;
	}
	if(fd==COTPIPE)
		sh.cpid = 0;
	close(fd);
	if(fp == &io_stdin)
		iofree = fp;
	else if(fp && (fp->flag&IOFREE))
		free((char*)fp);
	io_ftable[fd] = 0;
}

/*
 * Open a file for reading
 * On failure, print message.
 */

int io_fopen(name)
register const char *name;
{
	register int	fd;
	if((fd=open(name,O_RDONLY))<0)
		sh_fail(name,e_open);
	return(fd);
}

/*
 * set up an I/O stream that will cause reading from a string
 */

void	io_sopen(s)
register char *s;
{
	register struct fileblk *fp;
	(fp=st.standin)->fdes = input = F_STRING;
	fp->flag = IOREAD;
	fp->base = fp->ptr = s;
	fp->flin = 1;
	fp->ftype=F_ISSTRING;
	fp->flag|=(s==0?IOEOF:0);
}

/*
 * create a pipe and print message on failure
 */

void	io_popen(pv)
register int	pv[];
{
	if(pipe(pv)<0 || pv[INPIPE]<0 || pv[OTPIPE]<0)
		sh_fail(e_pipe,NIL);
}

/*
 * close a pipe
 */

void io_pclose(pv)
register int pv[];
{
	if(pv[INPIPE]>=2)
		io_fclose(pv[INPIPE]);
	if(pv[OTPIPE]>=2)
		io_fclose(pv[OTPIPE]);
	pv[INPIPE] = pv[OTPIPE] = -1;
}
/*
 * io_sync - flushes output buffer and positions stdin if necessary
 */

void io_sync()
{
	register struct fileblk *fp = io_ftable[0];
	register int count;
	p_setout(ERRIO);
	p_flush();
	/* position back the read-ahead characters */
	if(fp)
	{
		/*@ assert fp->base!=0; @*/
		if(count=fp->last-fp->ptr)
			lseek(filenum(fp),-((off_t)count),SEEK_CUR);
		fp->ptr = fp->last = fp->base;
		*fp->ptr = 0;
	}
}

/*
 * create a link to iodoc for child process to use
 */

void io_linkdoc(i)
register struct ionod	*i;
{
	register int maxtry;
	while(i)
	{
		/* link to a tempory file name */
		maxtry = MAXTRY;
		do
		{
			sh_copy(sh_itos(++serial),temp_suffix);
		}
		while((link(i->ioname,io_tmpname))< 0 && maxtry--);
		if(maxtry<=0)
			sh_fail(io_tmpname,e_create);
		free(i->iolink);
		i->iolink = sh_heap(io_tmpname);
		i = i->iolst;
	}
}


/*
 * rename the file with the link name of the parent
 */

void	io_swapdoc(i)
register struct ionod	*i;
{
	while(i)
	{
		free(i->ioname);
		i->ioname = i->iolink;
		i->iolink = 0;
		i = i->iolst;
	}
}




/*
 * I/O redirection
 * flag = 0 if files are to be restored
 * flag = 2 if files are to be closed on exec
 */

int	io_redirect(iop,flag)
struct ionod	*iop;
{
	register char *ion;
	register int 	iof;
	register int	fd;
	int o_mode;
	static char io_op[5];
	char fname[TMPSIZ];
	int fn;
	int mark = MARK;
	int indx = sh.topfd;
	int traceon;
	if(flag==2)
		mark = 0;
	if(iop)
		traceon = sh_trace((char**)0,0);
	for(;iop;iop=iop->ionxt)
	{
		iof=iop->iofile;
		if(flag==0)
		{
			/* save current file descriptor */
			io_save(iof&IOUFD,indx);
		}
		ion=iop->ioname;
		io_op[0] = '0'+(iof&IOUFD);
		if(iof&IOPUT)
		{
			io_op[1] = '>';
			o_mode = O_WRONLY|O_CREAT;
		}
		else
		{
			io_op[1] = '<';
			o_mode = O_RDONLY;
		}
		io_op[2] = 0;
		io_op[3] = 0;
		if(!(iof&IORAW))
			ion=mac_trim(ion,1);
		if(*ion)
		{
			if(iof&IODOC)
			{
				if(traceon)
				{
					p_setout(ERRIO);
					io_op[2] = '<';
					p_str(io_op,NL);
				}
				fd = mac_here(iop);
				ion = 0;
			}
			else if(iof&IOMOV)
			{
				io_op[2] = '&';
				if(ion[1]!=0)
					goto fail;
				fd = (iof&IOUFD);
				fn = *ion;
				if(fn=='-')
				{
					io_fclose(fd);
					continue;
				}
				if(fn == 'p')
				{
					if(iof&IOPUT)
						fn = COTPIPE;
					else
						fn =  CINPIPE;
				}
				else if(isdigit(fn))
					fn -= '0';
				else
					goto fail;
				if((fd=dup(fn))<0)
					goto fail;
				if(fn==COTPIPE)
					io_fclose(COTPIPE);
				else if(fn==CINPIPE)
					io_pclose(sh.cpipe);
			}
			else if(iof&IORDW)
			{
				io_op[2] = '>';
				o_mode = O_RDWR|O_CREAT;
				goto openit;
			}
			else if((iof&IOPUT)==0)
				fd=io_fopen(ion);
			else if(is_option(RSHFLG))
				sh_fail(ion,e_restricted);
			else
			{
				if(iof&IOAPP)
				{
					io_op[2] = '>';
					o_mode |= O_APPEND;
				}
				else
				{
					o_mode |= O_TRUNC;
					if(iof&IOCLOB)
						io_op[2] = '|';
					else if(is_option(NOCLOB) && test_type(ion,S_IFMT,S_IFREG))
						sh_fail(ion,e_fexists);
				}
			openit:
				if((fd=open(ion,o_mode,RW_ALL))<0)
					sh_fail(ion,e_create);
			}
			if(traceon && ion)
			{
				p_setout(ERRIO);
				p_str(io_op,' ');
				p_str(ion,iop->ionxt?SP:NL);
			}
			io_renumber(fd,(iof&IOUFD)|mark);
		}
	}
	return(indx);
fail:
	sh_fail(ion,e_file);
	/* NOTREACHED */
}

/*
 * copy file fd into a save place
 */

void io_save(fd,oldtop)
register int fd;
{
	register int	f = sh.topfd;
	/* see if already saved, only save once */
	while(f > oldtop)
	{
		if((fdmap[--f].org_fd)>>1 == fd)
			return;
	}
	if(fd==output)
		p_flush();
	else if(fd==0)
		io_sync();
	f = fcntl(fd, F_DUPFD, USERIO);
	if(sh.topfd >= MAXFILES)
		sh_fail(e_flimit,NIL);
	if(f >= 0)
	{
		io_ftable[f] = io_ftable[fd];
		io_ftable[fd] = 0;
		fd <<= 1;
		/* make sure saved file close-on-exec */
		if(fcntl(f,F_GETFD,0)==0)
		{
			/* the low bit on fd set to restore close-on-exec */
			fd |=  1;
			fcntl(f,F_SETFD,1);
		}
	}
	fdmap[sh.topfd].org_fd = fd;
	fdmap[sh.topfd++].dup_fd = f;
	return;
}

/*
 *  restore saved file descriptors from <last> on
 */

void	io_restore(last)
int	last;
{
	register int 	i;
	register int	dupfd;
	register int	fd;
	int flag;

	for (i = sh.topfd - 1; i >= last; i--)
	{
		fd = fdmap[i].org_fd;
		if ((dupfd = fdmap[i].dup_fd) > 0)
		{
			flag = fd&1;
			fd >>= 1;
			io_renumber(dupfd, fd);
			/* turn off close-on-exec if flag is set */
			if(flag)
				fcntl(fd,F_SETFD,0);
		}
		else
			io_fclose(fd);
	}
	sh.topfd = last;
}


/*
 * This routine returns 1 if fd corresponds to a pipe, 0 otherwise.
 */

int ispipe(fno)
register int fno;
{
#ifdef PIPE_ERR
	if(lseek(fno,(off_t)0,SEEK_CUR)>=0)
		return(0);
	if(errno==PIPE_ERR)
		return(!tty_check(fno));
	else
		return(0);
#else
	return(0);
#endif /* PIPE_ERR */
}




/*
 * This routine returns the next input character but strips shell
 * line continuations and issues prompts at end of line
 * Otherwise this routine is the same as io_readc()
 */

io_nextc()
{
	register int c;
retry:
	nosave = 0;
	if((c=io_readc())==ESCAPE)
	{
		nosave = 1;
		if(io_readc()==NL)
		{
			sh_prompt(0);
			goto retry;
		}
		st.standin->ptr--;
	}
	nosave = 0;
	return(c);
}

int io_intr(fp)
register struct fileblk *fp;
{
	clearerr(fp);
	if(sh.trapnote&SIGSET)
	{
		newline();
		sh_exit(SIGFAIL);
	}
	if(fp->flag&IOSLOW)
	{
#ifdef JOBS
		job_wait((pid_t)0);
#endif /* JOBS */
		if(sh.trapnote&TRAPSET)
		{
			newline();
			fp = st.standin;
			sh_chktrap();
			io_clear(fp);
			return(0);
		}
	}
	return(-1);
}

#ifndef SIG_NORESTART
/*
 * This routine is for systems that automatically restarts read
 */

static VOID interrupt()
{
		st.intfn = 0;
		longjmp(readerr,1);
}
#endif	/* SIG_NORESTART */

/*
 * read a character
 */

io_readc()
{
	register int c;
	register struct fileblk	*fp;
	int maxtry;
	if(st.peekn)
	{
		c = st.peekn&~MARK;
		st.peekn = 0;
		return(c);
	}
	maxtry = MAXTRY;
	fp = st.standin;
retry:
	c = (*fp->ptr++)&STRIP;
retry2:
	if(c==0)
	{
		switch(fp->ftype)
		{
		case F_ISALIAS:
			if(fp->flast == 0)
			{
			popit:
				io_pop(1);
				return(io_readc());
			}
			/* does it end in whitespace? */
			c = *(--fp->ptr-1);
			if(isblank(c))
				sh.wdset |= CAN_ALIAS;
			else if(c==ESCAPE && fp->flast==NL)
				goto popit;
			c = (fp->flast&~MARK);
			fp->flast = 0;
			if(c)
				goto retry2;
			return(ENDOF);

		case F_ISEVAL:
			io_sopen(*fp->feval++);
			if(*fp->feval)
				fp->ftype = F_ISEVAL;
			c = SP;
			break;

		case F_ISSTRING:
			io_sopen(NULLSTR);
			fp->flag |= IOEOF;
			break;

		case F_ISFILE:
			/* check for end-of-buffer */
			if(fp->ptr>fp->last)
			{
				if((c = io_readbuff(fp)) != EOF)
					goto retry2;
				c = ENDOF;
				if(!fiseof(fp))
				{
					if(io_intr(fp)<0)
					{
						if(--maxtry > 0)
							goto retry;
						else
							fp->flag |= IOERR;
					}
				}
				break;
			}
			/* treat a zero byte as eof for TMPIO */
			if(filenum(fp) == TMPIO)
			{
				lseek(TMPIO,(off_t)0,SEEK_SET);
				io_ftable[TMPIO] = 0;
				break;
			}
			/* assume foreign binary and don't execute */
			if(sh.readscript)
			{
				register char *cp = st.cmdadr;
				st.cmdadr = sh.readscript;
				sh_fail(cp,e_exec);
			}
		default:
			/* skip over null bytes in files */
			goto retry;
		}
	}
	else
	{
		if(c==NL)
		{
			fp->flin++;
			nosave = 0;
		}
		if(!nosave)
		{
			if((st.states&READPR) && (fp->ftype!=F_ISALIAS) &&
				(fp->fstak==0||(st.states&FIXFLG)))
			{
				p_setout(ERRIO);
				p_char(c);
			}
			if((st.states&FIXFLG)&&fp->ftype==F_ISFILE && hist_ptr)
			{
				fp = hist_ptr->fixfp;
				if(fp->ptr >= fp->last)
				{
					int savout = output;
					output = filenum(fp);
					p_flush();
					output = savout;
					hist_ptr->fixflush++;
				}
				*fp->ptr++ = c;
			}
		}
	}
	return(c);
}

/*
 * remove temporary files
 */

void	io_rmtemp(base)
struct ionod	*base;
{
	register struct ionod *iop = st.iotemp;
	while(iop>base)
	{
		unlink(iop->ioname);
		free(iop->iolink);
		iop=iop->iolst;
	}
	st.iotemp = iop;
}


/*
 * returns access information on open file <fd>
 * returns -1 for failure, 0 for success
 * <mode> is the same as for access()
 */

io_access(fd,mode)
register int mode;
{
	register int flags;
	register struct fileblk *fp;
#ifndef F_GETFL
	struct stat statb;
#endif /* F_GETFL */
	if(mode==X_OK)
		return(-1);
	if(fp=io_ftable[fd])
	{
		if(mode==F_OK)
			return(0);
		if(mode==R_OK && fp->flag&(IORW|IOREAD))
			return(0);
		if(mode==W_OK && fp->flag&(IORW|IOWRT))
			return(0);
		return(-1);
	}
#ifdef F_GETFL
	flags = fcntl(fd,F_GETFL,0);
#else
	flags = fstat(fd,&statb);
#endif /* F_GETFL */
	if(flags < 0)
		return(-1);
#ifdef F_GETFL
	if(mode==R_OK && (flags&1))
		return(-1);
	if(mode==W_OK && !(flags&3))
		return(-1);
#endif /* F_GETFL */
	return(0);
}

#ifdef NOFCNTL
#   ifdef _sys_ioctl_
#	include	<sys/ioctl.h>
#   endif /* _sys_ioctl_ */

int fcntl(f1,type,arg)
register int arg;
{
	struct stat statbuf;
	if(type==F_DUPFD)
	{
		register int fd;
		/* find first non-open file */
		while(arg < NFILE &&  (fstat(arg,&statbuf)>=0))
			arg++;
		if(arg >= NFILE)
			return(-1);
		fd = dup2(f1, arg);
		return(fd);
	   }
#   ifdef FIOCLEX
	else if(type==F_SETFD)
		ioctl(f1, arg?FIOCLEX:FIONCLEX, 0);
#   endif /* FIOCLEX */
	else 
		return(0);
}

#undef open

/*
 * Some braindamaged systems don't have O_CREAT
 */

int myopen(name,flag,mode)
register char *name;
register int flag;
{
	register int fd;
	int created = 0;
	if(flag&O_TRUNC)
	{
	filecreate:
		created++;
		fd = creat(name,O_CREAT?mode:0644);
	}
	else
	{
		fd = open(name,flag&(O_RDWR|O_RDONLY|O_WRONLY));
		if(fd<0 && O_CREAT)
			goto filecreate;
	}
	if(fd<0)
		return(fd);
	if(created&&O_RDWR)
	{
		close(fd);
		return(myopen(name,flag&~(O_CREAT|O_TRUNC)));
	}
	else if(flag&O_APPEND)
		lseek(fd,(off_t)0,SEEK_END);
	return(fd);
}
#endif	/* _fcntl_ */

#ifdef DBUG
/*
 * dump an io structure using only writes
 */

int_write(n,base)
{
	register char *cp;
	extern char *utos();
	cp = utos(n,base);
	write(2,cp,strlen(cp));
}

io_dump(n)
int n;
{
	struct fileblk *fp = io_ftable[n];;
	write(2,"dump ",5);
	int_write(n,10);
	if(fp)
	{
		write(2,":fp=",4);
		int_write(fp,16);
		write(2," base=",6);
		int_write(fp->base,16);
		write(2," ptr=",5);
		int_write(fp->ptr,16);
		write(2," last=",6);
		int_write(fp->last,16);
		write(2," flag=",6);
		int_write(fp->flag,8);
		write(2," *last=",7);
		int_write(*fp->last&0xff,10);
		write(2,"\n",1);
	}
	else
		write(2,":no file structure\n",20);
}
#endif /* DBUG */
	
