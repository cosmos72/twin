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

extern udat ErrNo;
extern byte CONST * ErrStr;
extern uldat unixSlot;
extern int unixFd;

void *CloneMem(CONST void *From, uldat Size);
byte *CloneStr(CONST byte *From);
byte *CloneStrL(CONST byte *From, uldat Size);
byte **CloneStrList(byte **FromList);
hwfont *CloneStr2HWFont(CONST byte *From, uldat Size);

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
byte SendControlMsg(msgport MsgPort, udat Code, udat Len, CONST byte *Data);

byte Minimum(byte MaxIndex, CONST uldat *Array);

uldat ComputeUsableLenArgv(byte *CONST *argv);
void SetArgv0(byte *CONST *argv, uldat argv_usable_len, CONST byte *src);


#define SelectionAppend(Len, Data) SelectionStore(SEL_APPEND, NULL, Len, Data)
byte SelectionStore(uldat Magic, CONST byte MIME[MAX_MIMELEN], uldat Len, CONST byte *Data);
byte SetSelectionFromWindow(window Window);
void doSelectionSetOwner(obj Owner, tany Time, tany Frac);
#define SEL_CURRENTTIME ((tany)0)

byte CreateXTermMouseEvent(event_mouse *Event, byte buflen, byte *buf);

void ResetBorderPattern(void);
void FallBackKeyAction(window W, event_keyboard *EventK);

byte **TokenizeStringVec(uldat len, byte *text);
byte **TokenizeHWFontVec(uldat len, hwfont *text);
void FreeStringVec(byte **argv);

byte InitTWDisplay(void);
void QuitTWDisplay(void);

extern uid_t Uid, EUid;
byte CheckPrivileges(void);
void GainPrivileges(void);
byte SetServerUid(uldat uid, byte privileges);

byte *FindFile(byte *name, uldat *fsize);
void RunTwEnvRC(void);

hwattr EncodeToHWAttrExtra(byte pos, byte detail, byte active, byte pressed);

byte AssignId(CONST fn_obj Fn_Obj, obj Obj);
byte AssignId_all(all Obj);
void DropId(obj Obj);
obj  Id2Obj(byte i, uldat Id);
#define Obj2Id(o) ((o) ? (o)->Id : NOID)

#endif /* _TWIN_UTIL_H */

