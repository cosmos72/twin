
#ifndef TWIN_HW_TTY_TERMCAP_H
#define TWIN_HW_TTY_TERMCAP_H

/* this is the usual mess of ad-hoc includes and defines... */

#if defined(__sun__) || defined(__SVR4)

/*  curses interfaces (emulated) to the termcap library (Solaris7, bwitt) */
#undef ttrue
#undef tfalse
#include <curses.h>
#include <termio.h> /* Solaris7: needed "typedef struct sgttyb SGTTY" */
#include <term.h>

#ifdef __sun__
/*  Someone forgot a few prototypes!  ugh  (Solaris7, bwitt) */
extern char *tgoto(const char *cap, int col, int row);
extern char *tgetstr(char *id, char **area);
#endif

#else /* rest of the world */

#ifdef TW_HAVE_TERMCAP_H
#include <termcap.h>
#else
#ifdef TW_HAVE_NCURSES_TERMCAP_H
#include <ncurses/termcap.h>
#else
#ifdef TW_HAVE_NCURSES_H
#include <ncurses.h>
#else
#ifdef TW_HAVE_NCURSES_NCURSES_H
#include <ncurses/ncurses.h>
#endif
#endif
#endif
#endif

#endif /* rest of the world */

/*
 * termcap names for special keys:
 *
 * kb : backspace
 * k1 : f1 function key
 * k2 : f2 function key
 * k3 : f3 function key
 * k4 : f4 function key
 * k5 : f5 function key
 * k6 : f6 function key
 * k7 : f7 function key
 * k8 : f8 function key
 * k9 : f9 function key
 * k; : f10 function key
 * F1 : f11 function key
 * F2 : f12 function key
 *
 * &7 : Suspend key
 * kh : Home key
 * @7 : End key
 * kD : Delete (Remove) key
 * kI : Insert key
 * kN : Next Page key
 * kP : Prev Page key
 * kl : Left Arrow key
 * ku : Up Arrow key
 * kr : Right Arrow key
 * kd : Down Arrow key
 */
struct tc_init_node {
  byte *cap, **buf;
};

enum {
  tc_seq_first = 0,

  tc_seq_scr_clear = tc_seq_first,
  tc_seq_cursor_goto,
  tc_seq_cursor_on,
  tc_seq_cursor_off,
  tc_seq_bold_on,
  tc_seq_blink_on,
  tc_seq_attr_off,
  tc_seq_kpad_on,
  tc_seq_kpad_off,
  tc_seq_audio_bell,
  tc_seq_charset_start,
  tc_seq_charset_end,

  tc_seq_last,
  tc_key_first = tc_seq_last,

  tc_key_FN = tc_key_first,
  tc_key_Suspend = tc_key_FN + 12,
  tc_key_Home,
  tc_key_End,
  tc_key_Delete,
  tc_key_Insert,
  tc_key_Page_Down,
  tc_key_Page_Up,
  tc_key_Left,
  tc_key_Up,
  tc_key_Right,
  tc_key_Down,

  tc_key_last,
  tc_seq_N = tc_key_last
};

#endif /* TWIN_HW_TTY_TERMCAP_H */
