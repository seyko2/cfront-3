/* @(#) search.h 1.3 1/27/86 17:46:57 */
/*ident	"@(#)cfront:incl/search.h	1.3"*/

/* BSEARCH(3C) */
extern char *bsearch(const char*, char*, unsigned, int, int);

/* HSEARCH(3C) */
typedef struct entry { char *key, *data; } ENTRY;
typedef enum { FIND, ENTER } ACTION;

extern ENTRY *hsearch (ENTRY,ACTION);
extern int hcreate(unsigned);
extern void hdestroy ();

/* LSEARCH(3C) */                       
typedef int (*PF) ();
extern char *lsearch (const char*,  char*, unsigned*, unsigned, PF);
extern char *lfind (const char*, char*, unsigned*, unsigned, PF);

/* TSEARCH(3C) */
typedef enum { preorder, postorder, endorder, leaf } VISIT;

extern char *tsearch (const char*, char**, int);
extern char *tfind (const char*, char**, int);
extern char *tdelete (const char*, char**, int);
extern void twalk(char*,void (*)());
