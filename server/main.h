/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_MAIN_H
#define TWIN_MAIN_H

class String;

extern fd_set save_rfds, save_wfds;
extern int max_fds;
extern byte lenTWDisplay;
extern char *TWDisplay, *origTWDisplay, *origTERM, *origCOLORTERM;
extern char **main_argv, **orig_argv;
extern uldat main_argv_usable_len;
extern byte flag_envrc, flag_secure;
extern const Chars flag_secure_msg;

extern String HOME;

extern int (*OverrideSelect)(int n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
                             struct timeval *timeout);

void Quit(int status);

void NoOp(void);
bool AlwaysTrue(void);
bool AlwaysFalse(void);
void *AlwaysNull(void);

#endif /* TWIN_MAIN_H */
