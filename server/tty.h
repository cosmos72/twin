#ifndef _TW_TTY_H
#define _TW_TTY_H

void WriteAscii(window *Window, uldat Len, byte *AsciiSeq);
window *KbdFocus(window *Window);
void ForceKbdFocus(void);

void WriteHWAttr(window *Window, udat x, udat y, uldat Len, hwattr *Attr);

#if 0
byte WriteContents(window *Window, hwattr *text, uldat len);
#endif

#endif /* _TW_TTY_H */
