/* @(#) errno.h 1.3 1/27/86 17:46:39 */
/*ident	"@(#)cfront:incl/errno.h	1.3" */
/*
 * Error codes
 */

#include <sys/errno.h>
extern int* _get_errno();
#define errno (*_get_errno())
