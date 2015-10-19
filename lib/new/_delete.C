/* @(#) _delete.c 1.2 1/27/86 17:47:49 */
/*ident	"@(#)cfront:lib/new/_delete.c	1.2"*/
free(char*);
extern void
operator delete(void* p)
{
	if (p) free( (char*)p );
}
