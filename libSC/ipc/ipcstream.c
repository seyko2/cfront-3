/*ident	"@(#)ipc:ipcstream.c	3.1" */
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

#ifdef __alpha
extern "C" int mknod(const char*, mode_t, int);
#endif
#ifndef FILNMLEN
#define FILNMLEN	14	/* Number of characters in a file name */
#endif

char ipcstream::ftok_id;
char ipc_attachment::ftok_id;

static String	next_string(const String& s);

/* To create an IPC_attachment, open the ipc main file */
ipc_attachment::ipc_attachment(const String& path, const char* /* param */)
: user_p(0), att_name(ipc_fix_name_ATTLC(path)), attached(0)
{
	initialize_attachment();
}

ipc_attachment::ipc_attachment(const char* path, const char* /* param */)
: user_p(0), att_name(ipc_fix_name_ATTLC(path)), attached(0)
{
	initialize_attachment();
}

void
ipc_attachment::initialize_attachment()
{
	xfd = ::open(att_name, O_WRONLY|O_CREAT|O_EXCL, 0666);
	if (xfd < 0) {
		okay = 0;
		return;
	}

	char*	p = new char[att_name.length()+1]; att_name.dump(p);
	key_t	key = ftok(p, ftok_id ? ftok_id : 'z');
	delete p;
	if (key == key_t(-1)) {
		okay = 0;
		::close(xfd); // wct
		return;
	}

	ofstream*	key_p = new ofstream(xfd);
	if (!key_p->good()) {
		okay = 0;
		::close(xfd); // wct
		return;
	}
	*key_p << "Key: " << key << "\n";
	delete key_p;
	::close(xfd);

	if (mknod((const char *)(String(att_name+'a')), 0010666, 0) != 0) {
		okay = 0;
		return;
	}

	if (mknod((const char *)(String(att_name+'b')), 0010666, 0) != 0) {
		okay = 0;
		return;
	}

	last_connection = att_name+'b';

	sema1 = new Monitor(key, 1);
	sema1->Open();
	sema2 = new Monitor(key+1, 1);
	sema2->Open();

	okay = 1;
}

ipc_attachment::~ipc_attachment()
{
	if (xfd < 0)
	    return;

	// the ordering here keeps multiple clients at bay
	unlink(att_name);	// new clients won't find the semaphore
	unlink(String(att_name+'b'));	// waiting clients won't open the reply channel
	int fd = ::open(String(att_name+'a'), O_RDONLY|O_NDELAY);	// un-block at-bat client
	unlink(String(att_name+'a'));	// on-deck client won't find request channel
	if (fd >= 0) ::close(fd);

	if (sema1) {
	    sema1->Close();
	    delete sema1;
	}

	if (sema2) {
	    sema2->Close();
	    delete sema2;
	}
}

void
ipc_attachment::listen()
{
	if (attached) return;

	sema2->P();

	attached = 1;

	// if (user_p) return;

	int	ifd;
	if ((ifd = ::open(String(att_name+'a'), O_RDONLY)) < 0) {
		okay = 0;
	 	return;
	}

	ifstream	ifs(ifd);
	String	temp;
	ifs >> temp >> uid_p >> gid_p;
	if (ifs.good()) {
		user_p = new char[temp.length()+1]; temp.dump(user_p);
	}
	ifs.close();
	::close(ifd);
}

void
ipc_attachment::reject(int err_no, const char* reason)
{
	if (user_p == 0) return;
	int	ofd;
	if ((ofd = ::open(String(att_name+'b'), O_WRONLY)) < 0) {
		okay = 0;
		return;
	}
	ofstream	ofs(ofd);
	ofs << "Reject " << err_no << " " << reason << "\n";
	ofs.close();
	::close(ofd);
	delete user_p;
	user_p = 0;
}

extern int errno;	
ipcstream
ipc_attachment::accept()
{
#ifdef __edg_att_40
	return (ipcstream&)ipcstream(*this);
#else
	return ipcstream(*this);
#endif
}

ipcbuf*
ipc_attachment::create_ipcbuf()
{
	ipcbuf *retval;
	if (!attached) 
	    listen();

	attached = 0;

	String	in_pipe = next_string(last_connection);

	while (mknod((const char *)in_pipe, 0010666, 0) != 0) {
		if (errno != EEXIST) return 0;
		in_pipe = next_string(in_pipe);
	}

	String	out_pipe = next_string(in_pipe);
	while (mknod((const char *)out_pipe, 0010666, 0) != 0) {
		if (errno != EEXIST) return 0;
		out_pipe = next_string(out_pipe);
	}

	last_connection = out_pipe;

	int	ofd;
	if ((ofd = ::open(String(att_name+'b'), O_WRONLY)) < 0) {
	 	retval=0;
	}
	else 
	{

	    ofstream	ofs(ofd);
	    if (ofs.good() == 0)
  		retval = NULL;

	    else
	    {
		ofs << "Accept " << ipc_basename_ATTLC(out_pipe) << " " << ipc_basename_ATTLC(in_pipe) << "\n";
		ofs.close();
		::close(ofd);
		retval = new ipcbuf(in_pipe, out_pipe);
	    }
	}

	return retval;
}

ipcstream::ipcstream(ipc_attachment& attach)
{
	init(attach.create_ipcbuf());
	verify(checkbuf());
}

ipcstream::ipcstream(const char* path, const char* param)
{
	init(new ipcbuf(path, param));
	verify(checkbuf());
}

ipcstream::ipcstream(const String& path, const char* param)
{
	init(new ipcbuf(path, param));
	verify(checkbuf());
}

void ipcstream::verify(int ok)
{
	if ( ok ) clear(ios::goodbit) ;
	else 	  setstate(ios::failbit|ios::badbit) ;
}

void ipcstream::open(const char* name, const char* param)
{
	verify(rdbuf()->open(name, param) != 0 ) ;
}

void ipcstream::open(const String& name, const char* param)
{
	verify(rdbuf()->open(name, param) != 0 ) ;
}


void ipcstream::setbuf(char* p, int len) 
{
	verify(rdbuf()->setbuf(p,len) != 0 ) ;
}

void ipcstream::close() 
{
	verify(rdbuf()->close() != EOF ) ;
}

ipcstream::~ipcstream()
{
#ifdef __GNUG__
	delete rdbuf();
#else
	delete bp;
#endif
}

/* count in base 62 */
static char
next_char(register char c)
{
	if ((c >= 'a' && c < 'z') ||
		(c >= 'A' && c < 'Z') ||
		(c >= '0' && c < '9')) return c+1;
	if (c == 'z') return 'A';
	if (c == 'Z') return '0';
	return 0;
}

static String
next_string(const String& s)
{
	String ans = s;
	char	c = next_char(ans[ans.length()-1]);
	if (c) {
		ans[ans.length()-1] = c;
		return ans;
	}
	ans[ans.length()-1] = 'a';
	int	ans_base = ans.strrchr('/') + 1;
	if (ans.length() - ans_base < FILNMLEN)
		return ans + 'a';
	int i = ans.length()-1;
	while ((c = next_char(ans[(unsigned)--i])) == 0) {
		ans[(unsigned)i] = 'a';
		if (i == ans_base)
			return ans;
	}
	ans[(unsigned)i] = c;
	return ans;
}

