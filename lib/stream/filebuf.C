/*ident	"@(#)cls4:lib/stream/filebuf.c	1.7" */
/*******************************************************************************
 
C++ source for the C++ Language System, Release 3.0.  This product
is a new release of the original cfront developed in the computer
science research center of AT&T Bell Laboratories.

Copyright (c) 1993  UNIX System Laboratories, Inc.
Copyright (c) 1991, 1992 AT&T and UNIX System Laboratories, Inc.
Copyright (c) 1984, 1989, 1990 AT&T.  All Rights Reserved.

THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE of AT&T and UNIX System
Laboratories, Inc.  The copyright notice above does not evidence
any actual or intended publication of such source code.

*******************************************************************************/

#include <iostream.h>
#include <fstream.h>

#include "streamdefs.h"

#ifdef STDIO_ONLY 
#include <stdio.h>

static int open(const char* name,int flags, int prot = 0) ;
static int creat(const char* name, int prot ) ;
static streampos lseek(int fd, long, ios::seek_dir) ;
static int read(int fd, char* b, int len) ;
static int write(int fd, char* b, int len) ;
static int close(int fd) ;
#else

#include <osfcn.h>

#ifndef O_RDONLY
#include <fcntl.h>
#endif

#endif /* STDIO_ONLY */

#ifndef O_RDONLY
#	define O_RDONLY 0 
#endif

#ifndef O_WRONLY
#	define O_WRONLY 1
#endif

#ifndef O_RDWR
#	define O_RDWR 2
#endif

#ifndef O_CREAT
#	define O_CREAT 00400
#endif

#ifndef O_TRUNC
#	define O_TRUNC 01000
#endif

#ifndef O_EXCL
#	define O_EXCL 02000
#endif

#ifndef O_APPEND
	static const int NOSYSAPPEND = 1 ;
#	define O_APPEND 010
#else
	static const int NOSYSAPPEND = 0 ;
#endif

//#include <errno.h>
//extern int errno ;
#include "errno.h"

const int filebuf::openprot = 0644 ;

static const int input=ios::in ;
static const int output=ios::out ;
static const int append=ios::app ;
static const int atend=ios::ate ;
static const int tcate=ios::trunc ;
static const int nocr=ios::nocreate ;
static const int norep=ios::noreplace ;

static const int seek_beg=ios::beg ;
static const int seek_end=ios::end ;
static const int seek_cur=ios::cur ;

int filebuf::last_op() 
{
	return (pptr()?output:(gptr()?input:0)) ;
}

inline void save_errno(int& orig)
{
//	orig = ::errno ;
//	::errno = 0 ;
	orig = errno ;
	errno = 0 ;
}

inline int restore_errno(int& orig)
{
//	if ( ::errno == 0 ) ::errno = orig ;
	if ( errno == 0 ) errno = orig ;
	return EOF ;
}
	
/*
 *	Open a file with the given mode.
 *	Return:		NULL if failure
 *			this if success
 */
filebuf* filebuf::open (const char *name, int om, int prot)
{
	int errno_orig ;
	save_errno(errno_orig) ;
	if ( om&append ) om |= output ;

	if ( opened ) {
		restore_errno(errno_orig) ;
		return 0 ;
		}

        int flags = 0 ;
	if ( om&append ) flags |= O_APPEND ;

	/* ios::nocreate and ios::noreplace should be mutually exclusive */
	if ( (om&(output|nocr|norep)) == (output|nocr|norep) )
		return 0 ;

	switch (om&(input|output)) {
	case input : 
		flags |=  O_RDONLY ;
		xfd = ::open(name,flags) ;
		if ( xfd >= 0 && (om&atend)) lseek(xfd,0,2) ;
		break ;

	case output :
		flags |= O_WRONLY|O_CREAT|O_TRUNC ;
		if ( om&nocr ) flags &= ~O_CREAT ;
		else if ( om&norep ) flags |= O_EXCL ;
		if ( om&append ) flags |= O_APPEND ;
		if ( om&(append|atend) ) flags &= ~O_TRUNC ;
		if ( flags == (O_WRONLY|O_CREAT|O_TRUNC) ) {
			xfd = ::creat(name,prot) ;
			}
		else {
			xfd = ::open(name,flags,prot) ;
			}

		if ( xfd >= 0 && (om&(atend|append))) lseek(xfd,0,2) ;
		break ;

	case input|output:
		flags |= O_RDWR|O_CREAT ;
		if ( om&tcate ) flags |= O_TRUNC ;
		if ( om&append ) flags |= O_APPEND ;
		if ( om&nocr ) flags &= ~O_CREAT ;
		else if ( om&norep ) flags |= O_EXCL;
		xfd = ::open(name,flags,prot) ;
		if ( xfd >= 0 && (om&(atend|append))) lseek(xfd,0,2) ;
		break;
	}

	if (xfd < 0) return 0;

	opened = 1;
	setp(0,0) ;
	setg(0,0,0) ;
	mode = om ;
	last_seek = EOF ;
	restore_errno(errno_orig) ;
	return this;
}

/*
 *  getmode_from_fd() gets the current mode of the
 *  file descriptor (using fcntl()), then builds up
 *  the mode value that will be needed for a filebuf
 *  object
 */
static int getmode_from_fd(int fd) {
	int status_flags, retval;
	retval = 0;
	if ((status_flags = fcntl(fd, F_GETFL, 0)) != -1) {
		switch (status_flags & 03) {
		case O_RDONLY:
			retval |= input;
			break;
		case O_WRONLY:
			retval |= output;
			break;
		case O_RDWR:
			retval |= input | output;
			break;
		}
		if (status_flags & O_APPEND) {
			retval |= append;
		}
	}
	return (retval);
}

filebuf* filebuf::attach (int f)
{
	if ( opened ) return 0 ;
	xfd = f;
	opened = 1;
	setp(0,0) ;
	setg(0,0,0) ;
	mode=0 ;
	last_seek= EOF;
	return this;
}

int filebuf::detach ()
{
	int f = xfd;
	if ( !opened ) return -1 ;
	if (last_op() == output) overflow();
	opened = 0;
	xfd = -1;
	setp(0,0) ;
	setg(0,0,0) ;
	mode=0 ;
	last_seek= EOF;
	return f;
}

/*
 *	Empty an output buffer.
 *	Returns:	EOF on error
 *			0 on success
 */
int filebuf::overflow(int c)
{
	int errno_orig ;
	save_errno(errno_orig) ;
	if ( !opened ) return restore_errno(errno_orig) ;
	if ( allocate() == EOF ) return restore_errno(errno_orig) ;
	if ( last_op() == input ) {
		if ( sync()==EOF ) return restore_errno(errno_orig) ;
	}

	register char* p = base() ;
	// pptr()==NULL does not imply p < pptr(),  so we need separate
	// test.
	while ( pptr() && p < pptr() )  {
		if ( NOSYSAPPEND && (mode&append) ) {
			// System doesn't have an append mode, so approximate
			// as best we can.
			lseek(xfd,0,2) ;
			}
		/* Partial writes are sometimes possible in peculiar
		 * circumstances */
		register int count = write(xfd,p,pptr()-p) ;
		if ( count < 0 ) {
			last_seek = EOF ;
			return restore_errno(errno_orig) ;
		}
		p += count ;

		if ( SEEK_ARITH_OK	
				&& last_seek != EOF
				&& mode && !(mode&append)
				&& count >= 0 ) {
			last_seek += count ;
		} else {
			last_seek = EOF ;
		}

		if ( count < 0  ) return restore_errno(errno_orig) ;
	}

	setp(base(),ebuf()) ;
	setg(0,0,0);

	if ( c == EOF ) /* don't do anything */ ;
	else if ( unbuffered() ) {
		char ch = c;
		last_seek = EOF ;
//		while ( write(xfd,&ch,1)!=1 && ::errno==0 ) ;
		while ( write(xfd,&ch,1)!=1 && errno==0 ) ;
//		if ( ::errno != 0 ) return restore_errno(errno_orig) ;
		if ( errno != 0 ) return restore_errno(errno_orig) ;
		}
	else {
		sputc(c) ;
		}

	restore_errno(errno_orig) ;
	return zapeof(c) ;
}


/*
 *	Fill an input buffer.
 *	Returns:	EOF on error or end of input
 *			next character on success
 */
int filebuf::underflow()
{
	int count;

	if ( !opened ) return EOF ;
	if ( allocate() == EOF ) return EOF ;
	if ( last_op() == output ) {
		if ( sync()==EOF ) return EOF ;
	}
	int orig_errno ;
	save_errno(orig_errno) ;
	setp(0,0) ;
	if ( unbuffered() ) {
		last_seek=EOF ;
		count = read(xfd,&lahead[0],1) ;
		setg(&lahead[0],&lahead[0],&lahead[count]) ;
		if ( count <= 0 ) return EOF ;
		} 
	else {
		register int	rdsize ;
		if ( blen() > 2*sizeof(long) ) {
			/* gptr must be set greater than base to
			 * guarantee at least 1 char of pushback.
			 * putting it farther will tend in many common
			 * cases to keep things aligned. 
			 */
			in_start = base()+sizeof(long) ;
			rdsize = blen()-sizeof(long) ;
		} else {
			in_start = base()+1 ;
			rdsize = blen()-1 ;
		}
		count = read(xfd,in_start,rdsize) ;
//		while ( count<=0 && ::errno==EINTR ) {
		while ( count<=0 && errno==EINTR ) {
			/*
			 * Signal caught and returned before any data
			 * transfered. 	
			 */
//			::errno = 0 ;
			errno = 0 ;
			count = read(xfd,in_start,rdsize) ;
		}

		if ( SEEK_ARITH_OK	
				&& last_seek != EOF
				&& mode && !(mode&append) 
				&& count >= 0 ) {
			last_seek += count ;
		} else {
			last_seek = EOF ;
		}

		if ( count <= 0 ) {
			setg(0,0,0) ;
			return restore_errno(orig_errno) ;
			}
		setg(base(),in_start,in_start+count) ;
	}
	
	restore_errno(orig_errno) ;	
	return zapeof(*gptr());
}

filebuf* filebuf::close()
{
	int f = xfd ;
	if ( !opened ) {
		return 0 ;
	}
	if (last_op()==output) overflow();
	setg(0,0,0) ;
	setp(0,0) ;
	opened = 0 ;
	xfd = -1 ;
	last_seek = EOF ;
	if ( mode != 0 ) {
		mode = 0 ;
		int orig_errno ;
		save_errno(orig_errno) ;
		int ok = ::close(f);
		restore_errno(orig_errno) ;
		return ( ok==EOF ? 0 : this ) ;
	} else {
		return this ; 
	}
}

int filebuf::sync() 
{
	int ok = 0 ;
	int orig_errno ;
	save_errno(orig_errno) ;
	switch(last_op()) {
	case input: 
		last_seek = lseek(xfd,gptr()-egptr(),seek_cur) ;
		if ( last_seek < 0 ) {
			ok = EOF ;
			last_seek = EOF ;
			}
		break ;
	case output:
		ok = overflow() ;
		/* This does not result in infinite recursion even though
		 * under some circumstances overflow might call sync.
		 * it explicitly does not when last_op==output
		 */
		break;
	}
	setp(0,0) ;
	setg(0,0,0) ;
	last_seek = EOF ;
	restore_errno(orig_errno) ;
	return (ok==EOF ? EOF : 0 ) ;
}

streampos filebuf::seekoff(streamoff p, ios::seek_dir d, int m)
{
	int orig_errno ;
	save_errno(orig_errno) ;

	if ( last_seek == EOF ) {
		last_seek = lseek(xfd,0,seek_cur) ;
		}
	if ( last_seek == EOF ) return EOF ;
	if( SEEK_ARITH_OK  && !unbuffered() ) {
		char* refptr = 0 ;
		streampos sneed ;
		streampos sref, minavail, maxavail ;

		switch ( last_op() ) {
		case input : {
			refptr = gptr() ;
			sref = last_seek-(egptr()-gptr()) ;
			minavail = last_seek-(egptr()-in_start) ; 
			maxavail = last_seek-1 ;
			} break ;
		case output : {
			// only allowable seek during output is
			// to present position. 
			refptr = pptr() ;
			sref = last_seek+pptr()-pbase() ;
			minavail = maxavail = sref ;
			} break  ;
		default: {
			sref = last_seek;
			} break ;
		}
		switch( d ) {
		case seek_beg : sneed = p ; break ;
		case seek_cur : sneed = sref+p ;break ;
		case seek_end : refptr = 0 ; sneed = 0; break ;
				/* Can't do seek_end */
		}
		if ( refptr && sneed >= minavail && sneed <= maxavail ) {
			switch( last_op() ) {
			case input : {
				setg(eback(),refptr+(sneed-sref),egptr());
				} break ;
			case output : {
				// Seeking to current position.  Nothing to
				// do.
				} break ;
			default : { 
				// shouldn't get here.  Try to recover somehow
				sync() ;
				seekoff(p,d,m);
				} break ;
			}
			return sneed ;
		}
#if 0
		if ( refptr && sneed < sref && sneed+blen()/2 > sref 
			&& last_op() == input ) {
			// looks like we are stepping backward through
			// a file.  Performance may be improved by
			// backing up a little extra. 

			streampos toofar = sneed-blen()/2 ;
			if ( toofar < 0 ) toofar = 0 ;
			// sync() ;
			last_seek=lseek(xfd,toofar,seek_beg) ;
			underflow();
			return seekoff(sneed, ios::beg, m);
		}
#endif
	}

	restore_errno(orig_errno) ;
	if ( sync()==EOF )	return EOF ;
	else	{
		last_seek=lseek(xfd,p,d) ;
		return last_seek ;
		}
}

filebuf::filebuf()
	: xfd(-1), opened(0), mode(0), last_seek(EOF), in_start(0)
{

}

filebuf::filebuf(int f)
	: xfd(f), opened(1), mode(getmode_from_fd(f)), last_seek(EOF),
          in_start(0)
{

}

filebuf::filebuf(int f, char* p, int l) 
	: streambuf(p,l), xfd(f), opened(1), mode(getmode_from_fd(f)),
	  last_seek(EOF), in_start(0)
{

}
	
filebuf::~filebuf() {
	close() ;
	}

streambuf* filebuf::setbuf(char* p , int len)
{
	if ( is_open() && base() ) return 0 ;
	// Note the special case of allowing buffering to be turned
	// on even for an already opened filebuf.
	setb(0,0) ;
	return streambuf::setbuf(p,len) ;
}

#ifdef STDIO_ONLY 

/****
	The following functions will simulate the UNIX function
	calls using stdio operations.  They are intended for
	non-unix environments.  Simulation is not perfect and
	there may be problems porting code to such environments.
	In particular issues of binary files are not dealt with
	here
 *****/
#ifndef SEEK_SET 
#	define SEEK_SET 0 
#endif

#ifndef SEEK_CUR
#	define SEEK_CUR 1 
#endif

#ifndef SEEK_END
#	define SEEK_END 2
#endif

typedef FILE* Filep ;

static Filep*	files = 0 ;
static int	nfiles = 0 ;

static Filep	zfp ;

static Filep& file(int fd)
{
	if ( fd < nfiles ) return files[fd] ;
	if ( !files ) {
		files = new Filep [3] ;
		if ( !files ) {
			// Malloc failed. We're in deep trouble  
			return zfp ;
			}
		files[0] = stdin ;
		files[1] = stdout ;
		files[2] = stderr ;
		nfiles = 3 ;
		}
	if ( fd < nfiles ) return files[fd] ;
	
	Filep* newfiles = new Filep [2*nfiles] ;
	if ( !newfiles ) {
		// Oh dear malloc failed.  There isn't really any 
		// reasonable recovery, so I just kind of boot.
		return zfp ;
		}
	int x ;
	for ( x = 0 ; x < nfiles ; ++x) newfiles[x] = files[x] ;
	nfiles = 2*nfiles ;
	for ( ; x < nfiles ; ++x ) newfiles[x] = 0 ;
	delete files ;
	files = newfiles ;
	return files[fd] ;
	}

static int xfile()
{
	if ( !files ) file(0) ;
	for ( int x = 0 ; x < nfiles ; ++x ) {
		if ( !files[x] ) return x ;
		}
	return nfiles ;
	}

static int open(const char* name, int flags, int ) 
{
	/* not all modes can be simulated using stdio,  sorry */
	char* type = "" ;
	switch ( flags & 03 ) {
		case 0 : /* RD_ONLY */ {
			type = "r" ;
			} break ;
		case 1 : /* WR_ONLY */ {
			if ( flags & O_TRUNC )		type = "w" ;
			else 				type = "a" ;
			} break ;
		case 2 : /* RDWR */ {
			if ( flags & O_TRUNC)		type = "w+" ;
			else if ( flags & O_APPEND )	type = "a+" ;
			else 				type = "r+" ;
			} break ;
		}
	
	int x = xfile() ;
	if ( x < 0 ) return -1 ;
	Filep* xfile = &file(x) ;
	*xfile = fopen(name, type) ;
	return ( *xfile ? x : -1 ) ;
	} 

static int creat(const char* name, int) 
{
	return open(name,O_WRONLY|O_CREAT|O_TRUNC,0) ;
	}

static streampos lseek(int fd, long n, ios::seek_dir d ) 
{
	int whence ;
	switch ( d ) {
		case seek_beg : whence = SEEK_SET ; break ;
		case seek_cur : whence = SEEK_CUR ; break ;
		case seek_end : whence = SEEK_END ; break ;
		}
	Filep f = file(fd) ;
	if ( !f ) return EOF ;
	int fail = fseek(f, n, whence ) ;
	if ( fail ) return EOF ;
	return ftell(f) ;
	}


static int read(int fd, char* b, int len)
{
	Filep f = file(fd) ;
	if ( !f ) return 0 ;
	return fread(b,1,len,file(fd))  ;
	}

static int write(int fd, char* b, int len) 
{
	Filep f = file(fd) ;
	if ( !f ) return 0 ;
	int n = fwrite(b,1,len,f)  ;
	fflush(f) ;
	return n ;
	}

static int close(int fd) 
{
	Filep f = file(fd) ;
	if (!f ) return 0 ;
	return fclose(f) ;
	}

#endif /* STDIO_ONLY */
