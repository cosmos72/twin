/*  Copyright (C) 2000 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */
#ifndef TWIN_TTY_IOCTL_H
#define TWIN_TTY_IOCTL_H

#ifndef TW_TWAUTOCONF_H
#include <Tw/autoconf.h>
#endif

#if defined(TW_HAVE_TERMIOS_H)
#include <termios.h>
#elif defined(TW_HAVE_TERMIO_H)
#include <termio.h>
#endif

#ifdef TW_HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#if defined(TCSETS)
#define tty_setioctl(tty_fd, ttyb) ioctl((tty_fd), TCSETS, (ttyb))
#define tty_getioctl(tty_fd, ttyb) ioctl((tty_fd), TCGETS, (ttyb))
#elif defined(TCSANOW)
#define tty_setioctl(tty_fd, ttyb) tcsetattr((tty_fd), TCSANOW, (ttyb))
#define tty_getioctl(tty_fd, ttyb) tcgetattr((tty_fd), (ttyb))
#else
#error TCSETS and TCSANOW both undefined.
#endif

#ifndef VDISABLE
#ifdef _POSIX_VDISABLE
#define VDISABLE _POSIX_VDISABLE
#else
#define VDISABLE 255
#endif
#endif

enum {
  TW_TTY_IFLAG_ON = BRKINT | IGNPAR | ICRNL | IXON
#ifdef ITW_MAXBEL
                    | ITW_MAXBEL
#endif
  ,
  TW_TTY_IFLAG_OFF = IGNBRK | ISTRIP | INLCR | IXANY | IXOFF
#ifdef PARMRK
                     | PARMRK
#endif
#ifdef INPCK
                     | INPCK
#endif
#ifdef IUCLC
                     | IUCLC
#endif
  ,
  TW_TTY_OFLAG_ON = OPOST | ONLCR,
  TW_TTY_OFLAG_OFF = 0
#ifdef OLCUC
                     | OLCUC
#endif
#ifdef ONOCR
                     | ONOCR
#endif
#ifdef ONLRET
                     | ONLRET
#endif
#ifdef OFILL
                     | OFILL
#endif
#ifdef OFDEL
                     | OFDEL
#endif
#ifdef NLDLY
                     | NLDLY
#endif
#ifdef CRDLY
                     | CRDLY
#endif
#ifdef TABDLY
                     | TABDLY
#endif
#ifdef BSDLY
                     | BSDLY
#endif
#ifdef VTDLY
                     | VTDLY
#endif
#ifdef FFDLY
                     | FFDLY
#endif
  ,
  TW_TTY_CFLAG_ON = CS8 | CREAD,
  TW_TTY_CFLAG_OFF = CSTOPB | PARENB | PARODD
#ifdef HUPCL
                     | HUPCL
#endif
#ifdef CLOCAL
                     | CLOCAL
#endif
#ifdef CMSPAR
                     | CMSPAR
#endif
#ifdef CRTSCTS
                     | CRTSCTS
#endif
  ,
  TW_TTY_CFLAG_SPEED =
#if defined(B38400)
      B38400
#elif defined(B19200)
      B19200
#else
      B9600
#endif
  ,
  TW_TTY_LFLAG_ON = ISIG | ICANON | IEXTEN | ECHO | ECHOE | ECHOK
#ifdef ECHOKE
                    | ECHOKE
#endif
  ,
  TW_TTY_LFLAG_OFF = ECHONL
#ifdef XCASE
                     | XCASE
#endif
#ifdef NOFLSH
                     | NOFLSH
#endif
#ifdef TOSTOP
                     | TOSTOP
#endif
#ifdef ECHOCTL
                     | ECHOCTL
#endif
#ifdef ECHOPRT
                     | ECHOPRT
#endif
#ifdef FLUSHO
                     | FLUSHO
#endif
#ifdef PENDIN
                     | PENDIN
#endif
  ,
};

#endif /* TWIN_TTY_IOCTL_H */
