
/*
 * Linux console, linux terminal and twterm terminal drivers:
 * 
 * raw keyboard tty input;
 * (as fallback, plain Unix-style tty keyboard input)
 * 
 * mouse input uses libgpm to connect to `gpm' mouse daemon
 * and read mouse state, but draws mouse pointer manually;
 * (as fallback, xterm-style mouse input)
 * 
 * output through /dev/vcsaXX;
 * (as fallback, output through stdout)
 *
 * CONF_HW_TTY_LINUX is for linux console support (gpm, /dev/vcsa*), while
 * CONF_HW_TTY_TWTERM is for twin terminal (xterm style mouse, stdout)
 */


#ifdef CONF_HW_TTY_LINUX
# include "mouse_gpm.h"
#endif /* CONF_HW_TTY_LINUX */



#if defined (CONF_HW_TTY_LINUX) || defined(CONF_HW_TTY_TWTERM)
# include "video_stdout.h"
#endif /* defined(CONF_HW_TTY_LINUX) || defined(CONF_HW_TTY_TWTERM) */



#ifdef CONF_HW_TTY_LINUX
# include "video_vcsa.h"
#endif /* CONF_HW_TTY_LINUX */



#if defined (CONF_HW_TTY_LINUX) && defined (CONF_HW_TTY_LRAWKBD)
# include "kbd_raw.h"
#endif /* defined(CONF_HW_TTY_LINUX) && defined(CONF_HW_TTY_LRAWKBD) */




