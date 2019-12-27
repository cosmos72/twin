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

byte TtyWriteAscii(window Window, uldat Len, CONST char *AsciiSeq);
byte TtyWriteString(window Window, uldat Len, CONST char *String);
byte TtyWriteHWFont(window Window, uldat Len, CONST hwfont *HWFont);
byte TtyWriteHWAttr(window Window, dat x, dat y, uldat Len, CONST hwattr *Attr);

widget TtyKbdFocus(widget Window);
void ForceKbdFocus(void);

#endif /* _TWIN_TTY_H */
