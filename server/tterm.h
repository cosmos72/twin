/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_TTERM_H
#define TWIN_TTERM_H

bool InitTerm(void);

Twindow OpenTerm(const char *arg0, const char *const *argv);

extern Tmenu Term_Menu;

#endif /* TWIN_TTERM_H */
