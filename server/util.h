/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_UTIL_H
#define TWIN_UTIL_H

#ifndef TW_TWAUTOCONF_H
#include <Tw/autoconf.h> /* for TW_HAVE_ALARM */
#endif

#ifdef TW_HAVE_SYS_TIMEB_H
#include <sys/timeb.h>
#endif
#ifdef TW_HAVE_SYS_UN_H
#include <sys/un.h>
#endif

#include "stl_types.h"

extern uldat unixSlot;
extern int unixFd;

extern String TmpDir;
extern String SocketDir;

#if defined(TW_HAVE_ALARM) && defined(TW_HAVE_SIGACTION)
extern volatile int AlarmReceived;
void SetAlarm(unsigned seconds) NOTHROW;
#else
#define SetAlarm(seconds) ((void)0)
#define AlarmReceived (0)
#endif

udat CopyToSockaddrUn(const char *src, struct sockaddr_un *addr, udat pos);

void NormalizeTime(timevalue *Time);
timevalue *InstantNow(timevalue *Now);
dat CmpTime(timevalue *T1, timevalue *T2);
timevalue *SumTime(timevalue *Result, timevalue *Time, timevalue *Incr);
timevalue *SubTime(timevalue *Result, timevalue *Time, timevalue *Decr);
timevalue *IncrTime(timevalue *Time, timevalue *Incr);
timevalue *DecrTime(timevalue *Time, timevalue *Decr);
void SortMsgPortByCallTime(Tmsgport Port);
void SortAllMsgPortsByCallTime(void);
byte SendControlMsg(Tmsgport MsgPort, udat Code, udat Len, const char *Data);

byte Minimum(byte MaxIndex, const ldat *Array);

uldat ComputeUsableLenArgv(char *const *argv);
void SetArgv0(char *const *argv, uldat argv_usable_len, const char *src);

bool SetSelectionFromWindow(Twindow Window);
void doSelectionSetOwner(Tobj Owner, tany Time, tany Frac);
#define SEL_CURRENTTIME ((tany)0)

byte CreateMouseEvent(event_mouse *Event, byte buflen, char *buf);

/* close all fds except tty_fd_to_dup: duplicate it on fds 0, 1 and 2 */
void closeAllFds(int tty_fd_to_dup);

void ResetBorderPattern(void);
void FallBackKeyAction(Twindow window, event_keyboard *EventK);

char **TokenizeStringVec(uldat len, char *text);
char **TokenizeTRuneVec(uldat len, trune *text);
void FreeStringVec(char **argv);

bool InitGlobalVariables(void);
bool InitTWDisplay(void);
void QuitTWDisplay(void);

byte CheckPrivileges(void);
void gainPrivileges(void);
byte SetServerUid(uldat uid, byte privileges);

/*
 * search for a file relative to $XDG_CONFIG_HOME/twin, to $HOME/.config/twin,
 * to global variable confdir or to current working directory
 *
 * this for example will search "foo" as:
 *   getenv("XDG_CONFIG_HOME") + "/twin/foo"
 *   getenv("HOME") + "/.config/twin/foo"
 *   confdir + "/foo"
 *   "./foo"
 *
 * returned path must be freed with FreeMem()
 */
char *FindConfigFile(const char *name, uldat *file_size);
void RunTwEnvRC(void);

/* remove const from a pointer and suppress compiler warnings */
void *RemoveConst(const void *x);

#endif /* TWIN_UTIL_H */
