#ifndef _TW_SETUP_H
#define _TW_SETUP_H

byte InitBuiltin(void);
void UpdateListWin(void);

extern menu *Builtin_Menu;
extern window *ListWin;

#if 0
void InitEmpty(void);
void InitTwEdit(void);
void InitClock(void);
#endif

#endif /* _TW_SETUP_H */
