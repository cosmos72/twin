#ifndef _TW_TTY_H
#define _TW_TTY_H

void TtyWriteAscii(window Window, ldat Len, CONST byte *AsciiSeq);
window TtyKbdFocus(window Window);
void ForceKbdFocus(void);

void TtyWriteHWAttr(window Window, dat x, dat y, ldat Len, CONST hwattr *Attr);

#if 0
byte WriteContents(window Window, hwattr *text, ldat len);
#endif

#endif /* _TW_TTY_H */
