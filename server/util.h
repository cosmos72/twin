#ifndef _TW_UTIL_H
#define _TW_UTIL_H

extern udat ErrNo;
extern byte *ErrStr;

byte AssignId(fn_obj *Fn_Obj, obj *Obj);
void DropId(obj *Obj);
obj *Id2Obj(byte i, uldat Id);
#define Obj2Id(obj) ((obj) ? (obj)->Id : NOID)

byte **StrDupList(uldat n, byte **list);

void Error(udat Code_Error);
void NormalizeTime(timevalue *Time);
timevalue *InstantNow(timevalue *Now);
dat CmpTime(timevalue *T1, timevalue *T2);
timevalue *SumTime(timevalue *Result, timevalue *Time, timevalue *Incr);
timevalue *SubTime(timevalue *Result, timevalue *Time, timevalue *Decr);
timevalue *IncrTime(timevalue *Time, timevalue *Incr);
timevalue *DecrTime(timevalue *Time, timevalue *Decr);
void SortMsgPortByCallTime(msgport *Port);
void SortAllMsgPortsByCallTime(void);
byte Minimum(byte MaxIndex, uldat *Elenco);

void SetArgv_0(byte **argv, byte *src);

#define AddToClipBoard(Len, Data) SetClipBoard(CLIP_APPEND, Len, Data)
byte SetClipBoard(uldat Magic, uldat Len, byte *Data);
byte SetClipBoardFromWindow(window *Window);

byte   SetTWDisplay(void);
void UnSetTWDisplay(void);

byte SearchInfo(menu *Menu, dat i);

#endif /* _TW_UTIL_H */

