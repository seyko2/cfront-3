/*ident	"@(#)ipc:ipcbuf.c	3.1" */
/******************************************************************************
*
* C++ Standard Components, Release 3.0.
*
* Copyright (c) 1991, 1992 AT&T and Unix System Laboratories, Inc.
* Copyright (c) 1988, 1989, 1990 AT&T.  All Rights Reserved.
*
* THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T and Unix System
* Laboratories, Inc.  The copyright notice above does not evidence
* any actual or intended publication of such source code.
*
******************************************************************************/

#include "ipclib.h"

#ifndef O_RDONLY
#	include <fcntl.h>
#endif

#ifndef O_TRUNC
#	define O_TRUNC 0
#endif

#ifndef O_CREAT
#	define O_CREAT 0
#endif

#ifndef O_APPEND
#	define O_APPEND 0
#endif

#ifdef __GNUG__
#define zapeof(x) ((x)&0377)
#endif

static String  // includes trailing slash
dirname_plus(const String& arg)
{
	int	base = arg.strrchr('/') + 1;
	if (base == 0) return "./";
	else return arg.chunk(0, base);
}

extern int errno ;

inline void save_errno(int& orig)
{
	orig = ::errno ;
	::errno = 0 ;
}

inline int restore_errno(int& orig)
{
	if ( ::errno == 0 ) ::errno = orig ;
	return EOF ;
}

/*
 *	Open a ipc connection.
 *	Return:		NULL if failure
 *			this if success
 */
ipcbuf* ipcbuf::open (const String& path, const char* /* param */)
{
	int errno_orig ;
	save_errno(errno_orig) ;
	String	name = ipc_fix_name_ATTLC(path);
	char*	p = new char[name.length()+10]; name.dump(p);
	key_t	key = ftok(p, ipcstream::ftok_id ? ipcstream::ftok_id : 'z');
	delete p;
	if (key == key_t(-1)) return 0;

	Monitor* sema1 = new Monitor(key,1);
	sema1->Open();
	sema1->P();

	Monitor* sema2 = new Monitor(key+1,1);
	sema2->Open();
	sema2->V();

	int	fd;

	if ((fd = ::open(String(name+'a'), O_WRONLY)) < 0) {
		sema1->V();
		delete sema1;
		delete sema2;
		return 0;
	}

	if ((p = cuserid(0)) == 0) {
		::close(fd);
		sema1->V();
		delete sema1;
		delete sema2;
		return 0;
	}
	errno = 0;
	{// note no if
		ofstream	ofs(fd);
		ofs << p << " " << getuid() << " " << getgid() << "\n";
		ofs.close();
	}
	::close(fd);
	if ((fd = ::open(String(name+'b'), O_RDONLY)) < 0) {
		sema1->V();
		delete sema1;
		delete sema2;
		return 0;
	}
	{// note no if
		ifstream	ifs(fd);
		String	ans;
		ifs >> ans;
		if (ans == "Accept") {
			String	temp;
			String	dir = dirname_plus(name);
			ifs >> temp;
			String	in_pipe = dir + temp;
			ifs >> temp;
			String	out_pipe = dir + temp;
			out_xfd = ::open(out_pipe, O_WRONLY);
			in_xfd = ::open(in_pipe, O_RDONLY);
			if (in_xfd >= 0 && out_xfd >= 0)
			    opened = 1;
		} else if (ans == "Reject") {
			ifs >> remote_errno;
			remote_reason = sgets(ifs);
		} else {
			remote_errno = -1;
			remote_reason = "Lost synch";
		}
		ifs.close();
	}
	::close(fd);
	sema1->V();
	delete sema1;
	delete sema2;
	return opened ? this : 0;
}

ipcbuf* ipcbuf::open (const char* path, const char* param)
{
	return open(String(path), param);
}

	
/*
 *	Empty an output buffer.
 *	Returns:	EOF on error
 *			0 on success
 */
int ipcbuf::overflow(int c)
{
	int errno_orig ;
	save_errno(errno_orig) ;
	if ( !opened ) return restore_errno(errno_orig) ;
	if ( out_allocate() == EOF ) return restore_errno(errno_orig) ;

	register char* p = aux_base() ;
	register int count = pptr() - p;

	// pptr()==NULL does not imply p < pptr(),  so we need separate
	// test.
	if ( pptr() && count > 0 )  {
		if (count != write(out_xfd,p,count))
			return restore_errno(errno_orig) ;
	}

	setp(pbase(),epptr()) ;

        // cerr << "OverF:abase=" << (int)aux_base() << " pptr=" << (int)pptr() << endl;

	if ( c == EOF ) /* don't do anything */ ;
	else if ( pbase() == epptr() ) {	// unbuffered
		char ch = c;
                // cerr << "OverF:pbase=" << (int)pbase() << " epptr=" << (int)epptr() << endl;
                // cerr << "OverF:xfdout=" << out_xfd << " errno=" << errno_orig << endl;

		if ( write(out_xfd,&ch,1)!=1 ) return restore_errno(errno_orig) ;
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
int ipcbuf::underflow()
{
	int count;

	if ( !opened ) return EOF ;
	if ( in_allocate() == EOF ) return EOF ;
	int orig_errno ;
	save_errno(orig_errno) ;
	if ( in_unbuffered() ) {
#ifdef __hpux
		count = read(in_xfd,(void*)&lahead[0],1) ;
#else
		count = read(in_xfd,&lahead[0],1) ;
#endif
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
#ifdef __hpux
		count = read(in_xfd,(void*)in_start,rdsize) ;
#else
		count = read(in_xfd,in_start,rdsize) ;
#endif
		while ( count<=0 && ::errno==EINTR ) {
			/*
			 * Signal caught and returned before any data
			 * transfered. 	
			 */
			::errno = 0 ;
#ifdef __hpux
			count = read(in_xfd,(void*)in_start,rdsize) ;
#else
			count = read(in_xfd,in_start,rdsize) ;
#endif
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

int ipcbuf::close()
{
	int f = in_xfd ;
	int g = out_xfd ;
	overflow();
	setg(0,0,0) ;
	setp(0,0) ;
	opened = 0 ;
	in_xfd = out_xfd = -1 ;
	int orig_errno ;
	save_errno(orig_errno) ;
	int ok = ::close(f) | ::close(g);
	restore_errno(orig_errno) ;
	return ok ;
}

int ipcbuf::sync() 
{
	return overflow() ;
}

ipcbuf::ipcbuf()
	: in_xfd(-1), out_xfd(-1), opened(0), x_get_unbuf(0), x_put_unbuf(0),
			aux_x_base(0), aux_alloc(0), owner(0)
{

}

ipcbuf::ipcbuf(const char* path, const char* param)
	: in_xfd(-1), out_xfd(-1), opened(0), x_get_unbuf(0), x_put_unbuf(0),
			aux_x_base(0), aux_alloc(0), owner(0)
{
	open(String(path), param);
}

ipcbuf::ipcbuf(const String& path, const char* param)
	: in_xfd(-1), out_xfd(-1), opened(0), x_get_unbuf(0), x_put_unbuf(0),
			aux_x_base(0), aux_alloc(0), owner(0)
{
	open(path, param);
}

ipcbuf::ipcbuf(int in_fd, int out_fd)
	: in_xfd(in_fd), out_xfd(out_fd), opened(1), x_get_unbuf(0),
			x_put_unbuf(0), aux_x_base(0), aux_alloc(0), owner(0)
{

}

ipcbuf::ipcbuf(String in_pipe, String out_pipe)
	: opened(1), x_get_unbuf(0),
			x_put_unbuf(0), aux_x_base(0), aux_alloc(0), owner(1),
			in_pip(in_pipe), out_pip(out_pipe)
{
	if ((in_xfd = ::open(in_pipe, O_RDONLY)) < 0 ||
			(out_xfd = ::open(out_pipe, O_WRONLY)) < 0)
		opened = 0;
}

ipcbuf::~ipcbuf() {
	close() ;
	if (owner) {
		unlink(in_pip);
		unlink(out_pip);
	}
}

streambuf* ipcbuf::setbuf(char*, int)
{
	return 0;
}

streambuf* ipcbuf::set_in_buf(char* p , int len)
{
	if ( base() ) return 0 ;
	if ( len <= 0 || p == 0 ) {
		// make it unbuffered
		setb(0,0,0) ;
		setg(0,0,0) ;
		in_unbuffered(1) ; 
		}
	else {
		setb(p,p+len,0) ;
		setg(p,p,p) ;
		in_unbuffered(0) ;
		} 
	return this;
}

streambuf* ipcbuf::set_out_buf(char* p , int len)
{
	if ( aux_base() ) return 0 ;
	if ( len <= 0 || p == 0 ) {
		// make it unbuffered
		aux_setb(0,0,0) ;
		setp(0,0) ;
		out_unbuffered(1) ; 
		}
	else {
		aux_setb(p,p+len,0) ;
		setp(p,p+len) ;
		out_unbuffered(0) ;
		} 
	return this;
}

/*
	Allocate some space for the buffer.
	Returns:	EOF on error
			0 on success
*/
int ipcbuf::do_out_allocate()
{
	char *buf = new char[STREAMBUFSIZE] ;
	if ( !buf ) return EOF ;
	aux_setb(buf,buf+STREAMBUFSIZE,1) ;
	return 0;
}
