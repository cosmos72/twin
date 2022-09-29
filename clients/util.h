/*
 *  Copyright (C) 2022  Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */

#ifndef TWIN_CLIENT_UTIL_H
#define TWIN_CLIENT_UTIL_H

/* close all fds except tty_fd_to_dup: duplicate it on fds 0, 1 and 2 */
void closeAllFds(int tty_fd_to_dup);

/* Open /dev/null and dup() it to fds 0, 1 and 2. return != 0 if successful */
unsigned char openDevNull(void);

void ignoreSigHup(void);

#endif /* TWIN_CLIENT_UTIL_H */
