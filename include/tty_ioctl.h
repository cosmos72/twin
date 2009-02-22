/*  Copyright (C) 2000 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */
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
