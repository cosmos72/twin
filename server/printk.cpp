/*
 *  printk.c -- report messages from twin core.
 *
 *  Copyright (C) 2001 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "twin.h"
#include "printk.h"
#include "remote.h"
#include "methods.h"
#include "builtin.h"
#include "draw.h"

char printk_buf[TW_BIGBUFF];
static int printk_fd = NOFD;

void printk_str(const char *s, size_t len) {
  ssize_t chunk;

  if (len == 0) {
    return;
  }

#ifdef CONF_PRINTK
  if (MessagesWin) {
    if (MessagesWin->HLogic > TW_SMALLBUFF) {
      while (MessagesWin->HLogic > TW_SMALLBUFF) {
        MessagesWin->USE.R.Rows.First->Delete();
        MessagesWin->CurY--;
      }
      if (MessagesWin->Parent) {
        DrawFullWindow(MessagesWin);
      }
    }
    MessagesWin->RowWriteCharset(len, s);
  }
#endif /* CONF_PRINTK */

  if (printk_fd == NOFD) {
    (void)fwrite(s, len, 1, stderr);
    return;
  }
  // write error message to printk_fd
  do {
    do {
      chunk = write(printk_fd, s, len);
    } while (chunk < 0 && errno == EINTR);
    if (chunk > 0) {
      s += chunk;
      len -= chunk;
    }
  } while (len && chunk > 0);
}

int flushk(void) {
  return printk_fd == NOFD ? fflush(stderr) : ttrue;
}

int printk_receive_fd(int fd) {
  int got = 0, len;

  do {
    do {
      len = read(fd, printk_buf, sizeof(printk_buf));
    } while (len < 0 && errno == EINTR);
    if (len > 0) {
      got += len;
      printk_str(printk_buf, len);
    }
  } while (len > 0);

  return got;
}

bool RegisterPrintkFd(int fd) {
  if (printk_fd == NOFD) {
    printk_fd = fd;
    return true;
  }
  return false;
}

void UnRegisterPrintkFd(void) {
  printk_fd = NOFD;
}
