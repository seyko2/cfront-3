/*ident	"@(#)G2++:g2++lib/demos/demo5.c	3.1" */
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

#include <g2++.h>
#include <stream.h>

void walk( const G2NODE* p,int level ){
    cout << level << ": " << p->name;

    if( !p->val.is_empty() ){
	cout << "\t" << p->val << "\n";
    }else{
	cout << "\n";
    }
    for( G2NODE* cp = p->child; cp; cp = cp->next ){
	walk(cp,level+1);
    }
}
main(){
    G2BUF buf;

    while( cin >> buf ){
	walk(buf.root,0);
    }
    return 0;
}
