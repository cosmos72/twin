#ifndef _TW_PTY_H
#define _TW_PTY_H

byte SpawnInWindow(window Window, CONST byte *arg0, byte * CONST *argv);

gid_t get_tty_grgid(void);

#endif /* _TW_PTY_H */
