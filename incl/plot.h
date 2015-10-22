/* @(#) plot.h 1.3 1/27/86 17:46:52 */
/*ident	"@(#)cfront:incl/plot.h	1.3"*/

extern int arc (int, int, int, int, int, int),
           circle (int, int, int),
           closepl (),
           cont (int, int),
           erase (),
           label (const char*),
           line (int, int, int, int),
           linemod (const char*),
           move (int, int),
           openpl (),
           point (int, int),
           space (int, int, int, int);
