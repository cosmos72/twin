#ifndef _TW_TTY_H
#define _TW_TTY_H

void TtyWriteAscii(window *Window, uldat Len, CONST byte *AsciiSeq);
window *TtyKbdFocus(window *Window);
void ForceKbdFocus(void);

void TtyWriteHWAttr(window *Window, udat x, udat y, uldat Len, CONST hwattr *Attr);

#if 0
byte WriteContents(window *Window, hwattr *text, uldat len);
#endif

#endif /* _TW_TTY_H */
