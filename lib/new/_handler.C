/* @(#) _handler.c 1.2 1/27/86 17:47:50 */
/*ident	"@(#)cfront:lib/new/_handler.c	1.2"*/
typedef void (*PFVV)();

extern PFVV _new_handler = 0;

extern PFVV
set_new_handler(PFVV handler)
{
	PFVV rr = _new_handler;
	_new_handler = handler;
	return rr;
}
