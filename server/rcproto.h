/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#ifndef TWIN_RCPROTO_H
#define TWIN_RCPROTO_H

/* stuff exported by rcparse.h */

extern ldat GlobalFlags[4];
extern ldat GlobalShadows[2];

bool rcload(Tdisplay);

#endif /* TWIN_RCPROTO_H */
