/* @(#) curses.h 1.4 1/27/86 17:46:37 */
/*ident	"@(#)cfront:incl/curses.h	1.4"*/
#ifndef WINDOW

# ifndef 	NONSTANDARD
#  include  <stdio.h>
  /*
   * This trick is used to distinguish between USG and V7 systems.
   * We assume that L_ctermid is only defined in stdio.h in USG
   * systems, but not in V7 or Berkeley UNIX.
   */
#  ifdef L_ctermid
#  define USG
#  endif
#  include  <unctrl.h>
#  ifdef USG
#   include <termio.h>
   typedef struct termio SGTTY;
#  else
#   include <sgtty.h>
   typedef struct sgttyb SGTTY;
#  endif
# else   NONSTANDARD 
/*
 * NONSTANDARD is intended for a standalone program (no UNIX)
 * that manages screens.  The specific program is Alan Hewett's
 * ITC, which runs standalone on an 11/23 (at least for now).
 * It is unclear whether this code needs to be supported anymore.
 */
# define NULL 0
# endif   NONSTANDARD 

# define	bool	char
# define	reg	register

/*
 * chtype is the type used to store a character together with attributes.
 * It can be set to "char" to save space, or "long" to get more attributes.
 */
# ifdef	CHTYPE
	typedef	CHTYPE chtype;
# else
	typedef unsigned short chtype;
# endif   CHTYPE 

# define	TRUE	(1)
# define	FALSE	(0)
# define	ERR	(-1)
# define	OK	(0)

# define	_SUBWIN		01
# define	_ENDLINE	02
# define	_FULLWIN	04
# define	_SCROLLWIN	010
# define	_FLUSH		020
# define	_ISPAD		040
# define	_STANDOUT	0200
# define	_NOCHANGE	-1

struct _win_st {
	short	_cury, _curx;
	short	_maxy, _maxx;
	short	_begy, _begx;
	short	_flags;
	chtype	_attrs;
	bool	_clear;
	bool	_leave;
	bool	_scroll;
	bool	_use_idl;
	bool	_use_keypad;	/* 0=no, 1=yes, 2=yes/timeout */
	bool	_use_meta;	/* T=use the meta key */
	bool	_nodelay;	/* T=don't wait for tty input */
	chtype	**_y;
	short	*_firstch;
	short	*_lastch;
	short	_tmarg,_bmarg;
};

extern int	LINES, COLS;

typedef struct _win_st	WINDOW;
extern WINDOW	*stdscr, *curscr;

extern char	*Def_term, ttytype[];

typedef struct screen	SCREEN;

# ifndef NOMACROS
#  ifndef MINICURSES
/*
 * psuedo functions for standard screen
 */
# define	addch(ch)	waddch(stdscr, ch)
# define	getch()		wgetch(stdscr)
# define	addstr(str)	waddstr(stdscr, str)
# define	getstr(str)	wgetstr(stdscr, str)
# define	move(y, x)	wmove(stdscr, y, x)
# define	clear()		wclear(stdscr)
# define	erase()		werase(stdscr)
# define	clrtobot()	wclrtobot(stdscr)
# define	clrtoeol()	wclrtoeol(stdscr)
# define	insertln()	winsertln(stdscr)
# define	deleteln()	wdeleteln(stdscr)
# define	refresh()	wrefresh(stdscr)
# define	inch()		winch(stdscr)
# define	insch(c)	winsch(stdscr,c)
# define	delch()		wdelch(stdscr)
# define	standout()	wstandout(stdscr)
# define	standend()	wstandend(stdscr)
# define	attron(at)	wattron(stdscr,at)
# define	attroff(at)	wattroff(stdscr,at)
# define	attrset(at)	wattrset(stdscr,at)

# define	setscrreg(t,b)	wsetscrreg(stdscr, t, b)
# define	wsetscrreg(win,t,b)	(win->_tmarg=(t),win->_bmarg=(b))

/*
 * mv functions
 */
#define	mvwaddch(win,y,x,ch)	(wmove(win,y,x)==ERR?ERR:waddch(win,ch))
#define	mvwgetch(win,y,x)	(wmove(win,y,x)==ERR?ERR:wgetch(win))
#define	mvwaddstr(win,y,x,str)	(wmove(win,y,x)==ERR?ERR:waddstr(win,str))
#define	mvwgetstr(win,y,x,str)	(wmove(win,y,x)==ERR?ERR:wgetstr(win,str))
#define	mvwinch(win,y,x)	(wmove(win,y,x)==ERR?ERR:winch(win))
#define	mvwdelch(win,y,x)	(wmove(win,y,x)==ERR?ERR:wdelch(win))
#define	mvwinsch(win,y,x,c)	(wmove(win,y,x)==ERR?ERR:winsch(win,c))
#define	mvaddch(y,x,ch)		mvwaddch(stdscr,y,x,ch)
#define	mvgetch(y,x)		mvwgetch(stdscr,y,x)
#define	mvaddstr(y,x,str)	mvwaddstr(stdscr,y,x,str)
#define	mvgetstr(y,x,str)	mvwgetstr(stdscr,y,x,str)
#define	mvinch(y,x)		mvwinch(stdscr,y,x)
#define	mvdelch(y,x)		mvwdelch(stdscr,y,x)
#define	mvinsch(y,x,c)		mvwinsch(stdscr,y,x,c)

#  else  MINICURSES 

# define	addch(ch)		m_addch(ch)
# define	addstr(str)		m_addstr(str)
# define	move(y, x)		m_move(y, x)
# define	clear()			m_clear()
# define	erase()			m_erase()
# define	refresh()		m_refresh()
# define	standout()		wstandout(stdscr)
# define	standend()		wstandend(stdscr)
# define	attron(at)		wattron(stdscr,at)
# define	attroff(at)		wattroff(stdscr,at)
# define	attrset(at)		wattrset(stdscr,at)
# define	mvaddch(y,x,ch)		move(y, x), addch(ch)
# define	mvaddstr(y,x,str)	move(y, x), addstr(str)
# define	initscr			m_initscr
# define	newterm			m_newterm

/*
 * These functions don't exist in minicurses, so we define them
 * to nonexistent functions to help the user catch the error.
 */
#define	getch		m_getch
#define	getstr		m_getstr
#define	clrtobot	m_clrtobot
#define	clrtoeol	m_clrtoeol
#define	insertln	m_insertln
#define	deleteln	m_deleteln
#define	inch		m_inch
#define	insch		m_insch
#define	delch		m_delch
/* mv functions that aren't valid */
#define	mvwaddch	m_mvwaddch
#define	mvwgetch	m_mvwgetch
#define	mvwaddstr	m_mvaddstr
#define	mvwgetstr	m_mvwgetstr
#define	mvwinch		m_mvwinch
#define	mvwdelch	m_mvwdelch
#define	mvwinsch	m_mvwinsch
#define	mvgetch		m_mvwgetch
#define	mvgetstr	m_mvwgetstr
#define	mvinch		m_mvwinch
#define	mvdelch		m_mvwdelch
#define	mvinsch		m_mvwinsch
/* Real functions that aren't valid */
#define box		m_box
#define delwin		m_delwin
#define longname	m_longname
#define makenew		m_makenew
#define mvprintw	m_mvprintw
#define mvscanw		m_mvscanw
#define mvwin		m_mvwin
#define mvwprintw	m_mvwprintw
#define mvwscanw	m_mvwscanw
#define newwin		m_newwin
#define _outchar		m_outchar
#define overlay		m_overlay
#define overwrite	m_overwrite
#define printw		m_printw
#define putp		m_putp
#define scanw		m_scanw
#define scroll		m_scroll
#define subwin		m_subwin
#define touchwin	m_touchwin
#define _tscroll        m_tscroll
#define _tstp		m_tstp
#define vidattr		m_vidattr
#define waddch		m_waddch
#define waddstr		m_waddstr
#define wclear		m_wclear
#define wclrtobot	m_wclrtobot
#define wclrtoeol	m_wclrtoeol
#define wdelch		m_wdelch
#define wdeleteln	m_wdeleteln
#define werase		m_werase
#define wgetch		m_wgetch
#define wgetstr		m_wgetstr
define winsch		m_winsch
#define winsertln	m_winsertln
#define wmove		m_wmove
#define wprintw		m_wprintw
#define wrefresh	m_wrefresh
#define wscanw		m_wscanw
#define setscrreg	m_setscrreg
#define wsetscrreg	m_wsetscrreg

#  endif MINICURSES

#define	getyx(win,y,x)	 y = win->_cury, x = win->_curx
#define	winch(win)	 (win->_y[win->_cury][win->_curx])


/*
 * General functions
 */

extern int clearok(WINDOW *, char);
extern int echo();
extern int noecho();
extern int raw();
extern int noraw();
extern int scroll(WINDOW *);
extern int set_term(WINDOW *);
extern int touchwin(WINDOW *);
extern int keypad(WINDOW *, char);
extern int leaveok(WINDOW *, char);
extern int nodelay(WINDOW *, char);
extern int intrflush(WINDOW *, char);
extern int meta(WINDOW *, char);
extern int typeahead(int);
extern int scrollok(WINDOW *, char);
extern int cbreak();
extern int nocbreak();
extern int resetty();
extern int savetty();
extern int mvwin(WINDOW *, int, int);
extern int overlay(WINDOW *, WINDOW *);
extern int overwrite(WINDOW *, WINDOW *);
extern int doupdate();
extern int wnoutrefresh(WINDOW *);
extern int wclear(WINDOW*);
extern int werase(WINDOW*);
extern int wclrtobot(WINDOW *);
extern int wclrtoeol(WINDOW *);
extern int winsertln(WINDOW *);
extern int wmove(WINDOW *, int, int);
extern int winsch(WINDOW *, char);
extern int wgetstr(WINDOW *, char *);
extern int waddch(WINDOW *, char);
extern int wgetch(WINDOW *);	/* because it can return KEY_*, for instance. */
extern int waddstr(WINDOW *, char *);
extern int wdelch(WINDOW *);
extern int wdeleteln(WINDOW *);

extern void nl();
extern void nonl();

extern WINDOW *	initscr();
extern WINDOW *	newwin(int, int, int, int);
extern WINDOW *	subwin(WINDOW *, int, int, int, int);
extern WINDOW *	newpad(int, int);
extern int delwin(WINDOW *);

extern void idlok(WINDOW *, char);
extern void wrefresh(WINDOW *);
extern void endwin();
extern void wattrset(WINDOW *, int);

extern char *	longname();
extern char	erasechar(), killchar();
extern SCREEN *	newterm(char *, FILE *);


typedef int (*PF) ();

extern int baudrate();
extern int beep ();
extern int box (WINDOW*, chtype, chtype);
extern int def_prog_mode ();
extern int def_shell_mode ();
extern int delay_output (int);
extern int fixterm ();
extern int flash ();
extern int flushinp ();
extern int gettmode ();
extern int has_ic ();
extern int has_il ();
extern int m_addch (chtype);
extern int m_addstr (chtype);
extern int m_clear ();
extern int m_erase ();
extern WINDOW* m_initscr();
extern int m_move (int, int);
extern screen *m_newterm (char*, FILE*, FILE*);
extern int m_refresh ();
extern int mvcur (int, int, int, int);
extern int mvprintw (int, int, char*, int);
extern int mvscanw (int, int, char*, int);
extern int mvwprintw (WINDOW*, int, int, char*, int);
extern int mvwscanw (WINDOW*, int, int, char*, int);
extern int pnoutrefresh (WINDOW*, int, int, int, int, int, int);
extern int prefresh (WINDOW*, int, int, int, int, int, int);
extern int printw (char*, int);
extern int putp (char*);
extern int reset_prog_mode ();
extern int reset_shell_mode ();
extern int resetterm ();
extern int saveterm ();
extern int scanw (char*, int);
extern int setupterm (char*, int, int*);
extern int tgetent (char*, char*);
extern int tgetflag (char*);
extern int tgetnum (char*);
extern char *tgetstr (char*, char*);
extern char *tgoto (char*, int, int);
extern char *tparm (char*, int ...);
extern int   tputs (char*, int, PF);
extern int   traceoff ();
extern int   traceon ();
extern int   vidattr (int);
extern int   vidputs (int, PF);
extern int wattroff (WINDOW*, int);
extern int wattron (WINDOW*, int);
extern int wprintw (WINDOW*, char*, int);
extern int wscanw (WINDOW*, char*, int);
extern int wstandend (WINDOW*);
extern int wstandout (WINDOW*); 

/* Various video attributes */
#define A_STANDOUT	0000200
#define A_UNDERLINE	0000400
#define A_REVERSE	0001000
#define A_BLINK		0002000
#define A_DIM		0004000
#define A_BOLD		0010000

/* The next three are subject to change (perhaps to colors) so don't depend on them */
#define A_INVIS		0020000
#define A_PROTECT	0040000
#define A_ALTCHARSET	0100000

#define A_NORMAL	0000000
#define A_ATTRIBUTES	0377600
#define A_CHARTEXT	0000177

/* Funny "characters" enabled for various special function keys for input */
#define KEY_BREAK	0401		/* break key (unreliable) */
#define KEY_DOWN	0402		/* The four arrow keys ... */
#define KEY_UP		0403
#define KEY_LEFT	0404
#define KEY_RIGHT	0405		/* ... */
#define KEY_HOME	0406		/* Home key (upward+left arrow) */
#define KEY_BACKSPACE	0407		/* backspace (unreliable) */
#define KEY_F0		0410		/* Function keys.  Space for 64 */
#define KEY_F(n)	(KEY_F0+(n))	/* keys is reserved. */
#define KEY_DL		0510		/* Delete line */
#define KEY_IL		0511		/* Insert line */
#define KEY_DC		0512		/* Delete character */
#define KEY_IC		0513		/* Insert char or enter insert mode */
#define KEY_EIC		0514		/* Exit insert char mode */
#define KEY_CLEAR	0515		/* Clear screen */
#define KEY_EOS		0516		/* Clear to end of screen */
#define KEY_EOL		0517		/* Clear to end of line */
#define KEY_SF		0520		/* Scroll 1 line forward */
#define KEY_SR		0521		/* Scroll 1 line backwards (reverse) */
#define KEY_NPAGE	0522		/* Next page */
#define KEY_PPAGE	0523		/* Previous page */
#define KEY_STAB	0524		/* Set tab */
#define KEY_CTAB	0525		/* Clear tab */
#define KEY_CATAB	0526		/* Clear all tabs */
#define KEY_ENTER	0527		/* Enter or send (unreliable) */
#define KEY_SRESET	0530		/* soft (partial) reset (unreliable) */
#define KEY_RESET	0531		/* reset or hard reset (unreliable) */
#define KEY_PRINT	0532		/* print or copy */
#define KEY_LL		0533		/* home down or bottom (lower left) */
					/* The keypad is arranged like this: */
					/*    a1    up    a3   */
					/*   left   b2  right  */
					/*    c1   down   c3   */
#define KEY_A1		0534		/* upper left of keypad */
#define KEY_A3		0535		/* upper right of keypad */
#define KEY_B2		0536		/* center of keypad */
#define KEY_C1		0537		/* lower left of keypad */
#define KEY_C3		0540		/* lower right of keypad */

# endif NOMACROS
#endif WINDOW
