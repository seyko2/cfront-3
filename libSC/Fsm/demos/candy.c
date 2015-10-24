/*ident	"@(#)Fsm:demos/candy.c	3.1" */
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

#include <Strstream.h>
#include <String.h>
#include <stream.h>
#include <Fsm.h>

/*
**  Ideally, this program should be interactive, getting
**  inputs from the terminal keyboard and sending outputs
**  to the screen.  To put it into the form required for
**  regression testing (pairs of equal lines), we simulate
**  terminal keyboard input by the iStrstream INPUT.  The
**  expected output is in the iStrstream OUTPUT and the
**  expected trace output is in iStrstream LOG.
**
*/

String example_input = "0 0 0 0 3 0 1 2 1 0 3";
iStrstream INPUT(example_input);

String example_output = "click click click click CANDY click clickclick GUM clickclick click RETURN15CENTS";
iStrstream OUTPUT(example_output);

String example_log = "0\t0\t1\n1\t0\t2\n2\t0\t3\n3\t0\t4\n4\t3\t0\n0\t0\t1\n1\t1\t3\n3\t2\t0\n0\t1\t2\n2\t0\t3\n3\t3\t0\n";

iStrstream LOG(example_log);

int click(Fsm&,unsigned int){
    cout << "click\n" << flush;
    return 0;
}
int clickclick(Fsm&,unsigned int){
    cout << "clickclick\n" << flush;
    return 0;
}
int dispense_gum(Fsm&,unsigned int){
    cout << "GUM\n" << flush;
    return 0;
}
int dispense_candy(Fsm&,unsigned int){
    cout << "CANDY\n" << flush;
    return 0;
}
int error(Fsm& f,unsigned int){
    cout 
	<< "RETURN" 
	<< f.state()*5 
	<< "CENTS\n"
	<< flush
    ;
    return 0;
}

enum States{zero,five,ten,fifteen,twenty};
enum Inputs{nickle,dime,select_gum,select_candy};

oStrstream os;
void tracer(const Fsm&,int source,int input,int current){
    os << source << "\t" << input << "\t" << current << "\n";
}

main(){
    Fsm v(5,zero,error);

    v.trans(zero,(unsigned)nickle,five,click);
    v.trans(zero,dime,ten,clickclick);
    v.trans(five,(unsigned)nickle,ten,click);
    v.trans(five,dime,fifteen,clickclick);
    v.trans(ten,(unsigned)nickle,fifteen,click);
    v.trans(ten,dime,twenty,clickclick);
    v.trans(fifteen,(unsigned)nickle,twenty,click);
    v.trans(fifteen,select_gum,zero,dispense_gum);
    v.trans(twenty,select_candy,zero,dispense_candy);

    v.trace(tracer);

//  Now operate the vending machine

    while(1){
	int i;
	String answer;
	if(!(INPUT >> i))break;
	v.fire(i);
	OUTPUT >> answer;
	cout << answer << "\n";
    }

//  Check the log

    char buf[100];
    String s(os.str());
    iStrstream is(s);

    while(1){
        if(!is.getline(buf,100))break;
	cout << buf << "\n";
	if(!LOG.getline(buf,100))break;
	cout << buf << "\n";
    }
    return 0;
}
