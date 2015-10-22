/* @(#) math.h 1.4 1/27/86 17:46:49 */
/*ident	"@(#)cfront:incl/math.h	1.4"*/
#ifndef _POLY9
//extern int errno, signgam;

extern int abs (int);
extern double atof(const char*), frexp(double, int*);
extern double ldexp(double, int), modf(double, double*);
extern double j0(double), j1(double), jn(double);
extern double y0(double), y1(double), yn(int, double);
extern double erf(double), erfc(double);
extern double exp(double), log(double), log10(double); 
extern double pow(double, double), sqrt(double);
extern double floor(double), ceil(double), fmod(double,double), fabs(double);
extern double gamma(double);
extern double hypot(double,double);
struct exception;
extern int matherr(exception *);
extern double sinh(double), cosh(double), tanh(double);
extern double sin(double), cos(double), tan(double);
extern double asin(double), acos(double), atan(double), atan2(double,double);

/* some useful constants */
#define M_E	2.7182818284590452354
#define M_LOG2E	1.4426950408889634074
#define M_LOG10E	0.43429448190325182765
#define M_LN2	0.69314718055994530942
#define M_LN10	2.30258509299404568402
#define M_PI	3.14159265358979323846
#define M_PI_2	1.57079632679489661923
#define M_PI_4	0.78539816339744830962
#define M_1_PI	0.31830988618379067154
#define M_2_PI	0.63661977236758134308
#define M_2_SQRTPI	1.12837916709551257390
#define M_SQRT2	1.41421356237309504880
#define M_SQRT1_2	0.70710678118654752440
#if u3b || M32 || u3b15 || u3b5 || u3b2
#define MAXFLOAT	((float)3.40282346638528860e+38)
#endif
#if pdp11 || vax
#define MAXFLOAT	((float)1.701411733192644299e+38)
#endif
#if gcos
#define MAXFLOAT	((float)1.7014118219281863150e+38)
#endif
#define HUGE	MAXFLOAT

#define _ABS(x)	((x) < 0 ? -(x) : (x))
#define _REDUCE(TYPE, X, XN, C1, C2)	{ \
	double x1 = (double)(TYPE)X, x2 = X - x1; \
	X = x1 - (XN) * (C1); X += x2; X -= (XN) * (C2); }
#define _POLY1(x, c)	((c)[0] * (x) + (c)[1])
#define _POLY2(x, c)	(_POLY1((x), (c)) * (x) + (c)[2])
#define _POLY3(x, c)	(_POLY2((x), (c)) * (x) + (c)[3])
#define _POLY4(x, c)	(_POLY3((x), (c)) * (x) + (c)[4])
#define _POLY5(x, c)	(_POLY4((x), (c)) * (x) + (c)[5])
#define _POLY6(x, c)	(_POLY5((x), (c)) * (x) + (c)[6])
#define _POLY7(x, c)	(_POLY6((x), (c)) * (x) + (c)[7])
#define _POLY8(x, c)	(_POLY7((x), (c)) * (x) + (c)[8])
#define _POLY9(x, c)	(_POLY8((x), (c)) * (x) + (c)[9])

struct exception {
	int type;
	char *name;
	double arg1;
	double arg2;
	double retval;
};

#define DOMAIN		1
#define	SING		2
#define	OVERFLOW	3
#define	UNDERFLOW	4
#define	TLOSS		5
#define	PLOSS		6
#endif

#define PI	3.1415926535897932384
#define	PI2	6.2831853071795864769
