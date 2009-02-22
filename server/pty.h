/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef _TWIN_PTY_H
#define _TWIN_PTY_H

byte SpawnInWindow(window Window, CONST byte *arg0, byte * CONST *argv);

gid_t get_tty_grgid(void);

#endif /* _TWIN_PTY_H */
