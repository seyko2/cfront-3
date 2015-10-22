/* @(#) stdio.h 1.8 1/31/86 13:40:51 */
/*ident	"@(#)cfront:incl/stdio.h	1.8"*/

extern "C" {

#define	BUFSIZ 1024
#define	_NFILE 20

#ifndef va_start
    typedef char *va_list;  //for the declaration of vprintf, vfprintf, vsprintf.
    #define va_start /
#endif

#if 0 //ndef FILE
extern	struct	_iobuf {
	int	_cnt;
	char	*_ptr;
	char	*_base;
//	int	_bufsiz;	//Add this line for bsd 4.2 or later
//	short	_flag;		//Replace next line for bsd 4.2 or later
	char	_flag;
	char	_file;
} _iob[_NFILE];
# endif

#define _IOFBF    0000
#define	_IOREAD   0001
#define	_IOWRT    0002
#define	_IONBF    0004
#define	_IOMYBUF  0010
#define	_IOEOF    0020
#define	_IOERR    0040
#define	_IOLBF    0100
#define	_IORW     0200

#ifndef NULL
#define	NULL 0
#endif

#define	FILE struct _iobuf
#define	EOF (-1)

extern int _flsbuf(unsigned,FILE*);
extern int _filbuf(FILE*);

#if 0
#define	stdin (&_iob[0])
#define	stdout (&_iob[1])
#define	stderr (&_iob[2])
#define	getc(p) (--(p)->_cnt>=0? *(p)->_ptr++&0377:_filbuf(p))
#define	getchar() getc(stdin)
#define putc(x,p) (--(p)->_cnt>=0? ((int)(*(p)->_ptr++=(unsigned)(x))):_flsbuf((unsigned)(x),p))
#define	putchar(x) putc(x,stdout)
#define	feof(p) (((p)->_flag&_IOEOF)!=0)
#define	ferror(p) (((p)->_flag&_IOERR)!=0)
#define	fileno(p) ((p)->_file)
#endif

extern FILE* fopen(const char*, const char*);
extern FILE* fdopen(int, const char*);
extern FILE* freopen(const char*, const char*, FILE*);
extern long ftell(const FILE*);
extern char* fgets(char*, int, FILE*);

#define L_ctermid	9
#define L_cuserid	9
#define	P_tmpdir "/tmp/"
#define	L_tmpnam (sizeof(P_tmpdir) + 15)

extern char* gets(char*);
extern int puts(const char *s);
extern int fputs(const char *s, FILE *stream);
extern int printf(const char* ...);
extern int fprintf(FILE*, const char* ...);
extern int sprintf(char*, const char* ...);
extern int scanf(const char* ...);
extern int fscanf(FILE*, const char* ...);
extern int sscanf(char*, const char* ...);
extern int fread(char*, int, int, FILE*);
extern int fwrite(const char*, int, int, FILE*);
extern int fclose(FILE*);
extern int fflush(FILE *stream);
extern void clearerr (FILE *__stream);
extern int fseek(FILE*, long, int);
extern void rewind(FILE*);
extern int getw(FILE*);
extern int fgetc(FILE*);
extern FILE* popen(const char*, const char*);
extern int pclose(FILE*);
extern int putw(int, FILE*);
extern int fputc(int, FILE*);
extern void setbuf(FILE *stream, char *buf);
extern int ungetc(int, FILE*);

extern void exit(int);
extern void abort(void);

extern int atoi(const char*);
extern double atof(const char*);
extern long atol(const char*);

#define _bufend(p)	_bufendtab[(p)->_file]
#define _bufsiz(p)	(_bufend(p) - (p)->_base)

extern FILE      *tmpfile ();
extern char	*ctermid(char*),
                *cuserid(char*),
                *tempnam(char*, char*),
                *tmpnam(char*);
extern int      vprintf(char*, va_list),
                vfprintf(FILE*, char*, va_list), 
                vsprintf(char*, char*, va_list),
		setvbuf(FILE*, char*, int, int); 

extern void perror (const char*);

// extern int errno;
extern char* sys_errlist[];
extern int sys_nerr;
extern unsigned char *_bufendtab[];

int feof(FILE *p);
int ferror(FILE *p);
int fileno(FILE *p);
char* strerror(int);

#ifndef stdin
extern FILE* _get_stdin();
extern FILE* _get_stdout();
extern FILE* _get_stderr();

#define stdin  _get_stdin()
#define stdout _get_stdout()
#define stderr _get_stderr()
#define getc(fp) fgetc(fp)
#define getchar() getc(stdin)
#define putc(c,fp) fputc(c,fp)
#define putchar(c) putc(c,stdout)
#endif

void _main();

}
