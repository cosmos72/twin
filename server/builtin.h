/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_BUILTIN_H
#define TWIN_BUILTIN_H

#include "obj/fwd.h"
#include <Tw/datatypes.h>

bool InitBuiltin(void);
void FillButtonWin(void);
void UpdateOptionWin(void);

const tcolor *ColorFill(tcolor *array, uldat len, tcolor fill);

extern Twindow WinList, MessagesWin;
extern Tmsgport Builtin_MsgPort;

#endif /* TWIN_BUILTIN_H */
