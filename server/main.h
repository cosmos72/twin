#ifndef _TW_MAIN_H
#define _TW_MAIN_H

extern fd_set save_rfds, save_wfds;
extern int max_fds;
extern byte lenTWDisplay, *TWDisplay, *origTWDisplay, *origTERM, *origHW;
extern byte **main_argv, **orig_argv;

extern msgport *WM_MsgPort;

extern int (*OverrideSelect)(int n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

void Quit(int status);

void NoOp(void);
byte AlwaysTrue(void);
byte AlwaysFalse(void);


#endif /* _TW_MAIN_H */

