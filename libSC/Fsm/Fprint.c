/*ident	"@(#)Fsm:Fprint.c	3.1" */
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

#include "Fsm.h"
#include <iostream.h>

ostream& operator<<(ostream& out,const Fsm& f){
    int i,j,empty;

    for(i=0;i<f.nstates();i++){  // print column headings
	out << "\t" << i;
    }
    for(j=0;j<256;j++){               // inputs run down page
	empty=1;

	for(i=0;i<f.nstates();i++){

	    if(f.action_number(i,j)!=0 || f.target(i,j)!=0){
		empty=0;
		break;
	    }
	}
	if(!empty){
	    cout << "\n" << j;

	    for(i=0;i<f.nstates();i++){
		cout << "\t(" 
		     << f.action_number(i,j) 
		     << ","
		     << f.target(i,j)
		     << ")";
	    }
	}
    }
    cout << "\n";
    cout.flush();
    return out;
}
