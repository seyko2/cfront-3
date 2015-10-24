/*ident	"@(#)G2++:incl/G2text.h	3.1" */
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

#ifndef G2TEXTH
#define G2TEXTH

#include <String.h>

class istream;
class ostream;

class G2text : public String { 
private:
    char* underflow(istream& is,char* p);
public:

    G2text():String(){ }
    G2text(const char* p):String(p){ }
    G2text(const char* p,unsigned n):String(p,n){ }
    G2text(char c):String(c){ }
    G2text(const Substring& s):String(s){ }
    G2text(Tmpstring& s):String(s){ }
    G2text(Stringsize n):String(n){ }
    G2text(const G2text& t):String(t){ } 

//  Insertion and extraction

    friend ostream &operator<<(ostream &os,const G2text &t);
    friend istream &operator>>(istream &is,G2text &t);
};

#endif
