#ifndef _IDEBUG_H
#define _IDEBUG_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
**	MACROS
*/
#ifdef IDEBUG
extern int idebug;
extern char* dbfilename;
extern FILE* dbfile;
#define DB(a) {a;fflush(dbfile);}
#define DBdeclare(whatever) \
	int idebug = 0; \
	char* dbfilename = 0; \
	FILE* dbfile = stderr; \
	whatever;
#else
#define DBdeclare(whatever) /**/
#define DB(a) /**/
#define dbfile stderr
#endif

#define MUSTREAD(_9fd,_9b,_9n) if(read((_9fd),(char*)(_9b),(_9n))!=(_9n)) { \
    int _7e = errno; \
    fprintf(stderr,"'%s', line %d: read(%d,0x%.8lx,%.8ld): ",__FILE__,__LINE__,_9fd,_9b,_9n); errno=_7e;perror(""); \
}
#define MUSTFREAD(_9b,_9n,_9fd) { \
int _7r = fread((char*)(_9b),(_9n),1,(_9fd)); \
if ( _7r != 1 ) { \
    int _7e = errno; \
    fprintf(stderr,"'%s', line %d: %d=fread(0x%.8lx,%ld,1,0x%.8lx): ",__FILE__,__LINE__,_7r,_9b,_9n,_9fd); errno=_7e;perror(""); \
}}
#define MUSTWRITE(_9fd,_9b,_9n) if(write(_9fd,(char*)(_9b),(_9n))!=(_9n)) { \
    int _7e = errno; \
    fprintf(stderr,"'%s', line %d: write(%d,0x%.8lx,%ld): ",__FILE__,__LINE__,_9fd,_9b,_9n); errno=_7e;perror(""); \
}
#define MUSTFWRITE(_9b,_9n,_9fd) \
if(fwrite((char*)(_9b),(_9n),1,(_9fd))!=1) { \
    int _7e = errno; \
    fprintf(stderr,"'%s', line %d: fwrite(0x%.8lx,%ld,1,0x%.8lx): ",__FILE__,__LINE__,_9b,_9n,_9fd); errno=_7e;perror(""); \
}
#define MUSTLSEEK(_9fd,_9o,_9f) if(lseek((_9fd),(_9o),(_9f))==-1) { \
    int _7e = errno; \
    fprintf(stderr,"'%s', line %d: lseek(%d,%ld,%d): ",__FILE__,__LINE__,_9fd,_9o,_9f); errno=_7e;perror(""); \
}
#define MUSTFSEEK(_9fd,_9o,_9f) if(fseek((_9fd),(_9o),(_9f))!=0) { \
    int _7e = errno; \
    fprintf(stderr,"'%s', line %d: fseek(0x%.8lx,%ld,%d): ",__FILE__,__LINE__,_9fd,_9o,_9f); errno=_7e;perror(""); \
}

#define MUSTMALLOC(_9p,_9sz,_9ptype)  { \
	if ( ((_9p) = (_9ptype)malloc(_9sz)) == NULL ) { \
		fprintf(stderr,"'%s', line %d -- can't malloc( %ld )!\n",__FILE__,__LINE__,(long)_9sz); exit(1); \
	} \
}
#define MUSTCALLOC(_9p,_9n,_9s,_9ptype)  { \
	if ( ((_9p) = (_9ptype)calloc(_9n,_9s)) == NULL ) { \
		fprintf(stderr,"'%s', line %d -- can't calloc( %ld, %ld )!\n",__FILE__,__LINE__,(long)_9n,(long)_9s); exit(1); \
	} \
}
#define MUSTREALLOC(_9p,_9sz,_9ptype)  { \
	if ( ((_9p) = (_9ptype)realloc((char*)(_9p),_9sz)) == NULL ) { \
		fprintf(stderr,"'%s', line %d -- can't realloc( 0x%.8lx, %ld )!\n",__FILE__,__LINE__,_9p,(long)_9sz); exit(1); \
	} \
}

#endif /*_IDEBUG_H*/
