/* @(#) dtors.c 1.1 1/27/86 17:47:57 */
/*ident	"@(#)cfront:lib/static/dtors.c	1.1"*/

typedef void (*PFV)();

void dtors()
{
	extern PFV _dtors[];
	static ddone;
	if (ddone == 0) {	// once only
		ddone = 1;
		PFV* pf = _dtors;
		while (*pf) pf++;
		while (_dtors < pf) (**--pf)();
	}
}
