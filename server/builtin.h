#ifndef _TWIN_BUILTIN_H
#define _TWIN_BUILTIN_H

byte InitBuiltin(void);
void FillButtonWin(void);
void UpdateOptionWin(void);

extern window WinList, MessagesWin;
extern msgport Builtin_MsgPort;

#endif /* _TWIN_BUILTIN_H */
