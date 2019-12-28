/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef _TWIN_UTIL_H
#define _TWIN_UTIL_H

#ifndef _TWAUTOCONF_H
#include "twautoconf.h" /* for TW_HAVE_ALARM */
#endif

#ifdef TW_HAVE_SYS_TIMEB_H
#include <sys/timeb.h>
#endif
#ifdef TW_HAVE_SYS_UN_H
#include <sys/un.h>
#endif

extern udat ErrNo;
extern CONST char *ErrStr;
extern uldat unixSlot;
extern int unixFd;

#if defined(TW_HAVE_ALARM) && defined(TW_HAVE_SIGACTION)
extern volatile int AlarmReceived;
void SetAlarm(unsigned seconds);
#else
#define SetAlarm(seconds) ((void)0)
#define AlarmReceived (0)
#endif

void *CloneMem(CONST void *From, uldat Size);
char *CloneStr(CONST char *From);
char *CloneStrL(CONST char *From, uldat Size);
char **CloneStrList(char **FromList);
hwfont *CloneStr2HWFont(CONST char *From, uldat Size);

CONST char *TmpDir(void);
udat CopyToSockaddrUn(CONST char *src, struct sockaddr_un *addr, udat pos);

byte Error(udat Code_Error);

void NormalizeTime(timevalue *Time);
timevalue *InstantNow(timevalue *Now);
dat CmpTime(timevalue *T1, timevalue *T2);
timevalue *SumTime(timevalue *Result, timevalue *Time, timevalue *Incr);
timevalue *SubTime(timevalue *Result, timevalue *Time, timevalue *Decr);
timevalue *IncrTime(timevalue *Time, timevalue *Incr);
timevalue *DecrTime(timevalue *Time, timevalue *Decr);
void SortMsgPortByCallTime(msgport Port);
void SortAllMsgPortsByCallTime(void);
byte SendControlMsg(msgport MsgPort, udat Code, udat Len, CONST char *Data);

byte Minimum(byte MaxIndex, CONST ldat *Array);

uldat ComputeUsableLenArgv(char *CONST *argv);
void SetArgv0(char *CONST *argv, uldat argv_usable_len, CONST char *src);

#define SelectionAppend(Len, Data) SelectionStore(SEL_APPEND, NULL, Len, Data)
byte SelectionStore(uldat Magic, CONST char MIME[MAX_MIMELEN], uldat Len, CONST char *Data);
byte SetSelectionFromWindow(window Window);
void doSelectionSetOwner(obj Owner, tany Time, tany Frac);
#define SEL_CURRENTTIME ((tany)0)

byte CreateXTermMouseEvent(event_mouse *Event, byte buflen, char *buf);

void ResetBorderPattern(void);
void FallBackKeyAction(window W, event_keyboard *EventK);

char **TokenizeStringVec(uldat len, char *text);
char **TokenizeHWFontVec(uldat len, hwfont *text);
void FreeStringVec(char **argv);

byte InitTWDisplay(void);
void QuitTWDisplay(void);

byte CheckPrivileges(void);
void GainPrivileges(void);
byte SetServerUid(uldat uid, byte privileges);

char *FindFile(CONST char *name, uldat *fsize);
void RunTwEnvRC(void);

/* remove CONST from a pointer and suppress compiler warnings */
void *RemoveConst(CONST void *x);

hwattr EncodeToHWAttrExtra(tpos pos, tternary detail, tbool active, tbool pressed);

#endif /* _TWIN_UTIL_H */
