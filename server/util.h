#ifndef _TW_UTIL_H
#define _TW_UTIL_H

extern udat ErrNo;
extern byte CONST * ErrStr;
extern uldat unixSlot;
extern int unixFd;

void *CloneMem(CONST void *From, uldat Size);
byte *CloneStr(CONST byte *s);
byte *CloneStrL(CONST byte *s, uldat len);
byte **CloneStrList(byte **list);

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

void SetArgv_0(byte * CONST * argv, CONST byte * src);

#define SelectionAppend(Len, Data) SelectionStore(SEL_APPEND, NULL, Len, Data)
byte SelectionStore(uldat Magic, CONST byte MIME[MAX_MIMELEN], uldat Len, CONST byte *Data);
byte SetSelectionFromWindow(window Window);
void doSelectionSetOwner(obj Owner, time_t Time, frac_t Frac);
#define SEL_CURRENTTIME ((time_t)0)

byte CreateXTermMouseEvent(event_mouse *Event, byte buflen, byte *buf);

void ResetBorderPattern(void);
void FallBackKeyAction(window W, event_keyboard *EventK);

byte **TokenizeStringVec(uldat len, byte *text);
void FreeStringVec(byte **argv);

byte InitTWDisplay(void);
void QuitTWDisplay(void);

void CheckPrivileges(void);
void GetPrivileges(void);

byte AssignId(CONST fn_obj Fn_Obj, obj Obj);
void DropId(obj Obj);
obj  Id2Obj(byte i, uldat Id);
#define Obj2Id(o) ((o) ? (o)->Id : NOID)

#endif /* _TW_UTIL_H */

