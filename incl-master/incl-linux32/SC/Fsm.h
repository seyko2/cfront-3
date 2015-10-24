/*ident	"@(#)Fsm:incl/Fsm.h	3.1" */
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

#ifndef FSMH
#define FSMH

class ostream;

class Chararray_ATTLC{
public:
    inline Chararray_ATTLC(
	unsigned int i,
	unsigned int j,
	unsigned int k
    );
    Chararray_ATTLC(
	const Chararray_ATTLC& x
    );
    inline ~Chararray_ATTLC();

    const Chararray_ATTLC& operator=(
	const Chararray_ATTLC& c
    );

    inline unsigned char& operator()(
	unsigned int i,
	unsigned int j,
	unsigned int k
    )const;
    inline unsigned int size(
        unsigned int i
    )const;
private:
    unsigned char* p;
    unsigned int   a;
    unsigned int   b;
    unsigned int   c;
    unsigned int   ab;

    inline unsigned int linearize(
	unsigned int i,
	unsigned int j
    )const;
    inline unsigned int linearize(
	unsigned int i,
	unsigned int j,
	unsigned int k
    )const;
};

static const int MAX_ACTIONS=256;

class Fsm;

typedef int (*hptr)(Fsm& f,unsigned int input);
typedef void (*tptr)(const Fsm& f,int source,int input,int target);

typedef int Fsm_action(Fsm&,unsigned);
typedef void Fsm_tracer(const Fsm&,int,int,int);

class Fsm{
    enum{ACTION =0,NEXT=1};
public:

    Fsm(
	unsigned int	n, 
	unsigned int	init=0, 
	Fsm_action*	action=0
    );
    Fsm( const Fsm& f );
    const Fsm& operator=(const Fsm& f);
    inline ~Fsm();

    inline unsigned int nstates()const;

    inline unsigned int nactions()const;

    inline unsigned int state()const;

    inline unsigned int initial_state()const;

    inline Fsm_action* action(
	unsigned int	state,
	unsigned int	input
    )const;

    inline unsigned int action_number(
	unsigned int	state,
	unsigned int	input
    )const;

    inline unsigned int target(
	unsigned int	state,
	unsigned int	input
    )const;

    void trans(
	unsigned int	start,
	unsigned int	input,
	unsigned int	target,
	Fsm_action*	action=0
    );
    void trans(
	unsigned int	start,
	unsigned int	input1,
	unsigned int	input2,
	unsigned int	target,
	Fsm_action*		action=0
    );
    void trans(
	unsigned int	start,
	char*		re,
	unsigned int	target,
	Fsm_action*		action=0
    );
    int fire( unsigned int input );

    void reset();

    void go( unsigned int s );

    inline void abort();

    inline void trace( Fsm_tracer* t );

private:

    Chararray_ATTLC       c;
    unsigned int    size;
    unsigned int    current;
    unsigned int    initial;
    Fsm_tracer*     tracer;
    Fsm_action*     actions[MAX_ACTIONS];
    Fsm_action**    action_guard;
    Fsm_action**    point;
    Fsm_action*     last_action;
    int             ignore_input;


};

ostream& operator<<(ostream& out,const Fsm& f);

//  Inline function definitions

inline unsigned int Chararray_ATTLC::linearize(
    unsigned int i,
    unsigned int j
)const{
    return(b*i+j);
}
inline unsigned int Chararray_ATTLC::linearize(
    unsigned int i,
    unsigned int j,
    unsigned int k
)const{
    return (ab*k + linearize(i,j));
}
inline Chararray_ATTLC::Chararray_ATTLC(
    unsigned int i,
    unsigned int j,
    unsigned int k
){
    a=i;
    b=j;
    c=k;
    ab=a*b;
    p=new unsigned char[ab*c];
}
inline Chararray_ATTLC::~Chararray_ATTLC(){
    delete[/*  ab*c */] p;
}
inline unsigned char& Chararray_ATTLC::operator()(
    unsigned int i,
    unsigned int j,
    unsigned int k
)const{
    return p[linearize(i,j,k)];
}
inline unsigned int Chararray_ATTLC::size(
    unsigned int i
)const{
    return( i==0? a : (i==1? b : c) );
}
inline Fsm::~Fsm(){
}
inline unsigned int Fsm::state()const{
    return current;
}
inline unsigned int Fsm::nstates()const{
    return size;
}
inline unsigned int Fsm::nactions()const{
    return action_guard - (hptr*)&actions[0];
}
inline unsigned int Fsm::initial_state()const{
    return initial;
}
inline Fsm_action* Fsm::action(unsigned int state,unsigned int input)const{
    return actions[c(state,input,ACTION)];
}
inline unsigned int Fsm::action_number(unsigned int state,unsigned int input)const{
    return c(state,input,ACTION);
}
inline unsigned int Fsm::target(unsigned int state,unsigned int input)const{
    return c(state,input,NEXT);
}
inline void Fsm::abort(){
    go(state());
}
inline void Fsm::trace(Fsm_tracer* t){
    tracer=t;
}

#endif
