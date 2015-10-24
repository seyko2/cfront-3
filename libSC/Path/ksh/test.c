/*ident "@(#)Path:ksh/test.c	3.1" */
/*
 * Code for test
 * Stolen from ksh
 * Originally written by David Korn
 * Modified by me
 */

/* The values of the enums in ../Path.h
*  must match the values used in this file.
*/

#include "defs.h"

static MSG       e_devfdNN       = "/dev/fd/+([0-9])";

#define	tio(a,f,id)	(_sh_access_Path_ATTLC(a,f,id)==0)
static time_t ftime_compare();
static int test_stat();
static int test_inode();
static int test_type();
static int io_access();
static struct stat statb;

unop_test_Path_ATTLC(op,arg,id)
register int op;
register char *arg;
register int id;
{
	switch(op)
	{
	case 'a':
		return(tio(arg, F_OK, id));
	case 'r':
		return(tio(arg, R_OK, id));
	case 'w':
		return(tio(arg, W_OK, id));
	case 'x':
		return(tio(arg, X_OK, id));
	case 'd':
		return(test_type(arg,S_IFMT,S_IFDIR));
	case 'c':
		return(test_type(arg,S_IFMT,S_IFCHR));
	case 'b':
		return(test_type(arg,S_IFMT,S_IFBLK));
	case 'f':
		return(test_type(arg,S_IFMT,S_IFREG));
	case 'u':
		return(test_type(arg,S_ISUID,S_ISUID));
	case 'g':
		return(test_type(arg,S_ISGID,S_ISGID));
	case 'k':
#ifdef S_ISVTX
		return(test_type(arg,S_ISVTX,S_ISVTX));
#else
		return(0);
#endif /* S_ISVTX */

	case 'V':
#ifdef FS_3D
	{
		struct stat statb;
		if(lstat(arg,&statb)<0)
			return(0);
		return((statb.st_mode&(S_IFMT|S_ISVTX|S_ISUID))==(S_IFDIR|S_ISVTX|S_ISUID));
	}
#else
		return(0);
#endif /* FS_3D */

	case 'L':
#ifdef LSTAT
	{
		struct stat statb;
		if(lstat(arg,&statb)<0)
			return(0);
		return((statb.st_mode&S_IFMT)==S_IFLNK);
	}
#else
		return(0);
#endif	/* S_IFLNK */

	case 'S':
#ifdef S_IFSOCK
		return(test_type(arg,S_IFMT,S_IFSOCK));
#else
		return(0);
#endif	/* S_IFSOCK */

	case 'p':
#ifdef S_IFIFO
		return(test_type(arg,S_IFMT,S_IFIFO));
#else
		return(0);
#endif	/* S_IFIFO */

	case 's':
	case 'O':
	case 'G':
	{
		struct stat statb;
		if(test_stat(arg,&statb)<0)
			return(0);
		if(op=='s')
			return(statb.st_size>0);
		else if(op=='O')
			return(statb.st_uid==geteuid());
		return(statb.st_gid==getegid());
	}

	case 't':
		if(isdigit(*arg) && arg[1]==0)
			 return(isatty(*arg-'0'));
		return(0);
	}
}

binop_test_Path_ATTLC(op,left,right,id)
char *left, *right;
register int op;
register int id;
{
	switch(op)
	{
		/* op must be one of the following values */
		case 0:  /* ef */
			return(test_inode(left,right));
		case 1:  /* nt */
			return(ftime_compare(left,right)>0);
		case 2:  /* ot */
			return(ftime_compare(left,right)<0);
	}
	/* NOTREACHED */
}

/*
 * returns the modification time of f1 - modification time of f2
 */

static time_t ftime_compare(file1,file2)
char *file1,*file2;
{
	struct stat statb1,statb2;
	if(test_stat(file1,&statb1)<0)
		statb1.st_mtime = 0;
	if(test_stat(file2,&statb2)<0)
		statb2.st_mtime = 0;
	return(statb1.st_mtime-statb2.st_mtime);
}

/*
 * return true if inode of two files are the same
 */

static test_inode(file1,file2)
char *file1,*file2;
{
	struct stat stat1,stat2;
	if(test_stat(file1,&stat1)>=0  && test_stat(file2,&stat2)>=0)
		if(stat1.st_dev == stat2.st_dev && stat1.st_ino == stat2.st_ino)
			return(1);
	return(0);
}

/*
/* These are the mode values supplied as argument to access():
/*     #define R_OK    4/* test for read permission */
/*     #define W_OK    2/* test for write permission */
/*     #define X_OK    1/* test for execute (search) permission */
/*     #define F_OK    0/* test for presence of file */
/*
/* And the status information word st_mode (in the info returned by stat()) 
/* has the following bits:
/*     #define    S_IFMT        0170000/* type of file */
/*     #define    S_IFIFO       0010000/* fifo special */
/*     #define    S_IFCHR       0020000/* character special */
/*     #define    S_IFDIR       0040000/* directory */
/*     #define    S_IFBLK       0060000/* block special */
/*     #define    S_IFREG       0100000/* regular file */
/*     #define    S_IFLNK       0120000/* symbolic link */
/*     #define    S_IFSOCK      0140000/* socket */
/*     #define    S_ISUID       0004000/* set user id on execution */
/*     #define    S_ISGID       0002000/* set group id on execution */
/*     #define    S_ISVTX       0001000/* save swapped text even after use */
/*     #define    S_IREAD       0000400/* read permission, owner */
/*     #define    S_IWRITE      0000200/* write permission, owner */
/*     #define    S_IEXEC       0000100/* execute/search permission, owner */
/*
/*     The mode bits 0000070 and 0000007 encode  group  and  others
/*     permissions (see chmod(2)).
*/

/*
 * This version of access checks against the desired uid/gid.
 * This version is guaranteed to match access(2) only in those
 * cases when we're checking against real id other than root.
 * The static buffer statb is shared with test_type.
 */

/* The setting of <realid> shouldn't make a difference in case 
 * <mode> = F_OK, unless access(2) is written screwy.  When <mode>
 * = F_OK, _sh_access should return 1 just if the file can be
 * statted.
 */

int _sh_access_Path_ATTLC(name, mode, realid)
register char	*name;
register int mode;
register int realid;
{
	int uid, gid;
	if (realid)
	{
		uid = getuid();
		gid = getgid();
	}
	else
	{
		uid = geteuid();
		gid = getegid();
	}
	if(strmatch_Path_ATTLC(name,(char*)e_devfdNN))
		return(io_access(atoi(name+8),mode));
	if(uid != 0 && realid)
		return(access(name,mode));
	if(stat(name, &statb) == 0)
	{
		if(mode == F_OK)
			return(0);
		else if(uid == 0)
		{
			if((statb.st_mode&S_IFMT)!=S_IFREG || mode!=X_OK)
				return(0);
		    	/* root needs execute permission for someone */
			mode = (S_IEXEC|(S_IEXEC>>3)|(S_IEXEC>>6));
		}
		else if(uid == statb.st_uid)
			mode <<= 6;
		else if(gid == statb.st_gid)
			mode <<= 3;
#if defined(NGROUPS) && NGROUPS>0
		else
		{
			/* in both the real and effective cases, permission is
			   checked against the groups in the group access list.
			   (if you don't believe me, read the intro(2) and access(2)
			   man pages.)
			*/
			int groups[NGROUPS];
			register int n;
			n = getgroups(NGROUPS,groups);
			while(--n >= 0)
			{
				if(groups[n] == statb.st_gid)
				{
					mode <<= 3;
					break;
				}
			}
		}
#endif /* NGROUPS */
		if(statb.st_mode & mode)
			return(0);
	}
	return(-1);
}


/*
 * Return true if the mode bits of file <f> corresponding to <mask> have
 * the value equal to <field>.  If <f> is null, then the previous stat
 * buffer is used.
 */

static test_type(f,mask,field)
char *f;
int field;
{
	if(f && test_stat(f,&statb)<0)
		return(0);
	return((statb.st_mode&mask)==field);
}

/*
 * do an fstat() for /dev/fd/n, otherwise stat()
 */

static int test_stat(f,buff)
char *f;
struct stat *buff;
{
	if(strmatch_Path_ATTLC(f,(char*)e_devfdNN))
		return(fstat(atoi(f+8),buff));
	else
		return(stat(f,buff));
}


/*
 * returns access information on open file <fd>
 * returns -1 for failure, 0 for success
 * <mode> is the same as for access()
 */

static io_access(fd,mode)
register int mode;
{
	register int flags;
	register struct fileblk *fp;
#ifndef F_GETFL
	struct stat statb;
#endif /* F_GETFL */
	if(mode==X_OK)
		return(-1);
#ifdef F_GETFL
	flags = fcntl(fd,F_GETFL,0);
#else
	flags = fstat(fd,&statb);
#endif /* F_GETFL */
	if(flags < 0)
		return(-1);
#ifdef F_GETFL
	if(mode==R_OK && (flags&1))
		return(-1);
	if(mode==W_OK && !(flags&3))
		return(-1);
#endif /* F_GETFL */
	return(0);
}
