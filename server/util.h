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

#ifndef TWAUTOCONF_H
#include "twautoconf.h" /* for TW_HAVE_ALARM */
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

#if defined(TW_HAVE_ALARM) && defined(TW_HAVE_SIGACTION)
extern volatile int AlarmReceived;
void SetAlarm(unsigned seconds);
#else
#define SetAlarm(seconds) ((void)0)
#define AlarmReceived (0)
#endif

const char *TmpDir(void);
udat CopyToSockaddrUn(const char *src, struct sockaddr_un *addr, udat pos);

void NormalizeTime(timevalue *Time);
timevalue *InstantNow(timevalue *Now);
dat CmpTime(timevalue *T1, timevalue *T2);
timevalue *SumTime(timevalue *Result, timevalue *Time, timevalue *Incr);
timevalue *SubTime(timevalue *Result, timevalue *Time, timevalue *Decr);
timevalue *IncrTime(timevalue *Time, timevalue *Incr);
timevalue *DecrTime(timevalue *Time, timevalue *Decr);
void SortMsgPortByCallTime(msgport Port);
void SortAllMsgPortsByCallTime(void);
byte SendControlMsg(msgport MsgPort, udat Code, udat Len, const char *Data);

byte Minimum(byte MaxIndex, const ldat *Array);

uldat ComputeUsableLenArgv(char *const *argv);
void SetArgv0(char *const *argv, uldat argv_usable_len, const char *src);

bool SetSelectionFromWindow(window Window);
void doSelectionSetOwner(obj Owner, tany Time, tany Frac);
#define SEL_CURRENTTIME ((tany)0)

byte CreateXTermMouseEvent(event_mouse *Event, byte buflen, char *buf);

void ResetBorderPattern(void);
void FallBackKeyAction(window W, event_keyboard *EventK);

char **TokenizeStringVec(uldat len, char *text);
char **TokenizeTRuneVec(uldat len, trune *text);
void FreeStringVec(char **argv);

byte InitTWDisplay(void);
void QuitTWDisplay(void);

byte CheckPrivileges(void);
void GainPrivileges(void);
byte SetServerUid(uldat uid, byte privileges);

char *FindFile(const char *name, uldat *fsize);
void RunTwEnvRC(void);

/* remove const from a pointer and suppress compiler warnings */
void *RemoveConst(const void *x);

#endif /* TWIN_UTIL_H */
