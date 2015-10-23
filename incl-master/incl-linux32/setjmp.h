/* @(#) setjmp.h 1.2 1/27/86 17:46:58 */
/*ident	"@(#)cfront:incl/setjmp.h	1.2"*/
#ifndef _JBLEN

#if vax || M32 || u3b15 || u3b5 || u3b2
#define _JBLEN	10
#endif

#if pdp11
#define _JBLEN	3
#endif

#if u370
#define _JBLEN	4
#endif

#if u3b
#define _JBLEN	11
#endif

typedef int jmp_buf[_JBLEN];

extern int setjmp(jmp_buf);
extern void longjmp(jmp_buf, int);

#endif
