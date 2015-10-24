/*ident	"@(#)Regex:libx/error.h	3.1" */
/*
 * G. S. Fowler
 * AT&T Bell Laboratories
 *
 * error and message formatter external definitions
 */

#ifndef ERROR_INFO

#ifndef VOID
#if __STDC__ || __cplusplus
#define VOID	void
#else
#define VOID	char
#endif
#endif

#define ERROR_INFO	0		/* info message -- no err_id	*/
#define ERROR_WARNING	1		/* warning message		*/
#define ERROR_ERROR	2		/* error message -- no err_exit	*/
#define ERROR_FATAL	3		/* error message with err_exit	*/
#define ERROR_PANIC	ERROR_LEVEL	/* panic message with err_exit	*/

#define ERROR_LEVEL	00077		/* level portion of status	*/
#define ERROR_SYSTEM	00100		/* report system errno message	*/
#define ERROR_SOURCE	00200		/* args 2-3 are FILE and LINE	*/
#define ERROR_USAGE	00400		/* usage message		*/
#define ERROR_PROMPT	01000		/* omit trailing newline	*/
#define ERROR_NOID	02000		/* omit err_id			*/
#define ERROR_LIBRARY	04000		/* library routine error	*/

extern int	errcount;		/* level>=ERROR_ERROR count	*/
extern int	errno;			/* system call error status	*/

#if __STDC__ || __cplusplus
#if __cplusplus
extern "C" {
#endif
extern VOID*	seterror(int opt, ...);
extern void	error(int level, ...);
extern void	liberror(char* lib, int level, ...);
extern void	verror(char* lib, int level, void* ap);
#if __cplusplus
}
#endif
#else
extern VOID*	seterror();
extern void	error();
extern void	liberror();
extern void	verror();
#endif

#endif
