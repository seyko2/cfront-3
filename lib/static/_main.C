/* @(#) _main.c 1.2 1/27/86 17:47:56 */
/*ident	"@(#)cfront:lib/static/_main.c	1.2"*/
extern void
_main()
{
	typedef void (*PFV)();
	extern PFV _ctors[];

       extern int atexit(void*);
       extern void dtors();
       atexit((void*)dtors);

	for (PFV* pf=_ctors; *pf; pf++) (**pf)();
}
