#ifndef _TW_TTY_H
#define _TW_TTY_H

void TtyWriteAscii(window Window, ldat Len, CONST byte *AsciiSeq);
void TtyWriteString(window Window, ldat Len, CONST byte *String);
void TtyWriteHWFont(window Window, ldat Len, CONST hwfont *HWFont);

widget TtyKbdFocus(widget Window);
void ForceKbdFocus(void);

void TtyWriteHWAttr(window Window, dat x, dat y, ldat Len, CONST hwattr *Attr);

#endif /* _TW_TTY_H */
