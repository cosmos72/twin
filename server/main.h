#ifndef _TW_MAIN_H
#define _TW_MAIN_H

extern fd_set save_rfds, save_wfds;
extern int max_fds;
extern byte *TWDisplay, lenTWDisplay, *origTWDisplay;
extern byte **main_argv;

void Quit(int status);

void NoOp(void);
byte AlwaysTrue(void);
byte AlwaysFalse(void);


#endif /* _TW_MAIN_H */

