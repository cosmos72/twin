/*
 *  util.c  --  utility functions
 *
 *  Copyright (C) 1993-2000 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include "twin.h"
#include "methods.h"
#include "main.h"
#include "data.h"
#include "util.h"
#include "hw.h"

udat ErrNo;
byte *ErrStr;

#if 0
uldat MemLeft(void) {
    /* TODO? */
    return 0;
}
#endif

void Error(udat Code_Error) {
    if ((ErrNo = Code_Error) == NOMEMORY) {
#if 0
	BeepHW();
#endif	
	ErrStr = "Out of memory!";
    }
}

byte *CloneStr(byte *s) {
    byte *q;
    uldat len;
    
    if (s) {
	len = 1 + LenStr(s);
	if ((q = AllocMem(len)))
	    CopyMem(s, q, len);
	return q;
    }
    return s;
}

byte *CloneStrL(byte *s, uldat len) {
    byte *q;
    
    if (s && len) {
	if ((q = AllocMem(len+1))) {
	    CopyMem(s, q, len);
	    q[len] = '\0';
	}
	return q;
    }
    return s;
}

byte **CloneStrList(byte **s) {
    uldat n = 1;
    byte **t = s, **v;
    
    if (t) {
	while (*t) {
	    t++;
	    n++;
	}
	t = AllocMem(n * sizeof(byte *));
    }
    
    if ((v = t)) {
	for (; *s && n; v++, s++, n--) {
	    if (!(*v = CloneStr(*s)))
		break;
	}

	if (*s && n) {
	    /* failed... clean up */
	    for (; t < v; t++)
		FreeMem(*t);
	    t = NULL;
	} else
	    *v = NULL;
    }
    return t;
}

void NormalizeTime(timevalue *Time) {
    while (Time->Fraction >= 1 FullSECs) {
	Time->Seconds++;
	Time->Fraction -= 1 FullSECs;
    }
    while (Time->Fraction < 0) {
	Time->Seconds--;
	Time->Fraction += 1 FullSECs;
    }
}

timevalue *InstantNow(timevalue *Now) {
    struct timeval sysNow;
    
    gettimeofday(&sysNow, NULL);
    Now->Seconds = sysNow.tv_sec;
    Now->Fraction = sysNow.tv_usec MicroSECs;
    return Now;
}

timevalue *IncrTime(timevalue *Time, timevalue *Incr) {
    Time->Seconds+=Incr->Seconds;
    Time->Fraction+=Incr->Fraction;
    NormalizeTime(Time);
    return Time;
}

timevalue *DecrTime(timevalue *Time, timevalue *Decr) {
    Time->Seconds-=Decr->Seconds;
    Time->Fraction-=Decr->Fraction;
    NormalizeTime(Time);
    return Time;
}

timevalue *SumTime(timevalue *Result, timevalue *Time, timevalue *Incr) {
    CopyMem(Time, Result, sizeof(timevalue));
    return IncrTime(Result, Incr);
}

timevalue *SubTime(timevalue *Result, timevalue *Time, timevalue *Decr) {
    CopyMem(Time, Result, sizeof(timevalue));
    return DecrTime(Result, Decr);
}

dat CmpTime(timevalue *T1, timevalue *T2) {
    NormalizeTime(T1);
    NormalizeTime(T2);
    
    if (T1->Seconds>T2->Seconds)
	return (dat)1;
    else if (T1->Seconds==T2->Seconds) {
	if (T1->Fraction>T2->Fraction)
	    return (dat)1;
	else if (T1->Fraction==T2->Fraction)
	    return (dat)0;
    }
    
    return (dat)-1;
}

static dat CmpCallTime(msgport *m1, msgport *m2) {
    if ((!m1->FirstMsg) != (!m2->FirstMsg))
	/* one of the two doesn't have msgs */
	return m1->FirstMsg ? (dat)-1 : (dat)1;
    if ((!m1->WakeUp) != (!m2->WakeUp))
	/* one doesn't need to be called */
	return m1->WakeUp ? (dat)-1 : (dat)1;
    if ((!m1->WakeUp) && (!m2->WakeUp))
	return 0;
    return CmpTime(&m1->CallTime, &m2->CallTime);
}

byte Minimum(byte MaxIndex, uldat *Elenco) {
    byte i, MinIndex;
    uldat Temp;
    
    Temp=MAXULDAT;
    MinIndex=(byte)0;
    if (!MaxIndex)
	return 0xFF;
    
    for(i=0; i<MaxIndex; i++)
	if (Elenco[i]<Temp) {
	    Temp = Elenco[i];
	    MinIndex = i;
	}
    
    return MinIndex;
}

/*
byte HexDigitToNum(byte HexDigit, byte *Error) {
     if (HexDigit>='0' && HexDigit<='9')
	 return HexDigit - (byte)'0';
     
     if (HexDigit>='A' && HexDigit<='F')
	 return (byte)10 + HexDigit - (byte)'A';
     
     if (HexDigit>='a' && HexDigit<='f')
	 return (byte)10 + HexDigit - (byte)'a';
     
     *Error = TRUE;
     return (byte)0;
}

uldat HexStrToNum(byte *StringHex) {
     byte Len, Error = FALSE;
     uldat Value=(uldat)0;
     
     Len=(byte)LenStr(StringHex);
     if (Len>(byte)8)
	 return (uldat)0;
     
     do {
	 Len--;
	 Value |= (uldat)HexDigitToNum(StringHex[Len], &Error) << (Len << 2);
     }	while (!Error && Len>(byte)0);
    
    return Error ? (uldat)0 : Value;
}
*/

/* adapted from similar code in bdflush */
void SetArgv_0(byte **argv, byte *src) {
    byte *ptr;
    uldat count, len = strlen(src);
    
    ptr = argv[0] + strlen(argv[0]);
    for (count = 1; argv[count]; count++) {
	if (argv[count] == ptr + 1)
	    ptr += strlen(++ptr);
    }
    
    if (len + 1 < (size_t)(ptr - argv[0])) {
	memcpy(argv[0], src, len);
	memset(argv[0] + len, 0, (size_t)(ptr - argv[0]) - len);
    } else
	memcpy(argv[0], src, (size_t)(ptr - argv[0]));
}
                                           


/*
 * move a msgport to the right place in an already sorted list,
 * ordering by CallTime
 */
void SortMsgPortByCallTime(msgport *Port) {
    msgport *other;
    if ((other = Port->Next) && CmpCallTime(Port, other) > 0) {
	Remove(Port);
	do {
	    other = other->Next;
	} while (other && CmpCallTime(Port, other) > 0);
	if (other)
	    InsertMiddle(MsgPort, Port, All, other->Prev, other);
	else
	    InsertLast(MsgPort, Port, All);
    } else if ((other = Port->Prev) && CmpCallTime(Port, other) < 0) {
	Remove(Port);
	do {
	    other = other->Prev;
	} while (other && CmpCallTime(Port, other) < 0);
	if (other)
	    InsertMiddle(MsgPort, Port, All, other, other->Next);
	else
	    InsertFirst(MsgPort, Port, All);
    }
}

/*
 * sort the msgport list by growing CallTime
 * 
 * we use a bubble sort... no need to optimize to death this
 */
void SortAllMsgPortsByCallTime(void) {
    msgport *Max, *This, *Port = All->FirstMsgPort;
    msgport *Start, *End;
    
    Start = End = (msgport *)0;
    
    while (Port) {
	Max = This = Port;
	while ((This = This->Next)) {
	    if (CmpCallTime(This, Max) > 0)
		Max = This;
	}
	if (Max == Port)
	    /* careful, we are mucking the list under our feet */
	    Port = Port->Next;
	
	Remove(Max);
	/*
	 * HACK : we create a parentless list 
	 * backward, from End adding ->Prev until Start
	 */
	Max->Next = Start;
	Max->All = All;
	if (Start) Start->Prev = Max;
	Start = Max;
	if (!End) End = Max;
    }
    All->FirstMsgPort = Start;
    All->LastMsgPort  = End;
}

byte SelectionStore(uldat Magic, byte MIME[MAX_MIMELEN], uldat Len, byte *Data) {
    uldat newLen;
    byte *newData;
    selection *Sel = All->Selection;
    
    if (Magic == SEL_APPEND) 
	newLen = Sel->Len + Len;
    else
	newLen = Len;
    
    if (Sel->Max < newLen) {
	if (!(newData = ReAllocMem(Sel->Data, newLen)))
	    return FALSE;
	Sel->Data = newData;
	Sel->Max = newLen;
    }
    if (Magic != SEL_APPEND) {
	Sel->Owner = NULL;
	Sel->Len = 0;
	Sel->Magic = Magic;
	if (MIME)
	    CopyMem(MIME, Sel->MIME, MAX_MIMELEN);
	else
	    WriteMem(Sel->MIME, '\0', MAX_MIMELEN);
    }
    if (Data)
	CopyMem(Data, Sel->Data + Sel->Len, Len);
    else
	WriteMem(Sel->Data + Sel->Len, ' ', Len);
    Sel->Len += Len;
    return TRUE;
}

byte SetSelectionFromWindow(window *Window) {
    uldat y, slen, len;
    byte *sData, *Data, ok = TRUE;
    
    if (!(Window->Attrib & WINDOW_ANYSEL) || Window->YstSel > Window->YendSel ||
	(Window->YstSel == Window->YendSel && Window->XstSel > Window->XendSel)) {
	
	ok &= SelectionStore(SEL_TEXTMAGIC, NULL, 0, NULL);
	if (ok) NeedHW |= NEEDSelectionExport;
	return ok;
    }
    
    if (Window->Flags & WINFL_USECONTENTS) {
	hwattr *hw;
	
	if (!(sData = AllocMem(slen = Window->NumRowOne)))
	    return FALSE;
	
	hw = Window->Contents + (Window->YstSel + Window->NumRowSplit) * slen;
	while (hw >= Window->TtyData->Split)
	    hw -= Window->TtyData->Split - Window->Contents;
	
	{
	    y = Window->YstSel;
	    if (y < Window->YendSel)
		slen -= Window->XstSel;
	    else
		slen = Window->XendSel - Window->XstSel + 1;
	    Data = sData;
	    len = slen;
	    hw += Window->XstSel;
	    while (len--)
		*Data++ = HWFONT(*hw++);
	    ok &= SelectionStore(SEL_TEXTMAGIC, NULL, slen, sData);
	}
	
	if (hw >= Window->TtyData->Split)
	    hw -= Window->TtyData->Split - Window->Contents;

	slen = Window->NumRowOne;
	for (y = Window->YstSel + 1; ok && y < Window->YendSel; y++) {
	    if (hw >= Window->TtyData->Split)
		hw -= Window->TtyData->Split - Window->Contents;
	    Data = sData;
	    len = slen;
	    while (len--)
		*Data++ = HWFONT(*hw++);
	    ok &= SelectionAppend(slen, sData);
	}
	
	if (ok && Window->YendSel > Window->YstSel) {
	    if (hw >= Window->TtyData->Split)
		hw -= Window->TtyData->Split - Window->Contents;
	    Data = sData;
	    len = slen = Window->XendSel + 1;
	    while (len--)
		*Data++ = HWFONT(*hw++);
	    ok &= SelectionAppend(slen, sData);
	}
	if (ok) NeedHW |= NEEDSelectionExport;
	return ok;
    }
    if (!(Window->Flags & WINFL_USEANY)) {
	row *Row;
	
	/* Gap not supported! */
	y = Window->YstSel;
	Row = Act(SearchRow,Window)(Window, y);
	
	if (Row) {
	    if (y < Window->YendSel)
		slen = Row->Len - Window->XstSel;
	    else
		slen = Min2(Row->Len, Window->XendSel+1) - Min2(Row->Len, Window->XstSel);
	    ok &= SelectionStore(SEL_TEXTMAGIC, NULL, slen, Row->Text + Window->XstSel);
	}
	if (y < Window->YendSel || !Row || Row->Len <= Window->XendSel)
	    ok &= SelectionAppend(1, "\n");

	for (y = Window->YstSel + 1; ok && y < Window->YendSel; y++) {
	    if ((Row = Act(SearchRow,Window)(Window, y)))
		ok &= SelectionAppend(Row->Len, Row->Text);
	    ok &= SelectionAppend(1, "\n");
	}
	if (Window->YendSel > Window->YstSel) {
	    if ((Row = Act(SearchRow,Window)(Window, Window->YendSel)))
		ok &= SelectionAppend(Min2(Row->Len, Window->XendSel+1), Row->Text);
	    if (!Row || Row->Len <= Window->XendSel)
		ok &= SelectionAppend(1, "\n");
	}
	if (ok) NeedHW |= NEEDSelectionExport;
	return ok;
    }
    return FALSE;
}





static byte fullTWD[]="/tmp/.Twin:\0\0\0";

/* set TWDISPLAY */
byte SetTWDisplay(void) {
    int i, fd;
    byte *arg0;
    
    for (i=0; i<0x1000; i++) {
	sprintf(fullTWD+11, "%x", i);
	if ((fd = open(fullTWD, O_WRONLY|O_CREAT|O_TRUNC|O_EXCL, 0600)) >= 0) {
	    close(fd);
	    TWDisplay = fullTWD+10;
	    lenTWDisplay = strlen(TWDisplay);
	    
	    setenv("TWDISPLAY", TWDisplay, 1);
	    setenv("TERM", "linux", 1);
	    if ((arg0 = AllocMem(strlen(TWDisplay) + 6))) {
		strcpy(arg0, "twin ");
		strcat(arg0, TWDisplay);
		SetArgv_0(main_argv, arg0);
		FreeMem(arg0);
	    }
	    return TRUE;
	}
    }
    return FALSE;
}

/* unlink /tmp/.Twin$TWDISPLAY */
void UnSetTWDisplay(void) {
    unlink(fullTWD);
}





/* finally, functions to manage Ids */

static obj **IdList[magic_n];
static uldat IdSize[magic_n], IdTop[magic_n], IdBottom[magic_n];

INLINE uldat IdListGrow(byte i) {
    uldat oldsize, size;
    obj **newIdList;
    
    oldsize = IdSize[i];
    if (oldsize >= MAXID)
	return NOSLOT;
    
    size = oldsize < 64 ? 96 : oldsize + (oldsize>>1);
    if (size > MAXID)
	size = MAXID;
    
    /* use realloc(), not ReAllocMem() here */
    if (!(newIdList = (obj **)realloc(IdList[i], size*sizeof(obj *))))
	return NOSLOT;
    
    WriteMem(newIdList+oldsize, 0, (size-oldsize)*sizeof(obj *));
    
    IdList[i] = newIdList;
    IdSize[i] = size;
    
    return oldsize;
}

INLINE uldat IdListGet(byte i) {
    if (IdBottom[i] == IdSize[i])
	return IdListGrow(i);
    
    return IdBottom[i];
}

INLINE byte _AssignId(byte i, obj *Obj) {
    uldat Id, j;
    if ((Id = IdListGet(i)) != NOSLOT) {
	Obj->Id = Id | ((uldat)i << magic_shift);
	IdList[i][Id] = Obj;
	if (IdTop[i] <= Id)
	    IdTop[i] = Id + 1;
	for (j = IdBottom[i] + 1; j < IdTop[i]; j++)
	    if (!IdList[i][j])
		break;
	IdBottom[i] = j;
    }
    return Id != NOSLOT;
}

INLINE void _DropId(byte i, obj *Obj) {
    uldat Id = Obj->Id & MAXID, j;

    if (Id < IdTop[i] && IdList[i][Id] == Obj /* paranoia */) {
	Obj->Id = NOID;
	IdList[i][Id] = (obj *)0;
	if (IdBottom[i] > Id)
	    IdBottom[i] = Id;
	for (j = IdTop[i] - 1; j > IdBottom[i]; j--)
	    if (IdList[i][j])
		break;
	IdTop[i] = (j == IdBottom[i]) ? j : j + 1;
    }
}
    
byte AssignId(fn_obj *Fn_Obj, obj *Obj) {
    byte i = 0;
    if (Obj) switch (Fn_Obj->Magic) {
      case area_magic:
      case area_win_magic:
      case gadget_magic:	
      case window_magic:
      case menuitem_magic:
      case menu_magic:
      case screen_magic:
      case msgport_magic:
      case mutex_magic:
      case display_hw_magic:
	i = Fn_Obj->Magic >> magic_shift;
	break;
      case msg_magic:
      case row_magic:
      case module_magic:
	/* we don't use Ids for rows and msgs as we expect to create *lots* of them */
	/* it's unsafe to allow modules access remotely, so no Ids for them too */
        Obj->Id = Fn_Obj->Magic;
	return TRUE;
      case obj_magic:
	/* 'obj' is just a template type, you can't create one */
      default:
	return FALSE;
    }
    return i ? _AssignId(i, Obj) : i;
}

void DropId(obj *Obj) {
    byte i = 0;
    if (Obj && Obj->Fn) switch (Obj->Fn->Magic) {
      case area_magic:
      case area_win_magic:
      case gadget_magic:	
      case window_magic:
      case menuitem_magic:
      case menu_magic:
      case screen_magic:
      case msgport_magic:
      case mutex_magic:
	i = Obj->Fn->Magic >> magic_shift;
	break;
      case msg_magic:
      case row_magic:
      case module_magic:
	/* we don't use Ids for rows and msgs as we expect to create *lots* of them */
	/* it's unsafe to allow modules access remotely, so no Ids for them too */
	Obj->Id = NOID;
	return;
      case obj_magic:
	/* 'obj' is just a template type, you can't create one */
      default:
	return;
    }
    if (i && i == (Obj->Id >> magic_shift))
	_DropId(i, Obj);
}

obj *Id2Obj(byte i, uldat Id) {
    if (i < magic_n) {
	if (i == (obj_magic >> magic_shift))
	    i = (Id >> magic_shift);
	if (i == (Id >> magic_shift)) {
	    Id &= MAXID;
	    if (Id < IdTop[i])
		return IdList[i][Id];
	}
    }
    return (obj *)0;
}
