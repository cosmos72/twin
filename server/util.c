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
#include <grp.h>
#include <errno.h>

#include "twin.h"
#include "data.h"
#include "methods.h"
#include "main.h"
#include "draw.h"
#include "resize.h"
#include "util.h"

#include "hw.h"

#include "libTwkeys.h"

udat ErrNo;
CONST byte *ErrStr;

#if 0
uldat MemLeft(void) {
    /* TODO? */
    return 0;
}
#endif

void Error(udat Code_Error) {
    switch ((ErrNo = Code_Error)) {
      case NOMEMORY:
	ErrStr = "Out of memory!";
	break;
      case SYSCALLERROR:
	ErrStr = strerror(errno);
	break;
      default:
	break;
    }
}

void *CloneMem(CONST void *From, uldat Size) {
    void *temp;
    if (From && Size && (temp = AllocMem(Size)))
	return CopyMem(From, temp, Size);
    return NULL;
}

byte *CloneStr(CONST byte *s) {
    byte *q;
    uldat len;
    
    if (s) {
	len = 1 + LenStr(s);
	if ((q = AllocMem(len)))
	    CopyMem(s, q, len);
	return q;
    }
    return NULL;
}

byte *CloneStrL(CONST byte *s, uldat len) {
    byte *q;
    
    if (s) {
	if ((q = AllocMem(len+1))) {
	    if (len)
		CopyMem(s, q, len);
	    q[len] = '\0';
	}
	return q;
    }
    return NULL;
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

byte Minimum(byte MaxIndex, CONST uldat *Array) {
    byte i, MinIndex;
    uldat Temp;
    
    Temp=MAXULDAT;
    MinIndex=(byte)0;
    if (!MaxIndex)
	return 0xFF;
    
    for(i=0; i<MaxIndex; i++)
	if (Array[i]<Temp) {
	    Temp = Array[i];
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
void SetArgv_0(byte **argv, CONST byte *src) {
    byte *ptr;
    uldat count, len = LenStr(src);
    
    ptr = argv[0] + LenStr(argv[0]);
    for (count = 1; argv[count]; count++) {
	if (argv[count] == ptr + 1)
	    ptr += LenStr(++ptr);
    }
    
    if (len + 1 < (size_t)(ptr - argv[0])) {
	CopyMem(src, argv[0], len);
	WriteMem(argv[0] + len, '\0', (size_t)(ptr - argv[0]) - len);
    } else
	CopyMem(src, argv[0], (size_t)(ptr - argv[0]));
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


byte SendControlMsg(msgport *MsgPort, udat Code, udat Len, CONST byte *Data) {
    msg *Msg;
    event_control *Event;
    
    if (MsgPort && (Msg = Do(Create,Msg)(FnMsg, MSG_CONTROL, sizeof(event_control) + Len))) {
	Event = &Msg->Event.EventControl;
	Event->Code = Code;
	Event->Len  = Len;
	CopyMem(Data, Event->Data, Len);
	SendMsg(MsgPort, Msg);
	
	return TRUE;
    }
    return FALSE;
}

byte SelectionStore(uldat Magic, CONST byte MIME[MAX_MIMELEN], uldat Len, CONST byte *Data) {
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
	    if (Row->Text) {
		if (y < Window->YendSel)
		    slen = Row->Len - Window->XstSel;
		else
		    slen = Min2(Row->Len, Window->XendSel+1) - Min2(Row->Len, Window->XstSel);
		ok &= SelectionStore(SEL_TEXTMAGIC, NULL, slen, Row->Text + Window->XstSel);
	    } else
		ok &= SelectionStore(SEL_TEXTMAGIC, NULL, 0, "");
	}
	if (y < Window->YendSel || !Row || !Row->Text || Row->Len <= Window->XendSel)
	    ok &= SelectionAppend(1, "\n");

	for (y = Window->YstSel + 1; ok && y < Window->YendSel; y++) {
	    if ((Row = Act(SearchRow,Window)(Window, y)) && Row->Text)
		ok &= SelectionAppend(Row->Len, Row->Text);
	    ok &= SelectionAppend(1, "\n");
	}
	if (Window->YendSel > Window->YstSel) {
	    if ((Row = Act(SearchRow,Window)(Window, Window->YendSel)) && Row->Text)
		ok &= SelectionAppend(Min2(Row->Len, Window->XendSel+1), Row->Text);
	    if (!Row || !Row->Text || Row->Len <= Window->XendSel)
		ok &= SelectionAppend(1, "\n");
	}
	if (ok) NeedHW |= NEEDSelectionExport;
	return ok;
    }
    return FALSE;
}


void ResetBorderPattern(void) {
    msgport *MsgP;
    menu *Menu;
    window *W;
    
    for (MsgP = All->FirstMsgPort; MsgP; MsgP=MsgP->Next) {
	for (Menu = MsgP->FirstMenu; Menu; Menu=Menu->Next) {
	    for (W = Menu->FirstWindow; W; W=W->ONext) {
		W->BorderPattern[0] = W->BorderPattern[1] = NULL;
	    }
	}
    }
}

void FallBackKeyAction(window *W, event_keyboard *EventK) {
    uldat NumRow, OldNumRow;
	
    switch (EventK->Code) {
      case TW_Up:
	if (!W->MaxNumRow)
	    break;
	OldNumRow=W->CurY;
	if (OldNumRow<MAXULDAT) {
	    if (!OldNumRow)
		NumRow=W->MaxNumRow-(uldat)1;
	    else
		NumRow=OldNumRow-(uldat)1;
	    W->CurY=NumRow;
	    if (W->Flags & WINFL_SEL_ROWCURR)
		DrawTextWindow(W, (uldat)0, OldNumRow, (uldat)MAXUDAT-(uldat)2, OldNumRow);
	}
	else
	    W->CurY=NumRow=W->MaxNumRow-(uldat)1;
	if (W->Flags & WINFL_SEL_ROWCURR)
	    DrawTextWindow(W, (uldat)0, NumRow, (uldat)MAXUDAT-(uldat)2, NumRow);
	UpdateCursor();
	break;
      case TW_Down:
	if (!W->MaxNumRow)
	    break;
	OldNumRow=W->CurY;
	if (OldNumRow<MAXULDAT) {
	    if (OldNumRow>=W->MaxNumRow-(uldat)1)
		NumRow=(uldat)0;
	    else
		NumRow=OldNumRow+(uldat)1;
	    W->CurY=NumRow;
	    if (W->Flags & WINFL_SEL_ROWCURR)
		DrawTextWindow(W, (uldat)0, OldNumRow, (uldat)MAXUDAT-(uldat)2, OldNumRow);
	}
	else
	    W->CurY=NumRow=(uldat)0;
	if (W->Flags & WINFL_SEL_ROWCURR)
	    DrawTextWindow(W, (uldat)0, NumRow, (uldat)MAXUDAT-(uldat)2, NumRow);
	UpdateCursor();
	break;
      case TW_Left:
	if (W->CurX) {
	    W->CurX--;
	    UpdateCursor();
	}
	break;
      case TW_Right:
	if (((W->Flags & WINFL_USECONTENTS) && W->CurX < W->XWidth - 3) ||
	    (!(W->Flags & WINFL_USEANY) && W->CurX < MAXULDAT - 1)) {
	    W->CurX++;
	    UpdateCursor();
	}
	break;
      default:
	break;
    }
}




static byte fullTWD[]="/tmp/.Twin:\0\0\0";
static byte envTWD[]="TWDISPLAY=\0\0\0\0";

/* set TWDISPLAY */
byte SetTWDisplay(void) {
    int i, fd;
    byte *arg0;

    HOME = getenv("HOME");
    
    for (i=0; i<0x1000; i++) {
	sprintf(fullTWD+11, "%x", i);
	if ((fd = open(fullTWD, O_WRONLY|O_CREAT|O_TRUNC|O_EXCL, 0600)) >= 0) {
	    close(fd);
	    TWDisplay = fullTWD+10;
	    lenTWDisplay = LenStr(TWDisplay);
	    
	    CopyMem(TWDisplay, envTWD+10, lenTWDisplay);
	    putenv(envTWD);
	    putenv("TERM=linux");
	    if ((arg0 = AllocMem(LenStr(TWDisplay) + 6))) {
		strcpy(arg0, "twin ");
		strcat(arg0, TWDisplay);
		SetArgv_0(main_argv, arg0);
		FreeMem(arg0);
	    }
	    return TRUE;
	}
    }
    fprintf(stderr, "twin: all TWDISPLAY already in use!\n"
	    "      Please cleanup stale /tmp/.Twin* sockets and try again\n");
    return FALSE;
}

/* unlink /tmp/.Twin$TWDISPLAY */
void UnSetTWDisplay(void) {
    unlink(fullTWD);
}



/* suid/sgid privileges related functions */

static enum { none, sgidtty, suidroot } Privilege = none;
static gid_t tty_grgid;

gid_t get_tty_grgid(void) {
    struct group *gr;
    
    if (!tty_grgid) {
	if ((gr = getgrnam("tty")))
	    tty_grgid = gr->gr_gid;
	else
	    tty_grgid = (gid_t)-1;
    }
    return tty_grgid;
}

void CheckPrivileges(void) {
    if (GetRootPrivileges() >= 0)
	Privilege = suidroot;
    else {
	(void)get_tty_grgid();
	if (tty_grgid != (gid_t)-1 && GetGroupPrivileges(tty_grgid) >= 0)
	    Privilege = sgidtty;
    }
    
    DropPrivileges();
    
#if 0
# ifdef CONF_TERM
#  ifdef CONF_TERM_DEVPTS
    if (Privilege < sgidtty) {
	byte c;
	fprintf(stderr, "twin: not running setgid tty.\n"
		"      might be unable to start the terminal emulator.\n"
		"      hit RETURN to continue, or CTRL-C to quit.\n");
	fflush(stdout);
	read(0, &c, 1);
    }
#  else /* !CONF_TERM_DEVPTS */
    if (Privilege < suidroot) {
	byte c;
	fprintf(stderr, "twin: not running setuid root.\n"
		"      might be unable to start the terminal emulator.\n"
		"      hit RETURN to continue, or CTRL-C to quit.\n");
	fflush(stdout);
	read(0, &c, 1);
    }
#  endif /* CONF_TERM_DEVPTS */
    
# else /* !CONF_TERM */

#  ifdef CONF_MODULES
    if (Privilege == none) {
	byte c;
	fprintf(stderr, "twin: not running setgid tty or setuid root.\n"
		"      might be unable to start the terminal emulator module.\n"
		"      hit RETURN to continue, or CTRL-C to quit.\n");
	fflush(stdout);
	read(0, &c, 1);
    }
#  endif /* CONF_MODULES */
# endif /* CONF_TERM */
#endif /* 0 */
}

void GetPrivileges(void) {
    if (Privilege == suidroot)
	GetRootPrivileges();
    else if (Privilege == sgidtty)
	GetGroupPrivileges(get_tty_grgid());
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
    
byte AssignId(CONST fn_obj *Fn_Obj, obj *Obj) {
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
