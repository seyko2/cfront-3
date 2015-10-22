/*ident "@(#)cls4:lib/stream/ios_compat.c	1.3" */
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

* ios_compat.c:
*	The functions in this file allow object files that were created
*	with C++ 2.0 or C++ 2.1 to be linked with an iostreams library
*	compiled with C++ 3.0.  The type "seek_dir" is an enum defined
*	within the class ios, and its name in argument lists is different
*	in C++ 3.0 because of the new "nested classes" feature.
*
*****************************************************************************/
#include <iostream.h>
#include <strstream.h>
#include <fstream.h>
#include <stdiostream.h>

extern "C" {
  streampos seekoff__12strstreambufFl8seek_diri(
	strstreambuf *, long, ios::seek_dir, int); 
  streampos seekoff__8stdiobufFl8seek_diri(
	stdiobuf *, long, ios::seek_dir, int);
  streampos seekoff__9streambufFl8seek_diri(
	streambuf *, long, ios::seek_dir, int);
  streampos seekoff__7filebufFl8seek_diri(
	filebuf *, long, ios::seek_dir, int);
  ostream& seekp__7ostreamFl8seek_dir(
	ostream *, long, ios::seek_dir);
  istream& seekg__7istreamFl8seek_dir(
	istream *, long, ios::seek_dir);
}

streampos
seekoff__12strstreambufFl8seek_diri(
	strstreambuf *s,
	long off,
	ios::seek_dir dir,
	int mode
) {
	return (s->seekoff(off, dir, mode));
}

streampos
seekoff__8stdiobufFl8seek_diri(
	stdiobuf *s,
	long off,
	ios::seek_dir dir,
	int mode
) {
	return (s->seekoff(off, dir, mode));
}

streampos
seekoff__9streambufFl8seek_diri(
	streambuf *s,
	long off,
	ios::seek_dir dir,
	int mode
) {
	return (s->seekoff(off, dir, mode));
}

streampos
seekoff__7filebufFl8seek_diri(
	filebuf *s,
	long off,
	ios::seek_dir dir,
	int mode
) {
	return (s->seekoff(off, dir, mode));
}

ostream&
seekp__7ostreamFl8seek_dir(ostream *os, long off, ios::seek_dir dir) {
	return (os->seekp(off, dir));
}

istream&
seekg__7istreamFl8seek_dir(istream *is, long off, ios::seek_dir dir) {
	return (is->seekg(off, dir));
}
