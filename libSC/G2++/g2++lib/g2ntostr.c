/*ident	"@(#)G2++:g2++lib/g2ntostr.c	3.1" */
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

#include <g2io.h>

//
// _g2ltostr - long to string mapping
// 	returns pointer into usr buffer where output string begins
//	(N.B. this will probably not be the beginning of the buffer.)

char* _g2ltostr_ATTLC(
    char buf[], 
    int bufsize, 
    register long n
){
    int	sign;
    register char* cp = buf+bufsize;

    *--cp = '\0';

    if( n < 0 ){
	sign = 1;

	if( n == HIBITL ){
	    unsigned long m = (unsigned long)n;
	    n = m/10;
	    *--cp = (unsigned char)(m - 10*n + '0');
	}else{ 
	    n = -n;
	}
    }else{
	sign = 0;
    }
    do{
	int val = (int)n;
	n /= 10;
	*--cp = (unsigned char)(val - 10*n + '0');
    }while (n);

    if( sign ){
	*--cp = '-';
    }
    return cp;
}
#ifndef _g2itostr

// _g2itostr - int to string mapping
// 	returns pointer into usr buffer where output string begins
//	(N.B. this will probably not be the beginning of the buffer.)

char* _g2itostr(char buf[], int bufsize, register int n){

    int	sign;
    register char* cp = buf+bufsize;

    *--cp = '\0';

    if (n < 0) {
	sign = 1;

	if (n == HIBITI) {
	    unsigned long m = (unsigned int)n;
	    n = m/10;
	    *--cp = m - 10*n + '0';
	}else{ 
	    n = -n;
	}
    }else{
	sign = 0;
    }
    do{
	int val = n;
	n /= 10;
	*--cp = val - 10*n + '0';
    }while (n);

    if(sign){
	*--cp = '-';
    }
    return cp;
}
#endif

char* _g2ctostr_ATTLC(char buf[], int bufsize, register int n){
    register char* cp = buf+bufsize;

    *--cp = '\0';

    do{
	*--cp = (n&07) + '0';
	n = (n>>1) & ~HIBITI;
	n >>= 2;
    }while (n);
    
    return cp;	
}

#ifdef TEST
_main()
{
    char buf[20]; 
    char* cp;
    printf("%s\n", _g2ltostr(buf, sizeof(buf), 0L));
    printf("%s\n", _g2ltostr(buf, sizeof(buf), 1000000L));
    printf("%s\n", _g2ltostr(buf, sizeof(buf), -1000000L));
    printf("%s\n", _g2ltostr(buf, sizeof(buf), 0x80000000));
    printf("%s\n", _g2itostr(buf, sizeof(buf), 0));
    printf("%s\n", _g2itostr(buf, sizeof(buf), 10000));
    printf("%s\n", _g2itostr(buf, sizeof(buf), -10000));
    printf("%s\n", _g2itostr(buf, sizeof(buf), 0x8000));
    printf("%s\n", _g2itostr(buf, sizeof(buf), 0x80000000));
    printf("%s\n", _g2ctostr(buf, sizeof(buf), 0));
    printf("%s\n", _g2ctostr(buf, sizeof(buf), 10000));
    printf("%s\n", _g2ctostr(buf, sizeof(buf), -10000));
    printf("%s\n", _g2ctostr(buf, sizeof(buf), 0x8000));
    printf("%s\n", _g2ctostr(buf, sizeof(buf), 0x80000000));
}
#endif
