/*ident	"@(#)G2++:g2++lib/Text.c	3.1" */
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

#include <G2text.h>
#include <ctype.h>
#include <iostream.h>
#include <memory.h>
#ifndef _MSC_VER
#include <values.h>
#endif
#if !defined(_MSC_VER) && !defined(__TCPLUSPLUS__)
#include <libc.h>
#ifdef _LIBC_INCLUDED
#define HP_NATIVE_COMPILER
#endif
#endif
#ifdef __GNUG__
#define IOSTREAMH
#endif

#ifdef IOSTREAMH
#define OSWRITE os.write
#define	GCOUNT	is.gcount()
#else
#define OSWRITE os.put
#define	GCOUNT	Gcount_ATTLC
static int Gcount_ATTLC = 0;
static int gcount_ATTLC(const char* p)
{
	register int	i=0;
	while(*p != '\0' && *p++ != '\n')
		++i;
	return i;
}
#endif

static char buf[4] = "\\";

ostream &operator<<( ostream &os, const G2text &t )
{

//  Translate each nonprintable character to an
//  octal escape sequence.  Pass all other characters 
//  through untranslated, except for backslash;
//  translate backslash to double backslash.

    if(!os)return os;

    const char* tp = (const char*)t;
    const char* guard = tp + t.length();
    const char* p = tp;
    const char* bp = p;

    while( p < guard ){
	register int c = *(unsigned char *)p;

	if( isprint(c) ) {    /* Note: using native isprint */
	    if( c == '\\'){
		OSWRITE(bp,p-bp);
		OSWRITE("\\\\",2);
		bp = p+1;

	    }
	    else{

//  This should be the most probable case.  
//  Do nothing.

	    }
	
	}
	else{

//  Nonprintable character.
//  Write out previous chunk of printables.

	    OSWRITE(bp,p-bp);
	    bp = p+1;

//  Translate this character to a three-octal-digit 
//  escape sequence

	    int n = (unsigned char)*p;
	    char* cp = buf + 4;

	    for(int i = 3;i;i--){
		*--cp = (n&07) + '0';
		n >>= 3;
	    }
	    OSWRITE(buf,4);
	}
	p++;
    }

//  Flush any remaining characters

    if( p > bp ) {
	OSWRITE(bp,p-bp);
    }
    return os;
}

static const int BLOCKSIZE = 2048;

char* G2text::underflow(istream& is,char* p){

//  Read a chunk of BLOCKSIZE characters into 
//  internal String memory and return a pointer to the 
//  first new character.
//
//  p points to the first cell into which the characters
//  should be read.

    char* oldp = (char*)(const char*)(*this);
    int size = p?(p - oldp):0;
    
    reserve(size + BLOCKSIZE);
#ifdef HP_NATIVE_COMPILER
    /* special version for HP's native C++ compiler */
    pad(size - length() + BLOCKSIZE,0);
#endif
    char* tp = (char*)(const char*)(*this) + size;
    is.get(tp,BLOCKSIZE,'\n');

#ifndef IOSTREAMH
    GCOUNT = gcount_ATTLC(tp);
#endif
    size += GCOUNT;
#ifndef HP_NATIVE_COMPILER
    g2_shrink(0);
    g2_pad(size,-1);
#else
    /* special version for HP's native C++ compiler */
    int diffp = tp - (char*)(const char *)(*this);
    shrink(size);
    tp = (char*)(const char*)(*this) + diffp;
#endif

    return tp;
}

istream &operator>>( istream &is, G2text &t ) {

//  Read strings written by operator<< and perform
//  the inverse translation

    if(!is)return is;

    register char* guard = 0;
    register char* pc1 = guard;
    register char* pc2 = guard;
    int c2;

//  gap is the offset relative to pc1 of the first
//  free cell of the target

    register int gap = 0;

//  Guarantee string is not shared and space is not
//  in static storage

    t.uniq();
#ifdef HP_NATIVE_COMPILER
    t.pad(1,-1);
#else
    t.g2_pad(1,-1);
#endif

    while( 1 ) {
	if ( pc1 == guard ) {
	    while (1) {
		pc1 = t.underflow(is,pc1-gap);

		if( GCOUNT==0 ) {  /* if the is.get() function in underflow()
				      has reached the newline, the input
				      operation has finished */
		    return is;
		}
		gap = 0;

#ifdef __GNUG__
	        char* temp = (char*)memchr((void*)pc1,'\\',GCOUNT);
#else
	        char* temp = (char*)memchr(pc1,'\\',GCOUNT);
#endif
			/* look for the first backslash character */


	        if( temp ) {
		    guard = pc1 + GCOUNT;
		    pc1 = temp; /* a short cut - skip to the first backslash */
		    break;
			/* if we did find a backslash, we need to break out
			   of this loop to look for octal escape sequences */
		}else{
		    pc1 += GCOUNT;
			/* if we didn't find a backslash, we can do another
			   is.get() operation */
		}
	    } /* end of while loop */
	}

	/* now we must look for the next backslash */
	if( *pc1 != '\\') {

//  This should be the most probable case:

	    if( gap ){
		*(pc1-gap) = *pc1;
	    }
	    pc1++;

	}else{

//  Decode an escape sequence

	    pc2 = pc1 + 1;

	    if( pc2 == guard ){
		pc2 = t.underflow(is,pc2-gap);

		if( GCOUNT==0){
		    return is;
		}
		guard = pc2 + GCOUNT;
		pc1 = pc2 - 1;
		gap = 0;
	    }
	    c2 = *pc2;

	    if( c2 == '\\'){

//  Slash
		*(pc1 - gap) = '\\';
		gap++;
		pc1 += 2;
	    }else{

//  Decode three octal digits
			
		unsigned short octal_digit = 64*(c2-'0');
		char *pc3 = pc2 + 1;

		if( pc3 == guard){
		    pc3 = t.underflow(is,pc3-gap);

		    if( GCOUNT==0){
			return is;
		    }
		    guard = pc3 + GCOUNT;
		    pc1 = pc3 - 2;
		    gap = 0;
		}
		unsigned int c3 = *pc3;
		octal_digit += (8*(c3-'0'));
		char *pc4 = pc3 + 1;
		
		if( pc4 == guard){
		    pc4 = t.underflow(is,pc4-gap);

		    if( GCOUNT==0 ){
			return is;
		    }
		    guard = pc4 + GCOUNT;
		    pc1 = pc4 - 3;
		    gap = 0;
		}
		unsigned c4 = *pc4;
		octal_digit += (c4-'0');
		*(pc1-gap) = octal_digit;
		gap += 3;
		pc1 += 4;
	    } 
	}
    }
}
