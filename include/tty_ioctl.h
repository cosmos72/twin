#ifndef _TWIN_TTY_IOCTL_H
#define _TWIN_TTY_IOCTL_H

#ifndef RETSIGTYPE
# include "autoconf.h"
#endif

#if defined(HAVE_TERMIOS_H)
# include <termios.h>
#elif defined(HAVE_TERMIO_H)
# include <termio.h>
#endif

#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif

#ifdef TCSETS
# define tty_setioctl(tty_fd, ttyb) ioctl((tty_fd), TCSETS, (ttyb))
# define tty_getioctl(tty_fd, ttyb) ioctl((tty_fd), TCGETS, (ttyb))
#else
# ifdef TCSANOW
#  define tty_setioctl(tty_fd, ttyb) tcsetattr((tty_fd), TCSANOW, (ttyb))
#  define tty_getioctl(tty_fd, ttyb) tcgetattr((tty_fd), (ttyb))
# else
#  error TCSETS and TCSANOW both undefined.
# endif
#endif

#endif /* _TWIN_TTY_IOCTL_H */
