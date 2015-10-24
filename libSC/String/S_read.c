/*ident	"@(#)String:S_read.c	3.1" */
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

#define IN_STRING_LIB
#include "String.h"

int
#if defined(SYSV) && !defined(hpux)
read(int fildes, String& buffer, unsigned nbytes)
#else
read(int fildes, String& buffer, int nbytes)
#endif
{
    int	ans;
    switch (nbytes) {
        case 0:
#if defined(__alpha) || defined(hpux)
            if (::read(fildes, (void*)"", 0))
#else
            if (::read(fildes, "", 0))
#endif
        	    return -1;
            else buffer = "";
            return 0;
        case 1: {
            char c;
#if defined(__alpha) || defined(hpux)
            switch (ans = ::read(fildes, (void*)&c, 1)) {
#else
            switch (ans = ::read(fildes, &c, 1)) {
#endif
                case 0:
                    buffer = "";
                    break;
                case 1:
                    buffer = c;
                    break;
            }
            return ans;
        }
    }
    Srep_ATTLC* temp = Srep_ATTLC::new_srep(nbytes);
#if defined(__alpha) || defined(hpux) || defined(_AIX)
    if ((ans = ::read(fildes, (void*)temp->str, nbytes)) == 0) {
#else
    if ((ans = ::read(fildes, temp->str, nbytes)) == 0) {
#endif
	buffer="";
	temp->rcdec();
    }
    else if(ans ==  -1) temp->rcdec();
    else if(ans ==  1) {
	buffer = *temp->str;
	temp->rcdec();
    }
    else {
	temp->len = ans;  // set length of string to what is really read in
	buffer = String(temp);
    }
    return ans;
}
