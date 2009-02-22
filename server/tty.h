/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef _TWIN_TTY_H
#define _TWIN_TTY_H

void TtyWriteAscii(window Window, ldat Len, CONST byte *AsciiSeq);
void TtyWriteString(window Window, ldat Len, CONST byte *String);
void TtyWriteHWFont(window Window, ldat Len, CONST hwfont *HWFont);

widget TtyKbdFocus(widget Window);
void ForceKbdFocus(void);

void TtyWriteHWAttr(window Window, dat x, dat y, ldat Len, CONST hwattr *Attr);

#endif /* _TWIN_TTY_H */
