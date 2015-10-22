/* @(#) stdarg.h 1.2 1/27/86 17:46:59 */
/*ident	"@(#)cfront:incl/stdarg.h	1.2"*/
/* stdarg.h */
/* ADAPTED FROM: */
/*	@(#)varargs.h	1.2	*/

/*
	USAGE:
		f( arg-declarations ... ) {
			va_list ap;
			va_start(ap, parmN);	// parmN == last named arg
			// ...
			type arg = va_arg(ap, type);
			// ...
			va_end(ap);
		}
*/

#ifndef va_start

typedef char *va_list;
#define va_end(ap)
#ifdef u370
#define va_start(ap, parmN) ap =\
	(char *) ((int)&parmN + 2*sizeof(parmN) - 1 & -sizeof(parmN))
#define va_arg(ap, mode) ((mode *)(ap = \
	(char *) ((int)ap + 2*sizeof(mode) - 1 & -sizeof(mode))))[-1]
#else
#define va_start(ap, parmN) ap = (char *)( &parmN+1 )
#define va_arg(ap, mode) ((mode *)(ap += sizeof(mode)))[-1]
#endif

#endif
