/*ident	"@(#)Fsm:demos/3-3.c	3.1" */
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

#include <Fsm.h>
#include <stream.h>
#include <ctype.h>

int no_change(Fsm&,unsigned input){ 
	return input; 
}

int make_upper(Fsm&,unsigned input){
	return toupper(input);
}

char* translate(char* s,Fsm& t){
	char* p=s;

	while(*p!=0){
		*p=t.fire(*p);
		p++;
	}
	return s;
}

char string[]="$$ablewasI$$ereIsawElba$$";

main(){
	Fsm t(1,0,no_change);  // "t" for "translator"
	t.trans(0,"[a-e]",0,make_upper);
	cout << translate(string,t) << "\n"; 
	return 0;
}
