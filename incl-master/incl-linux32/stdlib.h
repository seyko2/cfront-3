/* @(#) stdlib.h 1.4 1/27/86 17:47:01 */
/*ident	"@(#)cfront:incl/stdlib.h	1.4"*/

extern "C" {
    void    abort(void);
    double  atof (const char*);
    int     atoi (const char*);
    long    atol (const char*);
    void*   calloc (unsigned,unsigned);
    void    exit (int);
    void    free (void*);
    char*   getenv (const char*);
    void*   malloc (unsigned);
    int     rand ();
    void*   realloc (void*, unsigned);
    void    srand  (unsigned);
    double  strtod (const char*, char**);
    long    strtol (const char*, char**, int);
    int     system (const char*);
}
