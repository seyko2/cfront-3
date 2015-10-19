/* @(#) exit.c 1.4 1/27/86 17:47:57 */
/*ident	"@(#)cfront:lib/static/exit.c	1.4"*/

extern void _exit(int);
extern void _cleanup();
extern void dtors();

extern void exit(int i)
{
	dtors();
	_cleanup();
	_exit(i);
}
