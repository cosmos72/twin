/*  Copyright (C) 1993-1999  Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#ifndef TW_PTY_H
#define TW_PTY_H

int Spawn(twindow Window, pid_t *pid, dat X, dat Y, const char *arg0, const char *const *argv);

#endif /* TW_PTY_H */
