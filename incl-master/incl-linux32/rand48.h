/* @(#) rand48.h 1.2 1/27/86 17:46:56 */
/*ident	"@(#)cfront:incl/rand48.h	1.2"*/
extern double drand48 (),
              erand48 (unsigned short);

extern long lrand48 (),
            nrand48 (unsigned short),
            mrand48 (),
            jrand48 (unsigned short);

extern void lcong48 (unsigned short);
extern void srand48 (long);

extern unsigned short *seed48 (unsigned short);
