#!/bin/sh

source ./00-COMMON.sh

[ ! -d $TMPDIR ] && {
    echo "Error: not exists TMPDIR=$TMPDIR"
    exit 1
}

[ -d $td  ] && rm -rf $td
[ -d $td2 ] && rm -rf $td2

mkdir -p $td
mkdir -p $td/xxx
mkdir -p $td/xxx/xxx
mkdir -p $td/yyy
mkdir -p $td2

chmod g-s $td
chmod g-s $td2
cd $td

cat <<EOF >$t1
template <class T> struct A {};
A<int> a;
struct B {};
EOF

cat <<EOF >$t3
struct A {};
enum E {};
int x;
EOF

cat <<EOF >$t4
template <class T> struct A {
	T x;
};
struct B;
A<B> a;
EOF

cat <<EOF >$t5
struct A {};
union B {};
class C {};
enum D {};
typedef int E;
typedef C F;
struct {} a;
union {} b;
typedef struct {} I;
void J() {}
struct K {void f();};
void K::f() {}
template <class T> void operator!(T);
template <class T> void operator&&(T, int) {}
template <class T> struct L {
	void f();
	static int x;
};
template <class T> void L<T>::f() {}
template <class T> int L<T>::x = 37;
struct M {} c;
template <class T> class N;
template <class T> class N;
template <class T> class N {
	int x;
};
struct O;
template <class T> void P(T);
template <class T> struct G {};
struct G<int> {};
struct Q {
	struct R {};
	typedef int T;
	enum U {};
};
void f()
{
struct A2 {};
union B2 {};
class C2 {};
enum D2 {};
typedef int E2;
typedef C F2;
struct {} a2;
union {} b2;
typedef struct {} I2;
struct K2 {void f() {}};
struct M2 {} c;
struct O2;
struct Q2 {
	struct R2 {};
	typedef int T2;
	enum U2 {};
};
}
EOF

cat <<EOF >$t5x
A "t5.C"
B "t5.C"
C "t5.C"
D "t5.C"
G "t5.C"
G__pt__2_i "t5.C"
I "t5.C"
K "t5.C"
L "t5.C"
M "t5.C"
N "t5.C"
P "t5.C"
Q "t5.C"
__nt "t5.C"
EOF

cat <<EOF >$t6
template <class T> void f(T);
template <class T> void f(T);
template <class T> void f(T);
template <class T> void operator!(T);
template <class T> void operator!(T);
template <class T> void operator!(T);
template <class T> void f(T);
template <class T> void f(T);
template <class T> void f(T);
template <class T> void operator!(T);
template <class T> void operator!(T);
template <class T> void operator!(T);
EOF

cat <<EOF >$t7
main() {}
EOF

cat <<EOF >$t8
#include "t8.h"
union Lurch {};
main()
{
	A<int> a;
	A<Lurch> b;
	a.f();
	b.f();
	a.g();
	b.g();
	f(37, 12.34);
	f("xxx", 'x');
	return 0;
}
EOF

cat <<EOF >$t8h
#ifndef T8_INCL
#define T8_INCL
template <class T> struct A {
	void f();
	void g();
};
template <class T, class U> void f(T, U);
#endif
EOF

cat <<EOF >$t8c
template <class T> void A<T>::f()
{
	static int x = 0;
	if (x++ < 5)
		g();
}
template <class T> void A<T>::g()
{
	static int x = 0;
	if (x++ < 5)
		f();
}
template <class T, class U> void f(T t, U u)
{
	static int x = 0;
	if (x++ < 5)
		f(t, u);
}
EOF

cat <<EOF >$t9h
template <class T> struct A {
	void f();
	void g();
};
EOF

cat <<EOF >$t9c
template <class T> void A<T>::f() {}
EOF
cat <<EOF >$t9
#include "t9.h"
main()
{
	A<int> a;
	a.f();
	a.g();
}
EOF

cat <<EOF >$t10h
template <class T, class U, class V, class W, class X> struct A {
	void f();
};
template <class T, class U, class V, class W, class X> void ff(T, U, V, W, X);
struct AA {};
EOF

cat <<EOF >$t10c
template <class T, class U, class V, class W, class X> void A<T,U,V,W,X>::f() {}
template <class T, class U, class V, class W, class X> void ff(T,U,V,W,X) {}
EOF

cat <<EOF >$t10h2
enum BB {bb = 47};
EOF

cat <<EOF >$t10h3
union C {};
EOF

cat <<EOF >$t10
#include "t10.h"
#include "t10_2.h"
#include "t10_3.h"
A<AA,BB*,AA&,BB**,C> a;
main()
{
	AA a;
	BB b;
	AA* ap;
	BB& br = b;
	C c;
	ff(a, b, ap, br, c);
}
EOF

cat <<EOF >$t11h
template <class T> struct A {
	void f();
};
EOF

cat <<EOF >$t11c
template <class T> void A<T>::f() {}
EOF

cat <<EOF >$t11
#include "t11.h"
struct B {};
main()
{
	extern void g();
	A<B> a;
	a.f();
	g();
}
EOF

cat <<EOF >$t11_2
#include "t11.h"
struct B {};
void g()
{
	A<B> a;
}
EOF

cat <<EOF >$t12
#include "t12.h"
main()
{
	AA<int> a;
	AA<double> b;
	a.f();
	b.f();
	ff(37, 12.34L);
	ff("xxx", (short)'x');
}
EOF

cat <<EOF >$t12h
template <class T> struct AA {
	void f();
	int zzz[N];
};
template <class T, class U> void ff(T, U);
#ifdef XXX
#include "t12_3.h"
#endif
#ifdef ZZZ
const int NN = 83;
#endif
EOF

cat <<EOF >$t12_3
const int NN = 97;
EOF
cat <<EOF >$t12c
template <class T> void AA<T>::f() {int x[NN];}
template <class T, class U> void ff(T, U) {}
EOF

cat <<EOF >$t13
#include "t13.h"
main()
{
	A<int> a;
	A<double> b;
	a.f();
	b.f();
	f(37, 12.34);
	f("xxx", 'x');
}
EOF

cat <<EOF >$t13h
template <class T> struct A {
	void f();
};
template <class T, class U> void f(T, U);
EOF
cat <<EOF >$t13c
template <class T> void A<T>::f() {}
template <class T, class U> void f(T, U) {}
EOF

cat <<EOF >$t14h1
template <class T> void f(char*, T);
EOF

cat <<EOF >$t14h2
template <class T> void f(T);
EOF

cat <<EOF >$t14c1
template <class T> void f(char*, T) {}
EOF

cat <<EOF >$t14c2
template <class T> void f(T) {}
EOF

cat <<EOF >$t14
#include "t14_1.h"
#include "t14_2.h"
main()
{
	f("foo", 37);
}
EOF

cat <<EOF >$t15
#include "t15.h"
main() {}
EOF

cat <<EOF >$t16
#include "t16.h"
main() {}
EOF

cat <<EOF >$t17
#include "t17x.h"
main()
{
	A<int> a;
	a.f();
}
EOF

cat <<EOF >$t17h
template <class T> struct A {
	void f();
};
EOF

cat <<EOF >$t17c
template <class T> void A<T>::f() {}
EOF

cat <<EOF >$t18
#include "t18_1.h"
main()
{
	A<long double> a;
	a.f();
}
EOF

cat <<EOF >$t18h
template <class T> struct A {
	void f();
};
EOF

cat <<EOF >$t18c
#include "t18_2.h"
template <class T> void A<T>::f() {
	B<T> b;
	b.f();
}
EOF

cat <<EOF >$t18h2
template <class T> struct B {
	void f();
};
EOF

cat <<EOF >$t18c2
template <class T> void B<T>::f() {}
EOF

cat <<EOF >$t19
main()
{
	extern void f();
	f();
}
EOF

cat <<EOF >$t19a1
#include "t19.h"
void g()
{
	A<char**> a;
	a.f();
}
EOF

cat <<EOF >$t19a2
void f()
{
	extern void g();
	g();
};
EOF

cat <<EOF >$t19h
template <class T> struct A {
	void f();
};
EOF

cat <<EOF >$t19c
template <class T> void A<T>::f()
{
}
EOF

cat <<EOF >$t20c
static int x111 = 35;
static void f222() {}
EOF

cat <<EOF >$t21
#include "t21_1.h"
char c;
A<&c> a1;
void f(int) {}
B<&f> a2;
C<Z::f> a3;
ZZ z;
E<&z, D<-27> > a4;
main()
{
	a1.f();
	a2.f();
	a3.f();
	a4.f();
}
void Z::f() {}
EOF

cat <<EOF >$t21h
template <char* p> struct A {void f();};
typedef void (*fp)(int);
template <fp p> struct B {void f();};
struct Z {
	void f();
};
typedef void (Z::*pmf)();
template <pmf p> struct C {void f();};
template <int n> struct D {void f();};
struct ZZ {};
template <ZZ* p, class T> struct E {void f();};
EOF

cat <<EOF >$t21c
template <char* p> void A<p>::f() {}
template <fp p> void B<p>::f() {}
template <pmf p> void C<p>::f() {}
template <int n> void D<n>::f() {}
template <ZZ* p, class T> void E<p,T>::f() {}
EOF

cat <<EOF >$t22
#include "t22_1.h"
struct B;
A<B*,B&> x;
main()
{
	x.f();
}
EOF

cat <<EOF >$t22h
template <class T, class U> struct A {
	void f();
};
EOF

cat <<EOF >$t22c
template <class T, class U> void A<T,U>::f() {}
EOF

cat <<EOF >$t23
#include <stream.h>
#include "19t23_1.h"
A<int> a;
main()
{
	cout << a << "\n";
	return 0;
}
EOF

cat <<EOF >$t23h
template <class T> struct A {
	operator int();
};
EOF

cat <<EOF >$t23c
#include <stream.h>
template <class T> A<T>::operator int()
{
	cout << "foo" << "\n";
	return 97;
}
EOF

cat <<EOF >$t24
template <class T> struct A {
	void f();
};
template <class T> void A<T>::f()
{
}
struct B {};
main()
{
	A<B> a;
	a.f();
}
EOF

cat <<EOF >$t25s1
#include "t1.h"
int A<int>::f()
{
	return 47;
}
EOF

cat <<EOF >$t25s2
#include "t2.h"
int B<int>::g()
{
	return 47;
}
EOF

cat <<EOF >$t25s3
#include "t1.h"
#include "t2.h"
#include "t3.h"
extern "C" void exit(int);
main()
{
	A<int> a;
	B<int> b;
	if (a.f() != 47)
		exit(1);
	b.f();
	if (b.g() != 47)
		exit(2);
	f(1234L);
	if (f(83) != 57)
		exit(3);
	exit(0);
}
EOF

cat <<EOF >$t25s4
#include "t3.h"
int f(int t)
{
	return 57;
}
EOF

cat <<EOF >$t26
#include "t26.h"
main()
{
	A<int> a;
	A<double> b;
	a.f();
	b.f();
	a.g();
	b.g();
	f(37, 12.34);
	f("xxx", 'x');
	return 0;
}
EOF

cat <<EOF >$t26h
template <class T> struct A {
	void f();
	void g();
};
template <class T, class U> void f(T, U);
template <class T> void A<T>::f() {}
template <class T> void A<T>::g() {}
template <class T, class U> void f(T, U) {}
EOF

cat <<EOF >$t27
#include "t27.h"
#ifdef OVER
int A<int>::x = 47;
#endif
main()
{
	A<int> a;
	if (a.f() != VAL)
		return 1;
	return 0;
}
EOF

cat <<EOF >$t27h
template <class T> struct A {
	int f();
	static int x;
};
EOF

cat <<EOF >$t27c
template <class T> int A<T>::x = 37;
template <class T> int A<T>::f()
{
	return x;
}
EOF

cat <<EOF >$t28
#include "t28.h"
main()
{
	A<int> a;

	int i = a.x;
	if (i != VAL)
		return 1;
	return 0;
}
EOF

cat <<EOF >$t28h
template <class T> struct A {
	static int x;
};
EOF

cat <<EOF >$t28c
#ifdef INIT
template <class T> int A<T>::x = 37;
#endif
EOF

cat <<EOF >$t29
template <short s> struct A {
	void f();
};
template <short s> void A<s>::f()
{
}
template <long x> struct B {
	void f();
};
template <long x> void B<x>::f()
{
}
template <char c> struct C {
	void f();
};
template <char c> void C<c>::f()
{
}
main()
{
	A<short(-23)> a;
	B<59L> b;
	C<'x'> c;

	a.f();
	b.f();
	c.f();
}
EOF

cat <<EOF >$t30
struct A {
	struct B {
		virtual void f();
	};
	virtual void f();
};
main()
{
	A a;
	A* ap = &a;
	A::B ab;
	A::B* abp = &ab;

	ap->f();
	abp->f();
}
EOF

cat <<EOF >$t31
extern "C" void strcpy(char*, char*);
extern f(int);
main()
{
	char buf[10];
	strcpy(buf, "");
	f(37);
}
EOF

cat <<EOF >$t32
template <class T> struct A {
#ifdef INLINE
	inline void f();
#else
	void f();
#endif
};
#ifdef INLINE
template <class T> inline void A<T>::f()
#else
template <class T> void A<T>::f()
#endif
{
}
main()
{
	A<int> a;
	a.f();
}
EOF

cat <<EOF >$t33
template <class T> struct A {
	void f();
};
template <class T> void A<T>::f() {}
#include <complex.h>
main()
{
	complex c;
	A<complex> a;

	c = 12.34;
	c = c * c;
	a.f();
}
EOF

cat <<EOF >$t34
template <class T> struct A {
	static int x;
	void f();
};
template <class T> void A<T>::f() {}
template <class T> int A<T>::x;
main()
{
	A<int> a;
	a.f();
}
EOF

cat <<EOF >$t35h
template <class T> struct A {
	void f();
	void g();
};
EOF

cat <<EOF >$t35c
template <class T> void A<T>::f()
{
#ifdef BAD
	zzz();
#endif
}
template <class T> void A<T>::g()
{
#ifndef BAD
	zzz();
#endif
}
EOF

cat <<EOF >$t35
#include "t35_1.h"
main()
{
	A<int> a;
	a.f();
}
EOF

cat <<EOF >$t36
template <class T> struct A {
	virtual void f();
	void g();
};
template <class T> void A<T>::f()
{
	g();
}
template <class T> void A<T>::g()
{
}
main()
{
	A<short>* p;
	A<short> a;
	p = &a;
	p->f();
}
EOF

cat <<EOF >$t37
template <class T> struct A {
	virtual void f1();
	void f2();
};
template <class T> void A<T>::f1()
{
}
template <class T> void A<T>::f2()
{
}
main()
{
	A<int> a;
	A<int>* p = &a;

	p->f1();
	p->f2();
}
void A<int>::f1()
{
}
EOF

cat <<EOF >$t38
template <class T> struct A {
	void f();
};
template <class T> void A<T>::f()
EOF

cat <<EOF >$t39
template <class T> void f(T);
template <class T> void f(T)
{
}
main()
{
	f(37);
	f(37L);
}
EOF

cat <<EOF >$t40
#include "t40_1.h"
main()
{
	f(37);
	f(37L);
}
EOF

cat <<EOF >$t40h
template <class T> void f(T);
EOF
cat <<EOF >$t40c
extern int x;
template <class T> void f(T)
{
	x++;
}
EOF

cat <<EOF >$t41
template <class T> struct A {
	void f1();
	void f2();
	void g();
};
template <class T> void A<T>::f1()
{
	g();
}
template <class T> void A<T>::g()
{
}
main()
{
	A<int> a;
	a.f1();
	a.f2();
}
EOF

cat <<EOF >$t42_1
template <int* ip> struct A {
	void g();
};
template <int* ip> void A<ip>::g() {}
int i;
main()
{
	A<&i> a;
	a.g();
}
EOF

cat <<EOF >$t42_2
template <int* ip> struct A {
	void g();
};
template <int* ip> void A<ip>::g() {}
struct B {
	static int i;
};
int B::i = 37;
main()
{
	A<&B::i> a;
	a.g();
}
EOF

cat <<EOF >$t42_3
typedef void (*fp)(int);
template <fp p> struct A {
	void g();
};
template <fp p> void A<p>::g() {}
void f(int) {}
main()
{
	A<&f> a;
	a.g();
}
EOF

cat <<EOF >$t42_4
typedef void (*fp)(int);
template <fp p> struct A {
	void g();
};
template <fp p> void A<p>::g() {}

struct B {
	static void f(int);
};
void B::f(int) {}
main()
{
	A<&B::f> a;
	a.g();
}
EOF

cat <<EOF >$t42_5
struct B {
	void f(int);
};
void B::f(int) {}
typedef void (B::*pmf)(int);
template <pmf ip> struct A {
	void g();
};
template <pmf ip> void A<ip>::g() {}
B b;
main()
{
	A<&B::f> a;
	a.g();
}
EOF

cat <<EOF >$t42_6
struct B {
	int i;
};
typedef int B::*pmf;
template <pmf ip> struct A {
	void g();
};
template <pmf ip> void A<ip>::g() {}
B b;
main()
{
	A<&B::i> a;
	a.g();
}
EOF

cat <<EOF >$t43
template <class T, class U> void f(T, U);
template <class T, class U> void f(T, U) {}
struct A {};
struct B {
	void f();
};
void B::f() {}
main()
{
	A a;
	f(a, B::f);
}
EOF

cat <<EOF >$t44
#include "t8.H"
union Lurch {};
main()
{
	A<int> a;
	A<Lurch> b;
	a.f();
	b.f();
	a.g();
	b.g();
	f(37, 12.34);
	f("xxx", 'x');
	return 0;
}
EOF

cat <<EOF >$t45
#include "t45_1.h"
main()
{
	A<int> a;
	A<int>* p = &a;
	p->f1();
	p->f2();
}
EOF

cat <<EOF >$t451
template <class T> struct A {
	virtual void f1();
	virtual void f2();
};
EOF

cat <<EOF >$t452
template <class T> void A<T>::f2()
{
}
EOF

cat <<EOF >$t453
#include "t45_1.h"
void A<int>::f1()
{
}
EOF

cat <<EOF >$t46a
#include "t46_1.h"
main()
{
	A<int> a;
	a.f();
}
EOF

cat <<EOF >$t46b
#include <t46_1.h>
main()
{
	A<int> a;
	a.f();
}
EOF

cat <<EOF >$t461
template <class T> struct A {
	void f();
};
EOF

cat <<EOF >$t462
template <class T> void A<T>::f()
{
}
EOF

cat <<EOF >$t47
struct Base {
			Base();
	virtual		~Base();
};
inline Base::
Base()
{
}
inline Base::
~Base()
{
}
template <class T>
struct Der : public Base {
		Der();
	T*	tp;
};
template <class T>
Der<T>::
Der()
{
}
main()
{
	Der<int>	der;
}
EOF

cat <<EOF >$t48
template <class T, class U> struct Map {
	void f();
};
template <class T, class U> void Map<T,U>::f() {}
struct Junk;
typedef Map<void (*)(Junk*,void*), int (*)(Junk&,void*)> T;
main()
{
	T t;

	t.f();
}
EOF

cat <<EOF >$t491
template <class T> struct B {
	void f();
};
template <class T> void B<T>::f() {}
void f2()
{
	B<long> b;
	b.f();
}
EOF

cat <<EOF >$t492
template <class T> struct A {
	void f();
};
template <class T> void A<T>::f() {extern void f2(); f2();}
main()
{
	A<int> a;
	a.f();
}
EOF

cat <<EOF >$t50
template <class T> struct A {
	void f();
	T t;
};
template <class T> void A<T>::f()
{
	t.g();
}
struct B {
	void g();
};
struct C {
	void g();
};
void B::g()
{
}
void C::g()
{
}
main()
{
	A<B> b;
	A<C> c;
	A<int> d;

	b.f();
	c.f();
	d.f();
}
EOF

cat <<EOF >$t51
#include <51.h>
main()
{
	B<int> b;
	b.f();
}
EOF

cat <<EOF >$t51h
template <class T> struct B {
	void f();
};
template <class T> void B<T>::f()
{
}
EOF

cat <<EOF >$t51hx
#include "t8.h"
template <class T> struct B {
	void f();
};
template <class T> void B<T>::f()
{
}
EOF

# +++++++++++++++++++++++++++++++++
cat <<EOF >$td/t1.h
template <class T> struct A {
	int f();
};
EOF

cat <<EOF >$td/t1.C
template <class T> int A<T>::f() {return 37;}
EOF
cat <<EOF >$td/t2.h
template <class T> struct B {
	int f();
	int g();
};
EOF

cat <<EOF >$td/t2.C
template <class T> int B<T>::f()
{
	return 37;
}
template <class T> int B<T>::g()
{
	return 37;
}
EOF

cat <<EOF >$td/t3.h
template <class T> int f(T);
EOF
cat <<EOF >$td/t3.C
template <class T> int f(T t)
{
	return 83;
}
EOF
# ---------------------------------

echo OK
echo "test are in:"
echo "  $td"
echo "  $td2"
exit 0
