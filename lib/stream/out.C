/* @(#) out.c 1.2 1/27/86 17:48:03 */
/*ident	"@(#)cfront:lib/stream/out.c	1.2"*/
/*
	C++ stream i/o source

	out.c
*/
sprintf(char*,char* ...);
strlen(char*);
#include "stream.h"
#include <common.h>


#define MAXOSTREAMS 20

char cout_buf[BUFSIZE];
filebuf cout_file(1,cout_buf,BUFSIZE);	// UNIX output stream 1
ostream cout(&cout_file);

char cerr_buf[1];
filebuf cerr_file(2,cerr_buf,0);	// UNIX output stream 2
					// 0-length to ensure unbuffered output
ostream cerr(&cerr_file);

const	cb_size = 512;
const	fld_size = 128;

/* a circular formating buffer */
static char	formbuf[cb_size];
static char*	bfree=formbuf;
static char*	max = &formbuf[cb_size-1];

char* chr(register i, register int w)	/* note: chr(0) is "" */
{
	register char* buf = bfree;

	if (w<=0 || fld_size<w) w = 1;
	w++;				/* space for trailing 0 */
	if (max < buf+w) buf = formbuf;
	bfree = buf+w;
	char * res = buf;

	w -= 2;				/* pad */
	while (w--) *buf++ = ' ';
	if (i<0 || 127<i) i = ' ';
	*buf++ = i;
	*buf = 0;
	return res;
}

char* str(char* s, register int w)
{
	register char* buf = bfree;
	int ll = strlen(s);
	if (w<=0 || fld_size<w) w = ll;
	if (w < ll) ll = w;
	w++;				/* space for traling 0 */
	if (max < buf+w) buf = formbuf;
	bfree = buf+w;
	char* res = buf;

	w -= (ll+1);			/* pad */
	while (w--) *buf++ = ' ';
	while (*s) *buf++ = *s++;
	*buf = 0;
	return res;
}

char* form(char* format ...)
{
	register* ap = (int*)&format;
	register char* buf = bfree;
	if (max < buf+fld_size) buf = formbuf;

	register ll = sprintf(buf,format,ap[1],ap[2],ap[3],ap[4],ap[5],ap[6],ap[7],ap[8]);
	if (0<ll && ll<1024)				/* length */
		;
	else if (buf<(char*)ll && (char*)ll<buf+1024)	/* pointer to trailing 0 */
		ll = (char*)ll - buf;
	else
		ll = strlen(buf);
	if (fld_size < ll) exit(10);
	bfree += (ll+1);
	return buf;
}

const char a10 = 'a'-10;

char* hex(long i, register w)
{
	int m = sizeof(long)*2;		/* maximum hex digits for a long */
	if (w<0 || fld_size<w) w = 0;
	int sz = (w?w:m)+1;
	register char* buf = bfree;
	if (max < buf+sz) buf = formbuf;
	register char* p = buf+sz;
	bfree = p+1;
	*p-- = 0;			/* trailing 0 */

	if (w) {
		do {
			register h = i&0xf;
			*p-- = (h < 10) ? h+'0' : h+a10;
		} while (w-- && (i>>=4));
		while (w--) *p-- = ' ';
	}
	else {
		do {
			register h = i&0xf;
			*p-- = (h < 10) ? h+'0' : h+a10;
		} while (i>>=4);
	}
	return p+1;
}

char* oct(long i, int w)
{
	int m = sizeof(long)*3;		/* maximum oct digits for a long */
	if (w<0 || fld_size<w) w = 0;
	int sz = (w?w:m)+1;
	register char* buf = bfree;
	if (max < buf+sz) buf = formbuf;
	register char* p = buf+sz;
	bfree = p+1;
	*p-- = 0;			/* trailing 0 */

	if (w) {
		do {
			register h = i&07;
			*p-- = h+'0';
		} while (w-- && (i>>=3));
		while (w--) *p-- = ' ';
	}
	else {
		do {
			register h = i&07;
			*p-- = h+'0';
		} while (i>>=3);
	}

	return p+1;
}

char* dec(long i, int w)
{
	int m = sizeof(long)*3;		/* maximum dec digits for a long */
	if (w<0 || fld_size<w) w = 0;
	int sz = (w?w:m)+1;
	register char* buf = bfree;
	if (max < buf+sz) buf = formbuf;
	register char* p = buf+sz;
	bfree = p+1;
	*p-- = 0;			/* trailing 0 */

	if (w) {
		do {
			register h = i%10;
			*p-- = h + '0';
		} while (w-- && (i/=10));
		while (w--) *p-- = ' ';
	}
	else {
		do {
			register h = i%10;
			*p-- = h + '0';
		} while (i/=10);
	}

	return p+1;
}


ostream& ostream.operator<<(char* s)
{
	register streambuf* nbp = bp;
	if (state) return *this;
	if (*s == 0) return *this;
	do
		if (nbp->sputc(*s++) == EOF) {
			state |= _eof|_fail;
			break;
		}
	while (*s);
//	if (*(s-1)=='\n') flush(); /* fudge due to lack of  destructors for static*/
	return *this;
}

ostream& ostream.operator<<(long i)
{
	register streambuf* nbp = bp;
	register long j;
	char buf[32];
	register char *p = buf;

	if (state) return *this;

	if (i < 0) {
		nbp->sputc('-');
		j = -i;
	} else
		j = i;
	do {
		*p++ = '0' + j%10;
		j = j/10;
	} while (j > 0);
	do {
		if (nbp->sputc(*--p) == EOF) {
			state |= _fail | _eof;
			break;
		}
	} while (p != buf);
	return *this;
}

ostream& ostream.put(char c)
{
	if (state) return *this;

	if (bp->sputc(c) == EOF) state |= _eof|_fail;

	return *this;
}

ostream& ostream.operator<<(double d)
{
	register streambuf* nbp = bp;
	char buf[32];
	register char *p = buf;

	if (state) return *this;

	sprintf(buf,"%g",d);
	while (*p != '\0')
		if (nbp->sputc(*p++) == EOF) {
			state |= _eof|_fail;
			break;
		}
	return *this;
}

ostream& ostream.operator<<(streambuf& b)
{
	register streambuf* nbp = bp;
	register int c;

	if (state) return *this;

	c = b.sgetc();
	while (c != EOF) {
		if (nbp->sputc(c) == EOF) {
			state |= _eof|_fail;
			break;
		}
		c = b.snextc();
	}
		
	return *this;
}


