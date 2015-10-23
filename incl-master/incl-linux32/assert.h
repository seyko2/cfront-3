/* @(#) assert.h 1.2 1/27/86 17:46:34 */
/*ident	"@(#)cfront:incl/assert.h	1.2"*/
#ifdef NDEBUG
#define assert(EX)
#else
extern void _assert(char*, char*, int);
#define assert(EX) if (EX) ; else _assert("EX", __FILE__, __LINE__)
#endif
