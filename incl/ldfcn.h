/* @(#) ldfcn.h 1.6 1/30/86 17:45:38 */
/*ident	"@(#)cfront:incl/ldfcn.h	1.6"*/
#ifndef FILE
#       include <stdio.h>
#endif

#ifndef FILHDR
#       include <filehdr.h>
#endif

#ifndef LDFILE
struct	ldfile {
	int	_fnum_;		/* so each instance of an LDFILE is unique */
	FILE	*ioptr;		/* system I/O pointer value */
	long	offset;		/* absolute offset to the start of the file */
	FILHDR	header;		/* the file header of the opened file */
	unsigned short	type;		/* indicator of the type of the file */
};


/*
	provide a structure "type" definition, and the associated
	"attributes"
*/

#define	LDFILE		struct ldfile
#define IOPTR(x)	x->ioptr
#define OFFSET(x)	x->offset
#define TYPE(x)		x->type
#define	HEADER(x)	x->header
#define LDFSZ		sizeof(LDFILE)

/*
	define various values of TYPE(ldptr)
*/

#define ARTYPE 	0177545

/*
	define symbolic positioning information for FSEEK (and fseek)
*/

#define BEGINNING	0
#define CURRENT		1
#define END		2

/*
	define a structure "type" for an archive header
*/

typedef struct
{
	char ar_name[16];
	long ar_date;
	int ar_uid;
	int ar_gid;
	long ar_mode;
	long ar_size;
} archdr;

#define	ARCHDR	archdr
#define ARCHSZ	sizeof(ARCHDR)


/*
	define some useful symbolic constants
*/

#define SYMTBL	0	/* section nnumber and/or section name of the Symbol Table */

#define	SUCCESS	 1
#define	CLOSED	 1
#define	FAILURE	 0
#define	NOCLOSE	 0
#define	BADINDEX	-1L

#define	OKFSEEK	0

/*
	define macros to permit the direct use of LDFILE pointers with the
	standard I/O library procedures
*/

extern LDFILE *ldopen(char*, LDFILE*);
extern LDFILE *ldaopen(const char*, LDFILE*);

#define GETC(ldptr)	getc(IOPTR(ldptr))
#define GETW(ldptr)	getw(IOPTR(ldptr))
#define FEOF(ldptr)	feof(IOPTR(ldptr))
#define FERROR(ldptr)	ferror(IOPTR(ldptr))
#define FGETC(ldptr)	fgetc(IOPTR(ldptr))
#define FGETS(s,n,ldptr)	fgets(s,n,IOPTR(ldptr))
#define FILENO(ldptr)	fileno(IOPTR(ldptr))
#define FREAD(p,s,n,ldptr)	fread(p,s,n,IOPTR(ldptr))
#define FSEEK(ldptr,o,p)	fseek(IOPTR(ldptr),(p==BEGINNING)?(OFFSET(ldptr)+o):o,p)
#define FTELL(ldptr)	ftell(IOPTR(ldptr))
#define FWRITE(p,s,n,ldptr)       fwrite(p,s,n,IOPTR(ldptr))
#define REWIND(ldptr)	rewind(IOPTR(ldptr))
#define SETBUF(ldptr,b)	setbuf(IOPTR(ldptr),b)
#define UNGETC(c,ldptr)		ungetc(c,IOPTR(ldptr))
#define STROFFSET(ldptr)	(HEADER(ldptr).f_symptr + HEADER(ldptr).f_nsyms * 18) /* 18 == SYMESZ */
#endif

#ifndef LINENO
#       include <linenum.h>
#endif

#ifndef SCNHDR
#       include <scnhdr.h>
#endif

#ifndef SYMENT
#       include <syms.h>
#endif

extern int ldaclose (LDFILE*),
           ldahread (LDFILE*, ARCHDR*),
           ldclose (LDFILE*),
           ldfhread (LDFILE*, FILHDR*),
           ldlinit (LDFILE*, long),
           ldlitem (LDFILE*, unsigned short, LINENO),// must include <linenum.h>
           ldlread (LDFILE*, long, unsigned short, LINENO),
           ldlseek (LDFILE*,unsigned short),
           ldnlseek (LDFILE*, const char*),
           ldnrseek (LDFILE*, const char*),
           ldnshread (LDFILE*, unsigned short, SCNHDR*),//must include<scnhdr.h>
           ldnsseek (LDFILE*, const char*),
           ldohseek (LDFILE*),
           ldrseek (LDFILE*, unsigned short),
           ldshread (LDFILE*, const char*, SCNHDR*),
           ldsseek (LDFILE*, unsigned short),
           ldtbread (LDFILE*, long, SYMENT*),  // must include <syms.h>
           ldtbseek (LDFILE*);
extern long ldtbindex (LDFILE*),
            sgetl (char*);
extern void sputl (long, char*);
