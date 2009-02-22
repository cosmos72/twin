/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef _TWIN_BUILTIN_H
#define _TWIN_BUILTIN_H

byte InitBuiltin(void);
void FillButtonWin(void);
void UpdateOptionWin(void);

extern window WinList, MessagesWin;
extern msgport Builtin_MsgPort;

#endif /* _TWIN_BUILTIN_H */
