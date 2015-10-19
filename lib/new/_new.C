/* @(#) _new.c 1.2 1/27/86 17:47:52 */
/*ident	"@(#)cfront:lib/new/_new.c	1.2"*/
typedef void (*PFVV)();

extern PFVV _new_handler;

extern void*
operator new(long size)
{
	extern char* malloc(unsigned);
	char* p;

	while ( (p=malloc(size))==0 ) {
		if(_new_handler)
			(*_new_handler)();
		else
			return 0;
	}
	return (void*)p;
}
