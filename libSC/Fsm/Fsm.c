/*ident	"@(#)Fsm:Fsm.c	3.1" */
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
#include <ctype.h>

static void one_char_re(char* re,int hit[256]);

Fsm::Fsm(
    unsigned int n,
    unsigned int init,
    Fsm_action* action
):
    c(n,256,2)
{
    for(int i=0;i<n;i++){
	for(int j=0;j<256;j++){
	    c(i,j,ACTION)=0;
	    c(i,j,NEXT)=0;
	}
    }
    size=n;
    current=init;
    initial=init;
    tracer=0;
    actions[0]=action;
    action_guard=actions+1;
    point=actions;
    last_action=action;
    ignore_input=0;
}
Fsm::Fsm(
    const Fsm& f
):
    c(f.c)
{
    size=f.size;
    current=f.current;
    initial=f.initial;
    tracer=f.tracer;

    Fsm_action** p=actions;
    Fsm_action** fp=((Fsm*)&f)->actions;
    action_guard=actions+(f.action_guard-(hptr*)f.actions);

    while(fp<f.action_guard){
	*p++ = *fp++;
    }
    point=actions+(f.point-(hptr*)f.actions);
    last_action=f.last_action;
    ignore_input=f.ignore_input;
}
const Fsm& Fsm::operator=(const Fsm& f){
    c = f.c;
    size=f.size;
    current=f.current;
    initial=f.initial;
    tracer=f.tracer;

    Fsm_action** p=actions;
    Fsm_action** fp=((Fsm*)&f)->actions;
    action_guard=actions+(f.action_guard-(hptr*)f.actions);

    while(fp<f.action_guard){
	*p++ = *fp++;
    }
    point=actions+(f.point-(hptr*)f.actions);
    last_action=f.last_action;
    ignore_input=f.ignore_input;
    return *this;
}
void Fsm::trans(
    unsigned int   start,
    unsigned int   input,
    unsigned int   next,
    Fsm_action*    action
){
    if( action!=last_action ){
	int found=0;
	point=actions;

	while( point<action_guard && !found ){

	    if( *point==action ){
		found=1;
		break;
	    }
	    point++;
	}
	if( !found ){
	    action_guard++;
	    *point=action;
	}
	last_action=action;
    }
    c(start,input,ACTION)=point-actions;
    c(start,input,NEXT)=next;
}
void Fsm::trans(
    unsigned int   start,
    unsigned int   input1,
    unsigned int   input2,
    unsigned int   next,
    Fsm_action*    action
){
    for( int i=input1;i<=input2;i++ ){
	trans(start,i,next,action);
    }
}
void Fsm::trans(
    unsigned int   start,
    char*          re,
    unsigned int   next,
    Fsm_action*    action
){
    int i;
    int hit[256];

//  hit[256] == 1 means a transition is defined

    for(i=0;i<256;i++){
	hit[i]=0;
    }

//  one_char_re sets hit[i] to 1 if a transition
//  should be defined for character i.

    one_char_re(re,hit);

//  Define transitions for characters for which
//  hit[i] is set.

    for(i=0;i<256;i++){
	if( hit[i] && isprint(i) ){
	    trans(start,i,next,action);
	}
    }
}
int Fsm::fire(unsigned int input){
    int result=0;
    int source=current;

//  FIRST invoke user-defined action routine in the old state 

    ignore_input=0; 
    int index=c(current,input,ACTION); 

    if( actions[index]!=0 ){ 
	result=actions[index](*this,input); 
    }

//  THEN go to the new state unless ignore_input has been set
//  (indicating an intermediate state transition).

    if( !ignore_input ){
	current=c(source,input,NEXT);

	if(tracer){
	    if(!ignore_input){
		tracer(*this,source,input,current);
	    }
	}
	ignore_input=1;
    }
    return result;
}
void Fsm::go(unsigned int s){
    int source=current;
    current=s;

    if(tracer){
	tracer(*this,source,-1,current);
    }
    ignore_input=1;
}
void Fsm::reset(){
    int source=current;
    current=initial;

    if(tracer){
	tracer(*this,source,-1,current);
    }
    ignore_input=1;
}

static void one_char_re(char* re,int hit[256]){

    for(int i=0;i<256;i++){
	hit[i]=0;
    }
    if(*re=='\\'){
	if(*++re)hit[*re]=1;
    }else if(*re=='.'){
	for(int i=0;i<256;i++)hit[i]=1;
    }else if(*re=='['){
	int negate;
	if(*++re){
	    negate=*re=='^';
	    for(int i=0;i<256;i++)hit[i]=negate;
	    if( negate ){
		if(*++re)hit[*re]=0;
	    }else{
		hit[*re]=1;
	    }
	    int start=*re;
	    while(*++re){
		if(*re==']'){
		    break;
		}else if(*re=='-'){
		    if(*++re){
			if(*re==']'){
			    break;
			}else{
			    for(int i=start+1;i<=*re;i++){
				hit[i] = !( hit[i] || negate );
			    }
			}
		    }
		}else{
		    start=*re;
		    hit[*re] = !( hit[*re] || negate );
		}
	    }
	}
    }else{
	hit[*re]=1;
    }
}
