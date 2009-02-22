/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef _TWIN_MAIN_H
#define _TWIN_MAIN_H

extern fd_set save_rfds, save_wfds;
extern int max_fds;
extern byte lenTWDisplay, *TWDisplay, *origTWDisplay, *origTERM, *origHW, *HOME;
extern byte **main_argv, **orig_argv;
extern uldat main_argv_usable_len;
extern byte flag_envrc, flag_secure, *flag_secure_msg;

extern int (*OverrideSelect)(int n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

void Quit(int status);

void NoOp(void);
byte AlwaysTrue(void);
byte AlwaysFalse(void);
void *AlwaysNull(void);


#endif /* _TWIN_MAIN_H */

