/*
 *  util.c  --  utility functions
 *
 *  Copyright (C) 1993-2001 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "twin.h"

#include <sys/socket.h>  
#include <sys/stat.h>
#include <sys/un.h>
#include <grp.h>
#include <pwd.h>

#ifdef HAVE_SYS_TIMEB_H
# include <sys/timeb.h>
#endif

#include "data.h"
#include "extreg.h"
#include "methods.h"
#include "main.h"
#include "draw.h"
#include "remote.h"
#include "resize.h"
#include "printk.h"
#include "util.h"

#include "hw.h"

#include <Tw/Twkeys.h>

#ifdef CONF__UNICODE
# include <Tutf/Tutf.h>
#endif

udat ErrNo;
byte CONST * ErrStr;

#if 0
uldat MemLeft(void) {
    /* TODO? */
    return 0;
}
#endif

byte Error(udat Code_Error) {
    switch ((ErrNo = Code_Error)) {
      case NOMEMORY:
	ErrStr = "Out of memory!";
	break;
      case NOTABLES:
	ErrStr = "Internal tables full!";
	break;
      case SYSCALLERROR:
	ErrStr = strerror(errno);
	break;
      default:
	break;
    }
    return FALSE;
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

#if TW_SIZEOFHWFONT == 1
hwfont *CloneStr2HWFont(CONST byte *s, uldat len) {
    return CloneStrL(s, len);
}
#else
hwfont *CloneStr2HWFont(CONST byte *s, uldat len) {
    hwfont *temp, *save;
    
    if (s) {
	if ((temp = save = (hwfont *)AllocMem((len+1) * sizeof(hwfont)))) {
	    while (len--) {
# ifdef CONF__UNICODE
		*temp++ = Tutf_CP437_to_UTF_16[*s++];
# else
		*temp++ = *s++;
# endif
	    }
	    *temp = '\0';
	}
	return save;
    }
    return NULL;
}
#endif

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
#if defined(HAVE_GETTIMEOFDAY)
    struct timeval sysNow;

    gettimeofday(&sysNow, NULL);
    
    Now->Seconds = sysNow.tv_sec;
    Now->Fraction = sysNow.tv_usec MicroSECs;
#elif defined(HAVE_FTIME)
    timeb sysNow;

    ftime(&sysNow);
    
    Now->Seconds = sysNow.time ;
    Now->Fraction = sysNow.millitm  MilliSECs;
#endif
    
    return Now;
}

timevalue *IncrTime(timevalue *Time, timevalue *Incr) {
    Time->Seconds+=Incr->Seconds;
    Time->Fraction+=Incr->Fraction;
    NormalizeTime(Time);
    return Time;
}

timevalue *DecrTime(timevalue *Time, timevalue *Decr) {
    Time->Seconds -= Decr->Seconds;
    if (Time->Fraction >= Decr->Fraction)
        Time->Fraction -= Decr->Fraction;
    else {
        Time->Seconds--;
        Time->Fraction += (1 FullSECs - Decr->Fraction);
    }
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

static dat CmpCallTime(msgport m1, msgport m2) {
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
uldat ComputeUsableLenArgv(byte *CONST *argv) {
    byte *ptr;
    uldat count;
    
    ptr = argv[0] + LenStr(argv[0]);
    for (count = 1; argv[count]; count++) {
	if (argv[count] == ptr + 1)
	    ptr += LenStr(ptr + 1) + 1;
    }
    return ptr - argv[0];
}

void SetArgv0(byte *CONST *argv, uldat argv_usable_len, CONST byte *src) {
    uldat len = strlen(src);
    
    if (len + 1 < argv_usable_len) {
	CopyMem(src, argv[0], len);
	WriteMem(argv[0] + len, '\0', argv_usable_len - len);
    } else
	CopyMem(src, argv[0], argv_usable_len);
}
                                           


/*
 * move a msgport to the right place in an already sorted list,
 * ordering by CallTime
 */
void SortMsgPortByCallTime(msgport Port) {
    msgport other;
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
    msgport Max, This, Port = All->FirstMsgPort;
    msgport Start, End;
    
    Start = End = (msgport)0;
    
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


byte SendControlMsg(msgport MsgPort, udat Code, udat Len, CONST byte *Data) {
    msg Msg;
    event_control *Event;
    
    if (MsgPort && (Msg = Do(Create,Msg)(FnMsg, MSG_CONTROL, Len))) {
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
    byte *newData, pad;
    selection *Sel = All->Selection;
    
    if (Magic == SEL_APPEND) 
	newLen = Sel->Len + Len;
    else
	newLen = Len;
    
    if ((pad = (Sel->Magic == SEL_HWFONTMAGIC && (newLen & 1))))
	newLen++;
    
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
    if (pad) {
#if TW_BYTE_ORDER == TW_LITTLE_ENDIAN
	Sel->Data[Sel->Len++] = '\0';
#else
	Sel->Data[Sel->Len] = Sel->Data[Sel->Len-1];
	Sel->Data[Sel->Len-1] = '\0';
	Sel->Len++;
#endif
    }
    return TRUE;
}

#ifdef CONF__UNICODE
# define _SEL_MAGIC SEL_HWFONTMAGIC
# if TW_BYTE_ORDER == TW_LITTLE_ENDIAN
#  define _SelAppendNL() SelectionAppend(2, "\n\0");
# else
#  define _SelAppendNL() SelectionAppend(2, "\0\n");
# endif
#else
# define _SEL_MAGIC SEL_TEXTMAGIC
# define _SelAppendNL() SelectionAppend(1, "\n");
#endif

byte SetSelectionFromWindow(window Window) {
    uldat y, slen, len;
    hwfont *sData, *Data;
    byte ok = TRUE, w_useC = W_USE(Window, USECONTENTS);
    
    if (!(Window->State & WINDOW_DO_SEL))
	return ok;
    
    if (!(Window->State & WINDOW_ANYSEL) || Window->YstSel > Window->YendSel ||
	(Window->YstSel == Window->YendSel && Window->XstSel > Window->XendSel)) {
	
	ok &= SelectionStore(_SEL_MAGIC, NULL, 0, NULL);
	if (ok) NeedHW |= NEEDSelectionExport;
	return ok;
    }

    /* normalize negative coords */
    if (Window->XstSel < 0)
	Window->XstSel = 0;
    else if (w_useC && Window->XstSel >= Window->WLogic) {
	Window->XstSel = 0;
	Window->YstSel++;
    }
    if (Window->YstSel < Window->YLogic) {
	Window->YstSel = Window->YLogic;
	Window->XstSel = 0;
    } else if (Window->YstSel >= Window->HLogic) {
	Window->YstSel = Window->HLogic - 1;
	Window->XstSel = w_useC ? Window->WLogic - 1 : MAXLDAT;
    }

    if (w_useC) {
	hwattr *hw;
	
	/* normalize negative coords */
        if (Window->XendSel < 0) {
	    Window->XendSel = Window->WLogic - 1;
	    Window->YendSel--;
	} else if (Window->XendSel >= Window->WLogic)
	    Window->XendSel = Window->WLogic - 1;
	    
	if (Window->YendSel < Window->YLogic) {
	    Window->YendSel = Window->YLogic;
	    Window->XendSel = 0;
	} else if (Window->YendSel >= Window->HLogic) {
	    Window->YendSel = Window->HLogic - 1;
	    Window->XendSel = Window->WLogic - 1;
	}
	
	if (!(sData = (hwfont *)AllocMem(sizeof(hwfont) * (slen = Window->WLogic))))
	    return FALSE;
	
	
	hw = Window->USE.C.Contents + (Window->YstSel + Window->USE.C.HSplit) * slen;
	while (hw >= Window->USE.C.TtyData->Split)
	    hw -= Window->USE.C.TtyData->Split - Window->USE.C.Contents;
	
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
		*Data++ = HWFONT(*hw), hw++;
	    ok &= SelectionStore(_SEL_MAGIC, NULL, slen * sizeof(hwfont), (byte *)sData);
	}
	
	if (hw >= Window->USE.C.TtyData->Split)
	    hw -= Window->USE.C.TtyData->Split - Window->USE.C.Contents;

	slen = Window->WLogic;
	for (y = Window->YstSel + 1; ok && y < Window->YendSel; y++) {
	    if (hw >= Window->USE.C.TtyData->Split)
		hw -= Window->USE.C.TtyData->Split - Window->USE.C.Contents;
	    Data = sData;
	    len = slen;
	    while (len--)
		*Data++ = HWFONT(*hw), hw++;
	    ok &= SelectionAppend(slen * sizeof(hwfont), (byte *)sData);
	}
	
	if (ok && Window->YendSel > Window->YstSel) {
	    if (hw >= Window->USE.C.TtyData->Split)
		hw -= Window->USE.C.TtyData->Split - Window->USE.C.Contents;
	    Data = sData;
	    len = slen = Window->XendSel + 1;
	    while (len--)
		*Data++ = HWFONT(*hw), hw++;
	    ok &= SelectionAppend(slen * sizeof(hwfont), (byte *)sData);
	}
	if (ok) NeedHW |= NEEDSelectionExport;
	return ok;
    }
    if (W_USE(Window, USEROWS)) {
	row Row;
	
	/* Gap not supported! */
	y = Window->YstSel;
	Row = Act(FindRow,Window)(Window, y);
	
	if (Row && Row->Text) {
	    if (y < Window->YendSel)
		slen = Row->Len - Window->XstSel;
	    else
		slen = Min2(Row->Len, Window->XendSel+1) - Min2(Row->Len, Window->XstSel);

	    ok &= SelectionStore(_SEL_MAGIC, NULL, slen * sizeof(hwfont),
				 (byte *)(Row->Text + Min2(Row->Len, Window->XstSel)));
	} else
	    ok &= SelectionStore(_SEL_MAGIC, NULL, 0, NULL);

	if (y < Window->YendSel || !Row || !Row->Text || Row->Len <= Window->XendSel)
	    ok &= _SelAppendNL();

	for (y = Window->YstSel + 1; ok && y < Window->YendSel; y++) {
	    if ((Row = Act(FindRow,Window)(Window, y)) && Row->Text)
		ok &= SelectionAppend(Row->Len * sizeof(hwfont), (byte *)Row->Text);
	    ok &= _SelAppendNL();
	}
	if (Window->YendSel > Window->YstSel) {
	    if (Window->XendSel >= 0 && (Row = Act(FindRow,Window)(Window, Window->YendSel)) && Row->Text)
		ok &= SelectionAppend(Min2(Row->Len, Window->XendSel+1) * sizeof(hwfont), (byte *)Row->Text);
	    if (!Row || !Row->Text || Row->Len <= Window->XendSel)
	    ok &= _SelAppendNL();
	}
	if (ok) NeedHW |= NEEDSelectionExport;
	return ok;
    }
    return FALSE;
}

byte CreateXTermMouseEvent(event_mouse *Event, byte buflen, byte *buf) {
    window W;
    udat Flags;
    udat Code = Event->Code;
    dat x = Event->X, y = Event->Y;
    byte len = 0;
	
    if (!(W = (window)Event->W) || !IS_WINDOW(W) || !W_USE(W, USECONTENTS) || !W->USE.C.TtyData)
	return len;
    
    Flags = W->USE.C.TtyData->Flags;
    
    if (Flags & TTY_REPORTMOUSE) {
	/* new-style reporting */

	/* when both TTY_REPORTMOUSE|TTY_REPORTMOUSE2 are set, also report motion */
	if (buflen < 9 || (Code == MOVE_MOUSE && !(Flags & TTY_REPORTMOUSE2)))
	    /* buffer too small, or nothing to report */
	    return len;
	
	/* report also button just pressed as down */
	if (isPRESS(Code))
	    Code |= HOLD_CODE(PRESS_N(Code));
	
	CopyMem("\033[5M", buf, 4);
	buf[4] = ' ' + ((Code & HOLD_ANY) >> HOLD_BITSHIFT) ;
	buf[5] = '!' + (x & 0x7f);
	buf[6] = '!' + ((x >> 7) & 0x7f);
	buf[7] = '!' + (y & 0x7f);
	buf[8] = '!' + ((y >> 7) & 0x7f);
	len = 9;
    } else if (Flags & TTY_REPORTMOUSE2) {
	/* classic xterm-style reporting */
	
	if (buflen < 6)
	    /* buffer too small! */
	    return len;
	
	CopyMem("\033[M", buf, 3);
    
	if (isSINGLE_PRESS(Code)) switch (Code & PRESS_ANY) {
	  case PRESS_LEFT: buf[3] = ' '; break;
	  case PRESS_MIDDLE: buf[3] = '!'; break;
	  case PRESS_RIGHT: buf[3] = '\"'; break;
	    /* WHEEL_REV and WHEEL_FWD supported only at release */
	}
	else if (isRELEASE(Code)) {
	    switch (Code & RELEASE_ANY) {
#ifdef HOLD_WHEEL_REV
	      case RELEASE_WHEEL_REV: buf[3] = '`'; break;
#endif
#ifdef HOLD_WHEEL_FWD
	      case RELEASE_WHEEL_FWD: buf[3] = 'a'; break;
#endif
	      default: buf[3] = '#'; break;
	    }
	} else
	    return len;

	buf[4] = '!' + x;
	buf[5] = '!' + y;
	len = 6;
	
    }
    return len;
}

void ResetBorderPattern(void) {
    msgport MsgP;
    widget W;
    
    for (MsgP = All->FirstMsgPort; MsgP; MsgP=MsgP->Next) {
	for (W = MsgP->FirstW; W; W = W->O_Next) {
	    if (IS_WINDOW(W))
		((window)W)->BorderPattern[0] = ((window)W)->BorderPattern[1] = NULL;
	}
    }
}

static gadget _PrevGadget(gadget G) {
    while (G->Prev) {
	G = (gadget)G->Prev;
	if (IS_GADGET(G))
	    return (gadget)G;
    }
    return (gadget)G->Prev;
}

static gadget _NextGadget(gadget G) {
    while (G->Next) {
	G = (gadget)G->Next;
	if (IS_GADGET(G))
	    return (gadget)G;
    }
    return (gadget)G->Next;
}



/* handle common keyboard actions like cursor moving and button navigation */
void FallBackKeyAction(window W, event_keyboard *EventK) {
    ldat NumRow, OldNumRow;
    gadget G, H;
    
    if ((G = (gadget)W->SelectW) && IS_GADGET(G)) switch (EventK->Code) {
      case TW_Escape:
	UnPressGadget(G, FALSE);
	W->SelectW = (widget)0;
	break;
      case TW_Return:
	UnPressGadget(G, TRUE);
	PressGadget(G);
	break;
      case TW_Up:
      case TW_Left:
	if ((H = _PrevGadget(G))) {
	    if (!(G->Flags & GADGETFL_TOGGLE))
		UnPressGadget(G, FALSE);
	    W->SelectW = (widget)H;
	    PressGadget(H);
	}
	break;
      case TW_Down:
      case TW_Right:
      case TW_Tab:
	if ((H = _NextGadget(G))) {
	    if (!(G->Flags & GADGETFL_TOGGLE))
		UnPressGadget(G, FALSE);
	    W->SelectW = (widget)H;
	    PressGadget(H);
	}
	break;
      default:
	break;
    } else if ((G = (gadget)W->FirstW) && IS_GADGET(G)) {
	PressGadget(G);
	W->SelectW = (widget)G;
    } else switch (EventK->Code) {
      case TW_Up:
	if (!W->HLogic)
	    break;
	OldNumRow=W->CurY;
	if (OldNumRow<MAXLDAT) {
	    if (!OldNumRow)
		NumRow=W->HLogic-(ldat)1;
	    else
		NumRow=OldNumRow-(ldat)1;
	    W->CurY=NumRow;
	    if (W->Flags & WINDOWFL_ROWS_SELCURRENT)
		DrawLogicWidget((widget)W, (ldat)0, OldNumRow, (ldat)MAXDAT-(ldat)2, OldNumRow);
	}
	else
	    W->CurY=NumRow=W->HLogic-(ldat)1;
	if (W->Flags & WINDOWFL_ROWS_SELCURRENT)
	    DrawLogicWidget((widget)W, (ldat)0, NumRow, (ldat)MAXDAT-(ldat)2, NumRow);
	UpdateCursor();
	break;
      case TW_Down:
	if (!W->HLogic)
	    break;
	OldNumRow=W->CurY;
	if (OldNumRow<MAXLDAT) {
	    if (OldNumRow>=W->HLogic-(ldat)1)
		NumRow=(ldat)0;
	    else
		NumRow=OldNumRow+(ldat)1;
	    W->CurY=NumRow;
	    if (W->Flags & WINDOWFL_ROWS_SELCURRENT)
		DrawLogicWidget((widget)W, (ldat)0, OldNumRow, (ldat)MAXDAT-(ldat)2, OldNumRow);
	}
	else
	    W->CurY=NumRow=(ldat)0;
	if (W->Flags & WINDOWFL_ROWS_SELCURRENT)
	    DrawLogicWidget((widget)W, (ldat)0, NumRow, (ldat)MAXDAT-(ldat)2, NumRow);
	UpdateCursor();
	break;
      case TW_Left:
	if (W->CurX > 0) {
	    W->CurX--;
	    UpdateCursor();
	}
	break;
      case TW_Right:
	if ((W_USE(W, USECONTENTS) && W->CurX < W->XWidth - 3) ||
	    (W_USE(W, USEROWS) && W->CurX < MAXLDAT - 1)) {
	    W->CurX++;
	    UpdateCursor();
	}
	break;
	default:
	break;
    }
}

/*
 * create a (malloced) array of non-space args
 * from arbitrary text command line
 * 
 * FIXME: need proper handling of double quotes:
 * "a b" is the string `a b' NOT the two strings `"a' `b"'
 * (same for single quotes, backslashes, ...)
 */
byte **TokenizeStringVec(uldat len, byte *s) {
    byte **cmd = NULL, *buf, c;
    uldat save_len, save_n, n = 0;
    
    /* skip initial spaces */
    while (len && ((c = *s) == '\0' || c == ' ')) {
	len--, s++;
    }
    save_len = len;
    
    if (len && (buf = AllocMem(len + 1))) {
	CopyMem(s, buf, len);
	buf[len] = '\0';
	
	/* how many args? */
	while (len) {
	    len--, c = *s++;
	    if (c && c != ' ') {
		n++;
		while (len && (c = *s) && c != ' ') {
		    len--, s++;
		}
	    }
	}
	if ((cmd = AllocMem((n + 1) * sizeof(byte *)))) {
	    save_n = n;
	    n = 0;
	    len = save_len;
	    s = buf;
	    
	    /* put args in cmd[] */
	    while (len) {
		len--, c = *s++;
		if (c && c != ' ') {
		    cmd[n++] = s - 1;
		    while (len && (c = *s) && c != ' ') {
			len--, s++;
		    }
		    *s = '\0'; /* safe, we did a malloc(len+1) */
		}
	    }
	    cmd[n] = NULL; /* safe, we did a malloc(n+1) */
	}
    }
    return cmd;
}

void FreeStringVec(byte **cmd) {
    if (cmd) {
	FreeMem(cmd[0]);
	FreeMem(cmd);
    }
}

/*
 * create a (malloced) array of non-space args
 * from arbitrary text command line
 * 
 * FIXME: need proper handling of double quotes:
 * "a b" is the string `a b' NOT the two strings `"a' `b"'
 * (same for single quotes, backslashes, ...)
 */
#if TW_SIZEOFHWFONT == 1
byte **TokenizeHWFontVec(uldat len, hwfont *s) {
    return TokenizeStringVec(len, s);
}
#else /* TW_SIZEOFHWFONT != 1 */
byte **TokenizeHWFontVec(uldat len, hwfont *s) {
    byte **cmd = NULL, *buf, *v;
    hwfont c;
    uldat save_len, save_n, n = 0, i;
    
    /* skip initial spaces */
    while (len && ((c = *s) == '\0' || c == ' ')) {
	len--, s++;
    }
    save_len = len;
    
    if (len && (buf = AllocMem(len + 1))) {
	for (i = 0; i < len; i++)
	    buf[i] = (byte)s[i];
	buf[len] = '\0';
	
	/* how many args? */
	while (len) {
	    len--, c = *s++;
	    if (c && c != ' ') {
		n++;
		while (len && (c = *s) && c != ' ') {
		    len--, s++;
		}
	    }
	}
	if ((cmd = AllocMem((n + 1) * sizeof(byte *)))) {
	    save_n = n;
	    n = 0;
	    len = save_len;
	    v = buf;
	    
	    /* put args in cmd[] */
	    while (len) {
		len--, c = *v++;
		if (c && c != ' ') {
		    cmd[n++] = v - 1;
		    while (len && (c = *v) && c != ' ') {
			len--, v++;
		    }
		    *v = '\0'; /* safe, we did a malloc(len+1) */
		}
	    }
	    cmd[n] = NULL; /* safe, we did a malloc(n+1) */
	}
    }
    return cmd;
}
#endif /* TW_SIZEOFHWFONT == 1 */

int unixFd;
uldat unixSlot;

static void TWDisplayIO(int fd, uldat slot) {
    struct sockaddr_un un_addr;
    int len = sizeof(un_addr);
    
    if ((fd = accept(fd, (struct sockaddr *)&un_addr, &len)) >= 0) {
	close(fd);
    }
}

static byte fullTWD[]="/tmp/.Twin:\0\0\0";
static byte envTWD[]="TWDISPLAY=\0\0\0\0";

/* set TWDISPLAY and create /tmp/.Twin:<x> */
byte InitTWDisplay(void) {
    struct sockaddr_un addr;
    int i, fd = NOFD;
    byte *arg0, ok;
    
    HOME = getenv("HOME");

    
    if ((unixFd = socket(AF_UNIX, SOCK_STREAM, 0)) >= 0) {
	
	WriteMem(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;

	for (i=0; i<0x1000; i++) {
	    sprintf(fullTWD+11, "%x", i);
	    CopyStr(fullTWD, addr.sun_path);

	    ok = bind(unixFd, (struct sockaddr *)&addr, sizeof(addr)) >= 0;
	    if (!ok) {
		Error(SYSCALLERROR);
		/* maybe /tmp/.Twin:<x> is already in use... */
		if (fd >= 0 || (fd = socket(AF_UNIX, SOCK_STREAM, 0)) >= 0) {
		    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) >= 0) {
			/*
			 * server is alive, try to grab another TWDISPLAY.
			 * also, we must junk `fd' since SOCK_STREAM sockets
			 * can connect() only once
			 */
			close(fd);
			fd = NOFD;
			continue;
		    }
		    /* either server is dead or is owned by another user */
		    if (unlink(fullTWD) >= 0) {
			/*
			 * if we have permission to delete it, we also had
			 * the permission to connect to it (hope).
			 * So it must have been be a stale socket.
			 * 
			 * Trying to delete a /tmp/.Twin:<x> entry we cannot
			 * connect to wreaks havoc if you mix this twin server
			 * with older ones, but having two different servers
			 * installed on one system should be rare enough.
			 */
			ok = bind(unixFd, (struct sockaddr *)&addr, sizeof(addr)) >= 0;
		    }
		}
	    }
	    if (ok) {
		if (chmod(fullTWD, 0700) >= 0 &&
		    listen(unixFd, 3) >= 0 &&
		    fcntl(unixFd, F_SETFD, FD_CLOEXEC) >= 0) {
		    
		    if ((unixSlot = RegisterRemoteFd(unixFd, TWDisplayIO)) != NOSLOT) {
			
			if (fd != NOFD)
			    close(fd);
			TWDisplay = fullTWD+10;
			lenTWDisplay = LenStr(TWDisplay);
			CopyMem(TWDisplay, envTWD+10, lenTWDisplay);
#if defined(HAVE_SETENV)
			setenv("TWDISPLAY",TWDisplay,1);
			setenv("TERM","linux",1);
#elif defined(HAVE_PUTENV)
			putenv(envTWD);
			putenv("TERM=linux");
#endif
			if ((arg0 = AllocMem(LenStr(TWDisplay) + 6))) {
			    sprintf(arg0, "twin %s", TWDisplay);
			    SetArgv0(main_argv, main_argv_usable_len, arg0);
			    FreeMem(arg0);
			}
			return TRUE;
		    }
		} else
		    Error(SYSCALLERROR);
		close(unixFd);
	    }
	}
    }
    if (fd != NOFD)
	close(fd);
    printk("twin: failed to create any /tmp/.Twin* socket: %."STR(SMALLBUFF)"s\n", ErrStr);
    printk("      possible reasons: either /tmp not writable, or all TWDISPLAY already in use,\n"
	   "      or too many stale /tmp/.Twin* sockets. Aborting.\n");
    return FALSE;
}

/* unlink /tmp/.Twin<TWDISPLAY> */
void QuitTWDisplay(void) {
    unlink(fullTWD);
}




/* suid/sgid privileges related functions */

static e_privilege Privilege;
static gid_t tty_grgid;
uid_t Uid, EUid;

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

byte CheckPrivileges(void) {
    Uid = getuid();
    EUid = geteuid();
    
    (void)get_tty_grgid();
    
    if (GainRootPrivileges() >= 0)
	Privilege = suidroot;
    else if (tty_grgid != (gid_t)-1 && GainGroupPrivileges(tty_grgid) >= 0)
	Privilege = sgidtty;
    else
	Privilege = none;
    
    DropPrivileges();
    
    return Privilege;
}

void GainPrivileges(void) {
    if (Privilege == suidroot)
	GainRootPrivileges();
    else if (Privilege == sgidtty)
	GainGroupPrivileges(get_tty_grgid());
}

static void SetEnvs(struct passwd *p) {
    byte buf[BIGBUFF];
    
    chdir(HOME = p->pw_dir);
#if defined(HAVE_SETENV)
    setenv("HOME", HOME, 1);
    setenv("SHELL", p->pw_shell, 1);
    setenv("LOGNAME", p->pw_name, 1);
    sprintf(buf, "/var/mail/%s", p->pw_name); setenv("MAIL", buf, 1);
#elif defined(HAVE_PUTENV)
    sprintf(buf, "HOME=%s", HOME); putenv(buf);
    sprintf(buf, "SHELL=%s", p->pw_shell); putenv(buf);
    sprintf(buf, "LOGNAME=%s", p->pw_name); putenv(buf);
    sprintf(buf, "MAIL=/var/mail/%s", p->pw_name); putenv(buf);
#endif
}

byte SetServerUid(uldat uid, byte privileges) {
    msgport WM_MsgPort;
    struct passwd *p;
    byte ok = FALSE;
    
    if (flag_secure && uid == (uldat)(uid_t)uid && Uid == 0 && EUid == 0) {
	if ((WM_MsgPort = Ext(WM,MsgPort))) {
	    if ((p = getpwuid(uid)) && p->pw_uid == uid &&
		chown(fullTWD, p->pw_uid, p->pw_gid) >= 0
#ifdef HAVE_INITGROUPS
		&& initgroups(p->pw_name, p->pw_gid) >= 0
#endif
		) {

		
		switch (privileges) {
		  case none:
		    ok = setgid(p->pw_gid) >= 0 && setuid(uid) >= 0;
		    break;
		  case sgidtty:
		    ok = setregid(p->pw_gid, tty_grgid) >= 0 && setuid(uid) >= 0;
		    break;
		  case suidroot:
		    ok = setgid(p->pw_gid) >= 0 && setreuid(uid, 0) >= 0;
		    break;
		  default:
		    break;
		}
		if (ok && (uid == 0 || CheckPrivileges() == privileges)) {
		    flag_secure = 0;
		    SetEnvs(p);
		    if ((ok = Ext(Socket,InitAuth)())) {
			/*
			 * it's time to execute .twenvrc.sh and read its output to set
			 * environment variables (mostly useful for twdm)
			 */
			RunTwEnvRC();
			
			/* tell the WM to restart itself (so that it reads user's .twinrc) */
			SendControlMsg(WM_MsgPort, MSG_CONTROL_OPEN, 0, NULL);
			return TRUE;
		    }
		} else
		    ok = FALSE;
		
		if (!ok) {
		    flag_secure = 1;
		    if (setuid(0) < 0 || setgid(0) < 0 ||
			chown(fullTWD, 0, 0) < 0) {
			/* tried to recover, but screwed up uids too badly. */
			printk("twin: failed switching to uid %u: %."STR(SMALLBUFF)"s\n", uid, strerror(errno));
			printk("twin: also failed to recover. Quitting NOW!\n");
			Quit(0);
		    }
		    SetEnvs(getpwuid(0));
		}
	    }
	    printk("twin: failed switching to uid %u: %."STR(SMALLBUFF)"s\n", uid, strerror(errno));
	}
    } else
	printk("twin: SetServerUid() can be called only if started by root with \"-secure\".\n");
    return FALSE;
}


/*
 * search for a file relative to HOME, to CONF_DESTDIR or as path
 * 
 * this for example will search "foo"
 * as "{HOME}/foo", "{CONF_DESTDIR}/lib/twin/foo" or plain "foo"
 */
byte *FindFile(byte *name, uldat *fsize) {
    byte *path;
    byte CONST *dir;
    byte CONST *search[3];
    int i, min_i, max_i, len, nlen = strlen(name);
    struct stat buf;
    
    search[0] = HOME;
    search[1] = conf_destdir_lib_twin;
    search[2] = "";
    
    if (flag_secure)
	min_i = max_i = 1; /* only conf_destdir_lib_twin */
    else
	min_i = 0, max_i = 2;

    for (i = min_i; i <= max_i && (dir = search[i]); i++) {
	len = strlen(dir);
	if ((path = AllocMem(len + nlen + 2))) {
	    sprintf(path, "%s%s%s", dir, *dir ? "/" : "", name);
	    if (stat(path, &buf) == 0) {
		if (fsize)
		    *fsize = buf.st_size;
		return path;
	    }
	    FreeMem(path);
	}
    }
    return NULL;
}

/*
 * read data from infd and set environment variables accordingly
 */
static void ReadTwEnvRC(int infd) {
    byte buff[BIGBUFF], *p = buff, *end, *q, *eq;
    int got, left = BIGBUFF;
    for (;;) {
	do {
	    got = read(infd, p, left);
	} while (got == -1 && errno == EINTR);
	
	if (got <= 0)
	    break;
	
	end = p + got;
	p = buff;
	    
	while ((eq = memchr(p, '=', end - p)) &&
	       (q = memchr(eq, '\n', end - eq))) {
		
	    *q++ = '\0';
#if defined(HAVE_SETENV)
	    *eq++ = '\0';
	    setenv(p, eq, 1);
#elif defined(HAVE_PUTENV)
	    putenv(p);
#endif
	    p = q;
	}
	left = end - p;
	if (left == BIGBUFF)
	    /* line too long! */
	    left = 0;
	    
	memmove(buff, p, left);
	p = buff + left;
	left = BIGBUFF - left;
    }
}

/*
 * execute .twenvrc.sh <dummy> and read its output to set
 * environment variables (mostly useful for twdm)
 */
void RunTwEnvRC(void) {
    byte *path;
    uldat len;
    int fds[2];
    
    if (flag_envrc != 1)
	return;
    
    if (flag_secure == 0) {
	flag_envrc = 0;
	
	if ((path = FindFile(".twenvrc.sh", &len))) {
	    if ((pipe(fds) >= 0)) {
		switch (fork()) {
		  case -1: /* error */
		    close(fds[0]);
		    close(fds[1]);
		    printk("twin: RunTwEnvRC(): fork() failed: %."STR(SMALLBUFF)"s\n", strerror(errno));
		    break;
		  case 0:  /* child */
		    close(fds[0]);
		    if (fds[1] != 2) {
			close(2);
			dup2(fds[1], 2);
			close(fds[1]);
		    }
		    close(1);
		    dup2(0, 1);
		    execl(path, path, "dummy", NULL);
		    exit(0);
		    break;
		  default: /* parent */
		    close(fds[1]);
		    ReadTwEnvRC(fds[0]);
		    close(fds[0]);
		    break;
		}
	    } else
		printk("twin: RunTwEnvRC(): pipe() failed: %."STR(SMALLBUFF)"s\n", strerror(errno));
	} else
	    printk("twin: RunTwEnvRC(): .twenvrc.sh: File not found\n", strerror(errno));
    } else
	printk("twin: RunTwEnvRC(): delaying .twenvrc.sh execution until secure mode ends.\n");
}


/*
 * encode POS_* position, position detail, active flag, pressed flag,
 * into 'extra' byte field inside hwattr
 * 
 * not all bits are preserved... this is just
 * a fair effort that covers most cases
 * 
 * this is used for example to decide where each icon
 * is placed inside hw_gfx_themes/<*>.xpm theme files.
 */
hwattr EncodeToHWAttrExtra(byte pos, byte detail, byte active, byte pressed) {
#define pitch 15
    byte o12 = active ? pressed ? 2 : 1 : 0;
    byte sides = (4 + o12) * pitch;
    byte scrollx = 9 + (4 + o12) * pitch;
    byte scrolly = 12 + o12;
    
    switch (pos) {
      case POS_TITLE:
	return 4 + sides;
      case POS_SIDE_LEFT:
	return 3 + sides;
      case POS_SIDE_UP:
	return detail + sides;
      case POS_SIDE_RIGHT:
	return 5 + sides;
      case POS_SIDE_DOWN:
	return 6 + detail + sides;
	
      case POS_BUTTON_RESIZE:
	return (detail ? 1 : 0) + (pressed ? 3 : 2) * pitch;
	
      case POS_X_BAR_BACK:
      case POS_X_BAR_FWD:
	return scrollx;
      case POS_X_TAB:
	return 1 + scrollx;
      case POS_X_ARROW_BACK:
	return 2 + scrollx;
      case POS_X_ARROW_FWD:
	return 3 + scrollx;
	
      case POS_Y_BAR_BACK:
      case POS_Y_BAR_FWD:
	return scrolly;
      case POS_Y_TAB:
	return scrolly + pitch;
      case POS_Y_ARROW_BACK:
	return scrolly + 2 * pitch;
      case POS_Y_ARROW_FWD:
	return scrolly + 3 * pitch;
      case POS_INSIDE:
	return 1 + pitch;
      case POS_MENU:
	return 1;
      case POS_ROOT:
	return pitch;
      default:
	if (pos < POS_TITLE)
	    return 2 + pos * 2 + (detail ? 1 : 0) + ((pressed ? 1 : 0) + (pos >= 5)) * pitch;
	break;
    }
    return 0;
#undef pitch
}


/* finally, functions to manage Ids */

static obj *IdList[magic_n];
static uldat IdSize[magic_n], IdTop[magic_n], IdBottom[magic_n];

INLINE uldat IdListGrow(byte i) {
    uldat oldsize, size;
    obj *newIdList;
    
    oldsize = IdSize[i];
    if (oldsize >= MAXID || i == obj_magic_id || i == all_magic_id)
	return NOSLOT;

    size = oldsize < SMALLBUFF/3 ? SMALLBUFF/2 : oldsize + (oldsize>>1);
    if (size > MAXID)
	size = MAXID;
    
    if (!(newIdList = (obj *)ReAllocMem(IdList[i], size*sizeof(obj))))
	return NOSLOT;
    
    WriteMem(newIdList+oldsize, 0, (size-oldsize)*sizeof(obj));
    
    IdList[i] = newIdList;
    IdSize[i] = size;
    
    return oldsize;
}

INLINE void IdListShrink(byte i) {
    obj *newIdList;
    uldat size = Max2(BIGBUFF, IdTop[i] << 1);
    
    if (size < IdSize[i] && (newIdList = (obj *)ReAllocMem(IdList[i], size*sizeof(obj)))) {
	IdList[i] = newIdList;
	IdSize[i] = size;
    }
}

INLINE uldat IdListGet(byte i) {
    if (IdBottom[i] == IdSize[i])
	return IdListGrow(i);
    
    return IdBottom[i];
}

INLINE byte _AssignId(byte i, obj Obj) {
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
	return TRUE;
    }
    Error(NOTABLES);
    return FALSE;
}

INLINE void _DropId(byte i, obj Obj) {
    uldat Id = Obj->Id & MAXID, j;

    if (Id < IdTop[i] && IdList[i][Id] == Obj /* paranoia */) {
	Obj->Id = NOID;
	IdList[i][Id] = (obj)0;
	if (IdBottom[i] > Id)
	    IdBottom[i] = Id;
	for (j = IdTop[i] - 1; j > IdBottom[i]; j--)
	    if (IdList[i][j])
		break;
	IdTop[i] = (j == IdBottom[i]) ? j : j + 1;
	
	if (IdSize[i] > (IdTop[i] << 4) && IdSize[i] > BIGBUFF)
	    IdListShrink(i);
    }
}
    
byte AssignId(CONST fn_obj Fn_Obj, obj Obj) {
    byte i;
    if (Obj) switch (Fn_Obj->Magic) {
      case obj_magic:
	/* 'obj' is just a template type, you can't create one */
	break;
      case row_magic:
      case module_magic:
      case display_hw_magic:
      case msg_magic:
	/*
	 * We don't use Ids for rows and msgs as we expect to create *lots* of them.
	 * It's unsafe to allow modules and display_hw access remotely,
	 * so no Ids for them too.
	 */
        Obj->Id = Fn_Obj->Magic;
	return TRUE;
      case widget_magic:
      case gadget_magic:	
      case window_magic:
      case screen_magic:
      case group_magic:
      case menuitem_magic:
      case menu_magic:
      case msgport_magic:
      case mutex_magic:
      case extension_magic:
	i = Fn_Obj->Magic >> magic_shift;
	return _AssignId(i, Obj);
      default:
	break;
    }
    return FALSE;
}

void DropId(obj Obj) {
    byte i = 0;
    if (Obj && Obj->Fn) switch (Obj->Fn->Magic) {
      case obj_magic:
	/* 'obj' is just a template type, you can't create one */
	break;
      case row_magic:
      case module_magic:
      case display_hw_magic:
      case msg_magic:
	/* we don't use Ids for rows and msgs as we expect to create *lots* of them */
	/* it's unsafe to allow modules access remotely, so no Ids for them too */
	Obj->Id = NOID;
	break;
      case widget_magic:
      case gadget_magic:	
      case window_magic:
      case screen_magic:
      case group_magic:
      case menuitem_magic:
      case menu_magic:
      case msgport_magic:
      case mutex_magic:
	i = Obj->Fn->Magic >> magic_shift;
	if (i == (Obj->Id >> magic_shift))
	    _DropId(i, Obj);
	break;
      default:
	break;
    }
}

static obj IdList_all[1];

byte AssignId_all(all Obj) {
    byte i = all_magic_id;
    
    if (!IdList[i]) {
	IdList[i] = IdList_all;
	IdSize[i] = 1;
	return _AssignId(i, (obj)Obj);
    }
    return FALSE;
}

obj Id2Obj(byte i, uldat Id) {
    byte I = Id >> magic_shift;
    
    if (i < magic_n && I < magic_n) {
	/* everything is a valid (obj) */
	/* gadgets, windows, screens are valid (widget) */
	/* menuitems are valid (row) */
	if (i == I || i == obj_magic_id ||
	    (i == widget_magic_id && (I == gadget_magic_id || I == window_magic_id || I == screen_magic_id)) ||
	    (i == row_magic_id && I == menuitem_magic_id)) {
	    
	    Id &= MAXID;
	    if (Id < IdTop[I])
		return IdList[I][Id];
	}
    }
    return (obj)0;
}
