/*ident	"@(#)cls4:demo/generic/Generic.C	1.1" */
/*******************************************************************************
 
C++ source for the C++ Language System, Release 3.0.  This product
is a new release of the original cfront developed in the computer
science research center of AT&T Bell Laboratories.

Copyright (c) 1991 AT&T and UNIX System Laboratories, Inc.
Copyright (c) 1984, 1989, 1990 AT&T.  All Rights Reserved.

THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE of AT&T and UNIX System
Laboratories, Inc.  The copyright notice above does not evidence
any actual or intended publication of such source code.

*******************************************************************************/

/* Stack.c */

#include <generic.h>
#include <iostream.h>

#define Stack(T) name2(Stack_,T)

#define Stackdeclare(T)		 					\
				 					\
class Stack(T) {							\
public:									\
	Stack(T)(int);							\
	~Stack(T)() { delete list;}					\
	T top() const { return list[curr_size]; }			\
	T pop() { if (curr_size <= -1) error(1); return list[curr_size--]; }				\
	void push(const T&);						\
private:								\
	T *list;							\
	int curr_size;							\
	int max_size;							\
};							

void error(int errno) {
	if (errno==1)
		cerr << "no elements to pop" << "\n";
	else    cerr << "maximum size of stack exceeded" << "\n";
}

#define Stackimplement(T)						\
									\
Stack(T)::Stack(T) (int init_size) {					\
	list = new T[init_size];					\
	curr_size = -1;							\
	max_size=init_size - 1;						\
}									\
void Stack(T)::push(const T& t) {					\
	if (curr_size < max_size) {					\
		list[++curr_size] = t;					\
	}								\
	else error(2);							\
}

Stackdeclare(int)
Stackimplement(int)
	
main() {

	Stack(int) mystack(5);
	mystack.push(1);
	mystack.push(2);
	mystack.push(3);

	cout << mystack.pop();
	cout << mystack.pop();
	cout << mystack.pop();
	cout << "\n";
	return 0;
}
