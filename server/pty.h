/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_PTY_H
#define TWIN_PTY_H

byte spawnInWindow(window Window, const char *arg0, const char *const *argv);

gid_t getTtyGrgid(void);

#endif /* TWIN_PTY_H */
