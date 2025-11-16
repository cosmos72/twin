
#ifndef TWIN_HW_TTY_TERMCAP_H
#define TWIN_HW_TTY_TERMCAP_H

#ifdef CONF_HW_TTY_TERMCAP

/* this is the usual mess of ad-hoc includes and defines... */

#if defined(__sun__) || defined(__SVR4)

/*  curses interfaces (emulated) to the termcap library (Solaris7, bwitt) */
#undef true
#undef false
#include <curses.h>
#include <termio.h> /* Solaris7: needed "typedef struct sgttyb SGTTY" */
#include <term.h>

#ifdef __sun__
/*  Someone forgot a few prototypes!  ugh  (Solaris7, bwitt) */
extern char *tgoto(const char *cap, int col, int row);
extern char *tgetstr(char *id, char **area);
#endif

#else /* rest of the world */

#if defined(TW_HAVE_TERMCAP_H)
#include <termcap.h>
#elif defined(TW_HAVE_NCURSES_TERMCAP_H)
#include <ncurses/termcap.h>
#elif defined(TW_HAVE_NCURSES_H)
#include <ncurses.h>
#elif defined(TW_HAVE_NCURSES_NCURSES_H)
#include <ncurses/ncurses.h>
#endif

#endif /* rest of the world */

#endif /* CONF_HW_TTY_TERMCAP */

#endif /* TWIN_HW_TTY_TERMCAP_H */
