
#ifndef _TWIN_HW_TTY_TERMCAP_H
#define _TWIN_HW_TTY_TERMCAP_H

/* this is the usual mess of ad-hoc includes and defines... */

# if defined (__sun__) || defined(__SVR4)

   /*  curses interfaces (emulated) to the termcap library (Solaris7, bwitt) */
#  undef TRUE
#  undef FALSE
#  include <curses.h>
#  include <termio.h>         /* Solaris7: needed "typedef struct sgttyb SGTTY" */
#  include <term.h>

#  ifdef __sun__
   /*  Some one forget a few prototypes!  ugh  (Solaris7, bwitt) */
   extern char * tgoto(), * tgetstr();
#  endif

# else /* rest of the world */

#  ifdef HAVE_TERMCAP_H
#   include <termcap.h>
#  else
#   ifdef HAVE_NCURSES_TERMCAP_H
#    include <ncurses/termcap.h>
#   else
#    ifdef HAVE_NCURSES_H
#     include <ncurses.h>
#    else
#     ifdef HAVE_NCURSES_NCURSES_H
#      include <ncurses/ncurses.h>
#     endif
#    endif
#   endif
#  endif

# endif /* rest of the world */

static void termcap_QuitVideo(void);
static void termcap_ShowMouse(void);
static void termcap_HideMouse(void);
static void termcap_FlushVideo(void);
static void termcap_UpdateMouseAndCursor(void);


static void termcap_Beep(void);
static void termcap_Configure(udat resource, byte todefault, udat value);
static void termcap_ConfigureKeyboard(udat resource, byte todefault, udat value);

static byte termcap_CanDragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp);
static void termcap_DragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp);


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
 * [...]
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
    tc_seq_first,
	
	tc_seq_scr_clear = tc_seq_first, tc_seq_cursor_goto,
	tc_seq_cursor_on, tc_seq_cursor_off,
	tc_seq_bold_on, tc_seq_blink_on, tc_seq_attr_off,
	tc_seq_kpad_on, tc_seq_kpad_off, tc_seq_audio_bell,
	tc_seq_charset_start, tc_seq_charset_end,

	tc_seq_last, tc_key_first = tc_seq_last,
	
	tc_key_FN = tc_key_first,
	tc_key_Delete = tc_key_FN + 12, tc_key_Insert,
	tc_key_Page_Down, tc_key_Page_Up,
	tc_key_Left, tc_key_Up, tc_key_Right, tc_key_Down,

	tc_key_last, tc_cap_N = tc_key_last
};

#endif /* _TWIN_HW_TTY_TERMCAP_H */

