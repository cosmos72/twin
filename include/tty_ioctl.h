#ifndef _TW_TTY_IOCTL_H
#define _TW_TTY_IOCTL_H

#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>


#ifdef TCSETS
# define tty_setioctl(tty_fd, ttyb) ioctl(tty_fd, TCSETS, ttyb)
# define tty_getioctl(tty_fd, ttyb) ioctl(tty_fd, TCGETS, ttyb)
#else
# ifdef TCSANOW
#  define tty_setioctl(tty_fd, ttyb) tcsetattr(tty_fd, TCSANOW, ttyb)
#  define tty_getioctl(tty_fd, ttyb) tcgetattr(tty_fd, ttyb)
# else
#  error TCSETS and TCSANOW both undefined.
# endif
#endif

#endif /* _TW_TTY_IOCTL_H */
