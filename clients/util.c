/*
 *  Copyright (C) 1993-2022  Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */

#include "util.h"

#include <Tw/autoconf.h> /* for TW_HAVE_* macros */

#ifdef TW_HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef TW_HAVE_LIMITS_H
#include <limits.h> /* INT_MAX */
#endif
#ifdef TW_HAVE_SIGNAL_H
#include <signal.h>
#endif
#ifdef TW_HAVE_UNISTD_H
#include <unistd.h>
#endif

/* close all fds except tty_fd_to_dup: duplicate it on fds 0, 1 and 2 */
void closeAllFds(int tty_fd_to_dup) {
  // do not assume all other file descriptors are set to fcntl(fd, F_SETFD, FD_CLOEXEC)
  int i, last_successful_close;
  if (tty_fd_to_dup >= 0) {
    for (i = 0; i <= 2; i++) {
      if (i != tty_fd_to_dup) {
        close(i);
        dup2(tty_fd_to_dup, i);
      }
    }
    if (tty_fd_to_dup > 2) {
      close(tty_fd_to_dup);
    }
  } else {
    i = 0;
  }
  last_successful_close = i - 1;
  for (; i < INT_MAX && i - 10 < last_successful_close; i++) {
    if (close(i) >= 0) {
      last_successful_close = i;
    }
  }
}

/* Open /dev/null and dup() it to fds 0, 1 and 2. return != 0 if successful */
unsigned char openDevNull(void) {
  int i, fd = open("/dev/null", O_RDWR);
  if (fd < 0) {
    return 0;
  }
  for (i = 0; i <= 2; i++) {
    if (i != fd) {
      close(i);
      dup2(fd, i);
    }
  }
  if (fd > 2) {
    close(fd);
  }
  return 1;
}

void ignoreSigHup(void) {
  (void)signal(SIGHUP, SIG_IGN);
}
