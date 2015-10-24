/*ident	"@(#)Stopwatch:Stopwatch.c	3.1" */
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

#include "Stopwatch.h"
#include <sys/types.h>
#include <sys/times.h> 

Stopwatch::Stopwatch(){ 
    old_r_ticks=0;
    old_s_ticks=0;
    old_u_ticks=0;

    r_ticks=0;
    s_ticks=0;
    u_ticks=0;

    move=0;
}
void Stopwatch::reset(){
    struct tms buf;

    old_r_ticks=times(&buf);
    old_u_ticks=buf.tms_utime;
    old_s_ticks=buf.tms_stime;

    r_ticks=0;
    s_ticks=0;
    u_ticks=0;
}
void Stopwatch::start(){
    struct tms buf;

    if(!move){
	move=1;
	old_r_ticks=times(&buf);
	old_u_ticks=buf.tms_utime;
	old_s_ticks=buf.tms_stime;
    }
}
void Stopwatch::stop(){
    struct tms buf;

    if(move){
	r_ticks+=times(&buf)-old_r_ticks;
	u_ticks+=buf.tms_utime-old_u_ticks;
	s_ticks+=buf.tms_stime-old_s_ticks;
	move=0;
    }
}
double Stopwatch::system()const{
    struct tms buf;
    double result;

    if(move){
	(void)times(&buf);
	result=s_ticks+buf.tms_stime-old_s_ticks;
    }else{
	result=s_ticks;
    }
    return result/double(HZ);
}
double Stopwatch::user()const{
    struct tms buf;
    double result;

    if(move){
	(void)times(&buf);
	result=u_ticks+buf.tms_utime-old_u_ticks;
    }else{
	result=u_ticks;
    }
    return result/double(HZ);
}
double Stopwatch::real()const{
    struct tms buf;
    double result;

    if(move){
	result=r_ticks+times(&buf)-old_r_ticks;
    }else{
	result=r_ticks;
    }
    return result/double(HZ);
}
